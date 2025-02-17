
import math

mc_verilog_str = '`timescale 1ns / 1ps \n\
 \n\
module microcode_mod( \n\
    input [___OPCODE_SIZE_LOG2___:0]opcode, \n\
    output [___CONTROL_SIGNAL_BW___:0]control_signals \n\
    ); \n\
 \n\
    parameter opcode_table_size = ___OPCODE_SIZE___; \n\
 \n\
    parameter subop_table_size = ___SUBOP_SIZE___; \n\
 \n\
    reg [___OPCODE_BW___:0]opcode_table[0:opcode_table_size-1]; \n\
    reg [___CONTROL_SIGNAL_BW___:0]subop_table[0:subop_table_size-1]; \n\
 \n\
    initial $readmemh("src/opcode_vector.txt", opcode_table); \n\
    initial $readmemh("src/subop_vector.txt", subop_table); \n\
     \n\
    wire [___OPCODE_BW___:0]opcode_table_out; \n\
    wire [___CONTROL_SIGNAL_BW___:0]subop_table_out; \n\
 \n\
    assign opcode_table_out = opcode_table[opcode]; \n\
 \n\
    assign subop_table_out = subop_table[opcode_table_out]; \n\
 \n\
    assign control_signals = subop_table_out; \n\
 \n\
 \n\
 endmodule'

cs_mapper_header = '`timescale 1ns / 1ps \n\
 \n\
module cs_mapper_mod( \n\
 \n\
'

cs_mapper_output = 'output ___SIGNAL_NAME___,'
cs_mapper_output_vector = 'output [___SIGNAL_BW___:0]___SIGNAL_NAME___,'

cs_mapper_mid = 'input [___CONTROL_SIGNAL_BW___:0]control_signals \n\
        ); \n\
        \n'

cs_mapper_assign = 'assign ___SIGNAL_NAME___ = control_signals[___SIGNAL_OFFSET___];'
cs_mapper_assign_vector = 'assign ___SIGNAL_NAME___ = control_signals[___SIGNAL_OFFSET_HIGH___:___SIGNAL_OFFSET___];'

cs_mapper_footer = 'endmodule'





def writeMicrocodeVerilog(filename, num_opcodes, num_subops, cs_position_dict, cs_bits_dict):

    global mc_verilog_str

    opcode_size_log2 = int(math.ceil(math.log(num_opcodes, 2)))
    opcode_table_bitwidth = int(math.ceil(math.log(num_subops, 2)))

    v = list(cs_position_dict.values())
    k = list(cs_position_dict.keys())
    last_signal = k[v.index(max(v))]

    cs_bitwidth = cs_bits_dict[last_signal] + cs_position_dict[last_signal]


    local_mc_verilog_str = mc_verilog_str

    local_mc_verilog_str = local_mc_verilog_str.replace("___OPCODE_SIZE_LOG2___", "%d" % (opcode_size_log2 - 1))
    local_mc_verilog_str = local_mc_verilog_str.replace("___OPCODE_BW___", "%d" % (opcode_table_bitwidth - 1))
    local_mc_verilog_str = local_mc_verilog_str.replace("___OPCODE_SIZE___", "%d" % (num_opcodes))
    local_mc_verilog_str = local_mc_verilog_str.replace("___SUBOP_SIZE___", "%d" % (num_subops))
    local_mc_verilog_str = local_mc_verilog_str.replace("___CONTROL_SIGNAL_BW___", "%d" % (cs_bitwidth - 1))

    f = open(filename, "w")
    f.write(local_mc_verilog_str)

def writeControlSignalVerilog(filename, cs_bits_dict, cs_position_dict):

    global cs_mapper_header
    global cs_mapper_output
    global cs_mapper_output_vector
    global cs_mapper_mid
    global cs_mapper_assign
    global cs_mapper_assign_vector
    global cs_mapper_footer
    
    f = open(filename, "w")


    # Write the header
    f.write(cs_mapper_header)

    # Write the input signals
    for signal in cs_bits_dict:
        if cs_bits_dict[signal] > 1:
            local_output_str = cs_mapper_output_vector.replace("___SIGNAL_BW___", "%d" % (cs_bits_dict[signal] - 1))
        else:
            local_output_str = cs_mapper_output

        local_output_str = local_output_str.replace("___SIGNAL_NAME___", signal)
        f.write(local_output_str + '\n')

    v = list(cs_position_dict.values())
    k = list(cs_position_dict.keys())
    last_signal = k[v.index(max(v))]

    bitwidth = cs_bits_dict[last_signal] + cs_position_dict[last_signal]

    # Write the output signal
    f.write(cs_mapper_mid.replace("___CONTROL_SIGNAL_BW___", "%d" % (bitwidth - 1)))

    # Write the signal assign statements
    for signal in cs_bits_dict:
        if cs_bits_dict[signal] > 1:
            local_output_str = cs_mapper_assign_vector.replace("___SIGNAL_OFFSET_HIGH___", "%d" % (cs_position_dict[signal] + cs_bits_dict[signal] - 1))
        else:
            local_output_str = cs_mapper_assign

        local_output_str = local_output_str.replace("___SIGNAL_NAME___", signal)
        local_output_str = local_output_str.replace("___SIGNAL_OFFSET___", "%d" % cs_position_dict[signal])

        f.write(local_output_str + '\n')

    # Write the footer
    f.write(cs_mapper_footer)








