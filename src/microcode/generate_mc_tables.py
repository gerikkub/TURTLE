#!/usr/bin/env python3

import json
import sys
import math
import verilog_gen

# Checks if an entry in the control signal json file is correct
def isValidSignalEntry(cs_entry, name):

    if not "type" in cs_entry:
        print('"type" field not found in entry: {0}'.format(name))
        return False

    if cs_entry["type"] == "num":
        if not "bits" in cs_entry:
            print('"bits" field not found in entry: {0}'.format(name))
            return False

        if cs_entry["bits"] is int:
                print('"bits" field has incorrect type {0} in entry: {1}'.format(type(cs_entry["bits"]), name))
                return False

        bits = cs_entry["bits"]

        if "default" in cs_entry:
            if cs_entry["default"] is int:
                print('"default" field has incorrect type {0} in entry: {1}'.format(type(cs_entry["default"], name)))
                return False

            value = cs_entry["default"]
            bits = cs_entry["bits"]

            if (value < 0 or value > (2**bits)):
                print('"default" field is out-of-range [0,{0}] in entry: {1}'.format((2**bits)-1, name))
                return False

    elif cs_entry["type"] == "mux":
        if not "values" in cs_entry:
            print('"values" field not found in entry: {0}'.format(name))
            return False

        if cs_entry["values"] is list:
            print('"values" field has incorrect type {0} in entry: {1}'.format(type(cs_entry["values"]), name))
            return False
        
        values = cs_entry["values"]

        if ("default" in cs_entry):
                if cs_entry["default"] is unicode:
                    print('"default" field has incorrect type {0} in entry: {1}'.format(type(cs_entry["default"]), name))
                    return False

                if not cs_entry["default"] in values:
                    print('"default" value is not found in values array in entry: {0}'.format(name))
                    return False
    else:
        print('Unknown type {0} in entry: {1}'.format(cs_entry["type"], name))
        return False

    return True

# Generates the signal->bits, and the signal->position tables as well
# as the default_bitfield value used by the subop table
def generateSignalTable(cs_json_name):

    # Load the JSON file
    cs_data = open(cs_json_name)
    cs_json = json.load(cs_data)

    default_bitfield = 0

    # Holds the number of bits needed for a signal
    bits = int(0)

    # Keeps track of the current position in the bitfield
    # for a signal
    position = int(0)

    # Dictionary containing the signal name as the key and the
    # number of bits used by the signal as the value
    cs_bits_dict = {}

    # Dictionary containing the signal name as the key and the
    # signal's position on the bitfield as the value
    cs_position_dict = {}

    # Dictionary containing the signal name as the key and the
    # signal's values list as the value. If the type in "num" then
    # the signal will not have an entry in the dictionary
    cs_values_dict = {}


    for cs_entry_name in cs_json:

        cs_entry = cs_json[cs_entry_name]

        if not isValidSignalEntry(cs_entry, cs_entry_name):
            return False, cs_bits_dict, cs_position_dict, cs_values_dict, default_bitfield

        if (cs_entry["type"] == "num"):
            # The number of necessary bits is provided in the entry
            bits = cs_entry["bits"]

            # Modify the default_bitfield if applicable
            if "default" in cs_entry:
                default_bitfield = default_bitfield | (cs_entry["default"] << int(position))


        elif (cs_entry["type"] == "mux"):
            # Calculates the number of necessary bits from the "values" array
            bits = int(math.ceil(math.log(len(cs_entry["values"]), 2)))

            # Modify the default_bitfield if applicable
            if "default" in cs_entry:
                def_value = cs_entry["values"].index(cs_entry["default"])

                default_bitfield = default_bitfield | (def_value << position)

            cs_values_dict[cs_entry_name] = cs_entry["values"]

        else:
            assert False, "Unknown signal type {0} for signal {1}".format((cs_entry["type"], cs_entry))

        cs_bits_dict[cs_entry_name] = int(bits)
        cs_position_dict[cs_entry_name] = int(position)
        
        position = position + bits

    return True, cs_bits_dict, cs_position_dict, cs_values_dict, default_bitfield

def getSignalValue(signal_name, signal_value, cs_bits, cs_values):

    # Holds the number value corresponding to the signal_value
    ret_num = 0

    if signal_name in cs_values:
        # The signal type is mux

        if not signal_value in cs_values[signal_name]:
            print("Unknown signal value {0} for signal {1}".format(signal_value, signal_name))
            return False, ret_num


        # Gets singal_values's position in the signal array. The position
        # corresponds to the signals value
        ret_num = cs_values[signal_name].index(signal_value)

    else:
        # This signal type is num

        if (signal_value > (2**cs_bits[signal_name]) - 1) or (signal_value < 0):
            print("Invalid signal value {0} for signal {1}".format(signal_value, signal_name))
            return False, ret_num

        ret_num = signal_value

    return True, ret_num
            
         


