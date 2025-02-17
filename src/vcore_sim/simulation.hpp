
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <Vcore.h>
#include <assert.h>

#include <memory>


class TurtleSimulation {

public:

    TurtleSimulation(int ram_size = 4096,
                     bool should_trace = false);

    ~TurtleSimulation();

    bool load_rom(const std::string& rom_file);

    void run_reset_cycles(int cycles = 5);

    bool run_cycles(int cycles);

    bool run_to_completion(int max_cycles = -1);

private:

    bool do_memory_update();

    bool get_byte(uint32_t addr, uint8_t& data_out);

    bool write_byte(uint32_t addr, uint8_t data);

    bool should_end(bool* success);

private:

    const bool m_should_trace;

    std::unique_ptr<Vcore> m_core;

    std::unique_ptr<VerilatedVcdC> m_trace_vcd;

    std::unique_ptr<uint8_t[]> m_rom_mem;
    int m_rom_size;

    std::unique_ptr<uint8_t[]> m_ram_mem;
    int m_ram_size;

    int m_dump_idx;

    bool m_have_expected_assert;
    uint32_t m_expected_assert;

    bool m_have_actual_assert;
    uint32_t m_actual_assert;
};
