
#include <filesystem>
#include <cassert>
#include <format>
#include <fstream>
#include <cstdlib>
#include <tuple>

#include <gtest/gtest.h>
#include <verilated.h>

std::tuple<uint8_t*,std::size_t> bin_from_asm(const std::string_view& s, const uint32_t addr, const std::string_view& tmp_name) {

    namespace fs = std::filesystem;

    auto tmp_dir = fs::temp_directory_path() / tmp_name;
    if (fs::exists(tmp_dir)) {
        fs::remove_all(tmp_dir);
    }
    bool ok = fs::create_directory(tmp_dir);
    assert(ok);

    std::ofstream asmf(tmp_dir / "test.s");
    asmf << ".global _start\n";
    asmf << ".text\n";
    asmf << std::format(".org {}\n", addr);
    asmf << s;
    asmf.close();
    const auto gas_args = "-march=rv32ezicsr -mno-relax -mno-csr-check -mlittle-endian";

    std::system(std::format("riscv32-unknown-elf-as {} {}/test.s -o {}/test.o",
                gas_args,
                tmp_dir.string(), tmp_dir.string()).c_str());
    std::system(std::format("riscv32-unknown-elf-objcopy -O binary {}/test.o {}/test.bin",
                tmp_dir.string(), tmp_dir.string()).c_str());

    std::ifstream binf(tmp_dir / "test.bin", std::ios::binary | std::ios::ate);
    std::streamsize binf_size = binf.tellg();
    binf.seekg(0, std::ios::beg);

    uint8_t* buf = new uint8_t[binf_size];
    binf.read(reinterpret_cast<char*>(buf), binf_size);

    binf.close();

    return std::make_pair(buf, binf_size);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