# Generates a dictionary with the subop name as the key and the position in the
# subop table as the value
def generateSubopTable(subop_json, cs_bits, cs_position, cs_values, default_bitfield):
    
    subop_bitfield_dict = {}
    subop_position_dict = {}

    position = int(0)

    for subop_entry_name in subop_json:
        
        subop_entry = subop_json[subop_entry_name]

        bitfield = default_bitfield

        for signal_name in subop_entry:
            if not signal_name in cs_bits:
                print("Unknown signal name {0} in subop {1}".format(signal_name, subop_entry_name))
                return False, subop_bitfield_dict, subop_position_dict

            sig_bits = cs_bits[signal_name]
            sig_position = cs_position[signal_name]
            
            # Creates a mask to insert the correct signal bits
            # For example, if (sig_bits = 2) and (sig_position = 5)
            # Then sig_mask = 0b1100000
            sig_mask = ~ (((2**sig_bits) - 1) << sig_position)

            bitfield = bitfield & sig_mask

            status, sig_value = getSignalValue(signal_name, subop_entry[signal_name], cs_bits, cs_values)

            if status == False:
                return False, subop_bitfield_dict, subop_position_dict

            # Set the appropriate bits in the bitfield
            bitfield = bitfield | (sig_value << sig_position)

        # Write the bitfield to the subop_bitfield_dict
        subop_bitfield_dict[subop_entry_name] = bitfield

        # Write the position to the subop_position_dict
        subop_position_dict[subop_entry_name] = position

        position = position + 1

    return True, subop_bitfield_dict, subop_position_dict

def addSubopsToOpcodeList(opcode_list, subop_list, subop_position_dict):

    for subop_name in subop_list:
        if not subop_name in subop_position_dict:
            print("Subop {0} not found in subop list".format(subop_name))
            return False

        opcode_list.append(subop_position_dict[subop_name])

    return True

# Generates opcode_list (a list of subops) and the position of
# the opcodes in opcode_position_dict
def generateOpcodeTable(opcode_json, subop_position_dict):

    opcode_list = []
    opcode_position_dict = {}

    position = 0

    if not "inst_fetch" in opcode_json:
        print('Special opcode "inst_fetch" not found in opcode list')
        return False, opcode_list, opcode_position_dict
    
    addSubopsToOpcodeList(opcode_list, opcode_json["inst_fetch"]["subops"], subop_position_dict)

    opcode_position_dict["inst_fetch"] = 0
    position = len(opcode_json["inst_fetch"]["subops"])
    del opcode_json["inst_fetch"]

    if not "illegal_op" in opcode_json:
        print('Special opcode "illegal_op" not found in opcode list')
        return False, opcode_list, opcode_position_dict

    for opcode_name in opcode_json:
        addSubopsToOpcodeList(opcode_list, opcode_json[opcode_name]["subops"], subop_position_dict)
        opcode_position_dict[opcode_name] = position

        position = position + len(opcode_json[opcode_name]["subops"])

    return True, opcode_list, opcode_position_dict

def generateMetadataDict(metadata_json):

    metadata_opcode_dict = {};

    for mdata_name in metadata_json:

        position = int(metadata_json[mdata_name]["opcode_position"], 16)
        if (metadata_json[mdata_name]["opcode_position"] == "00"):
            print("Found 149: {:}".format((metadata_json[mdata_name])))
         
        metadata_opcode_dict[position] = metadata_json[mdata_name]["opcode"]
        
    return True, metadata_opcode_dict

def writeMetadataVector(metadata_opcode_dict, opcode_position_dict):

    ill_position = opcode_position_dict["illegal_op"]

    metadata_vector = ['%X' % ill_position for x in range(0x1F)]


    for mdata_pos in metadata_opcode_dict:
        metadata_vector[mdata_pos] = '%X' % opcode_position_dict[metadata_opcode_dict[mdata_pos]]

    mdata_file = open("metadata_vector.txt", "w")

    mdata_file.write("\n".join(metadata_vector))


def writeOpcodeVector(opcode_list):
    
    opcode_vector = map((lambda x: '%X' % x), opcode_list)

    opcode_file = open("opcode_vector.txt", "w")

    opcode_file.write("\n".join(opcode_vector))

def writeSubopVector(subop_bitfield_dict, subop_position_dict):

    subop_vector = ['0' for x in range(len(subop_position_dict))]

    for subop in subop_bitfield_dict:
        subop_vector[subop_position_dict[subop]] = '%X' % subop_bitfield_dict[subop]

    subop_file = open("subop_vector.txt", "w")

    subop_file.write("\n".join(subop_vector))


def main():

    if len(sys.argv) < 3:
        print("Usage: " + sys.argv[0] + " control_signals.json microcode.json")
        sys.exit(1)

    status, cs_bits_dict, cs_position_dict, cs_values_dict, default_bitfield = generateSignalTable(sys.argv[1])

    if status == False:
        return

    print(cs_position_dict)


    print(default_bitfield)

    microcode_data = open(sys.argv[2])
    microcode_json = json.load(microcode_data)

    status, subop_bitfield_dict, subop_position_dict = generateSubopTable(microcode_json["subop"], cs_bits_dict, cs_position_dict, cs_values_dict, default_bitfield)

    if status == False:
        return

    status, opcode_list, opcode_position_dict = generateOpcodeTable(microcode_json["opcode"], subop_position_dict)

    if status == False:
        return

    status, metadata_opcode_dict = generateMetadataDict(microcode_json["metadata"])

    if status == False:
        return

    print(metadata_opcode_dict)


    writeMetadataVector(metadata_opcode_dict, opcode_position_dict)
    writeOpcodeVector(opcode_list)
    writeSubopVector(subop_bitfield_dict, subop_position_dict)

    verilog_gen.writeMicrocodeVerilog("microcode_mod.v", len(opcode_list), len(subop_position_dict), cs_position_dict, cs_bits_dict)

    verilog_gen.writeControlSignalVerilog("cs_mapper_mod.v", cs_bits_dict, cs_position_dict)


if __name__ == "__main__":
    main()






