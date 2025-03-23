
#include <print>
#include <vector>
#include <boost/program_options.hpp>
#include <fstream>

#include <elfio/elfio.hpp>

#include "dut.hpp"

namespace po = boost::program_options;
using namespace ELFIO;

struct hex_uint32 {
public:
    hex_uint32(std::string s) :
        n(std::stol(s,nullptr,0)) {}
    hex_uint32(uint32_t n) : n(n) {}
    uint32_t n;
};
void validate(boost::any& v,
              const std::vector<std::string>& values,
              hex_uint32* target_type, int) {

    try {
        v = boost::any(hex_uint32(values[0]));
    } catch (std::exception e){
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}

bool validate_elf(const elfio& elf) {

    if (elf.get_class() != ELFCLASS32) { std::println(stderr, "Invalid ELF class"); return false; }

    if (elf.get_encoding() != ELFDATA2LSB) { std::println(stderr, "Invalid ELF encoding"); return false; }

    if (elf.segments.size() == 0) { std::println(stderr, "No ELF Segments"); return false; }

    const uint32_t reset_vector = elf.get_entry();
    bool have_reset_segment = false;
    for (int idx = 0; idx < elf.segments.size(); idx++) {
        const segment* pseg = elf.segments[idx];
        uint32_t v_start = pseg->get_virtual_address();
        uint32_t v_end = v_start + pseg->get_memory_size();
        if (v_start <= reset_vector &&
            v_end > reset_vector) {
            if (pseg->get_type() != SHT_PROGBITS ||
                (pseg->get_flags() & SHF_EXECINSTR) == 0) {
                std::println(stderr, "Segment with reset vector is not marked executable");
                return false;
            }
            have_reset_segment = true;
            break;
        }
    }

    if (!have_reset_segment) { std::println(stderr, "No Segment containing the reset vector {:08x}", reset_vector); return false; }

    return true;
}

int main(int argc, char** argv) {

    po::options_description desc("Allowed options");
    desc.add_options()
        ("semihost_address,s", po::value<hex_uint32>(), "Semihosting Address")
        ("clock_ticks,c", po::value<int>()->default_value(-1), "Run for clock_ticks cycles")
        ("trace_file", po::value<std::string>(), "Trace filename")
        ("verbose,v", "Verbose mode")
        ("help", "Help message")
        ("input_file", po::value<std::string>(), "input ELF file");

    po::positional_options_description p;
    p.add("input_file", -1);

    po::variables_map args;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).run(), args);
    po::notify(args);

    if (args.count("help")) {
        std::cerr << desc << std::endl;
        return 1;
    }

    const uint32_t semihost_address = (args.count("semihost_address") > 0) ?
                                      args["semihost_address"].as<hex_uint32>().n :
                                      0xABCDE000;
    const int clock_ticks = args["clock_ticks"].as<int>();
    const std::string input_file = args["input_file"].as<std::string>();
    const bool verbose = args.count("verbose") > 0;

    // Load ELF file
    elfio elf;
    if (!elf.load(input_file)) {
        std::println(stderr, "Unable to open file {}", input_file);
        return 1;
    }

    uint32_t reset_vector = elf.get_entry();

    if (!validate_elf(elf)) {
        std::println(stderr, "Invalid ELF file {}", input_file);
        return 1;
    }

    // Create DUT
    Dut dut(reset_vector, semihost_address);
    if (args.count("trace_file")) {
        dut.enableTrace(args["trace_file"].as<std::string>());
    }


    // Populate program segments
    for (int idx = 0; idx < elf.segments.size(); idx++) {
        const segment* pseg = elf.segments[idx];
        if (pseg->get_type() != SHT_PROGBITS) {
            continue;
        }
        dut.add_memory_map({
            .addr = static_cast<uint32_t>(pseg->get_virtual_address()),
            .size = static_cast<uint32_t>(pseg->get_memory_size()),
            .data = reinterpret_cast<const uint8_t*>(pseg->get_data())
        });
    }

    int ticks = dut.simulate(clock_ticks);

    if (!dut.m_return_value) {
        std::println(stderr, "Simluation did not complete after {} clock ticks", clock_ticks);
        return 1;
    }

    if (*dut.m_return_value != 0) {
        std::println(stderr, "Simluation returned non-zero exit value {}", *dut.m_return_value);
        return 1;
    }

    if (verbose) {
        std::println(stderr, "Simluation completed after {} cycles with exit value {}", ticks, *dut.m_return_value);
    }


    return 0;
}
