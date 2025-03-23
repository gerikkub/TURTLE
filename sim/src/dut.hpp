
#pragma once

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vpipeline.h"

#include <optional>
#include <format>
#include <cstdlib>

class Dut {

    public:

    Dut(uint32_t reset_address,
        uint32_t semihost_address);

    ~Dut();

    void enableTrace(std::string trace_name);

    int simulate(int clks);

    struct MemoryMap {
        uint32_t addr;
        uint32_t size;
        const uint8_t* data;
    };
    void set_memory_map(const std::vector<MemoryMap>& mappings);
    void add_memory_map(const MemoryMap& map);

    private:
    enum CsrRORegister : uint32_t {
        MISA = 0x301,
        MVENDORID = 0xF11,
        MARCHID = 0xF12,
        MIMPID = 0xF13,
        MHARDIT = 0xF14,
        MCONFIGPTR = 0xF15,
    };

    enum CsrRWRegister : uint32_t {
        MSTATUS = 0x300,
        MEDELEG = 0x302,
        MIDELEG = 0x303,
        MIE = 0x304,
        MTVEC = 0x305,
        MCOUNTEREN = 0x306,
        MSTATUSH = 0x310,
        MEDELEGH = 0x312,

        MSCRATCH = 0x340,
        MEPC = 0x341,
        MCAUSE = 0x342,
        MTVAL = 0x343,
        MIP = 0x344,
        MTINST = 0x34A,
        MTVAL2 = 0x34B,
    };

    void eval();
    void clk();
    void reset();

    void handle_inst_bus(void);
    void handle_data_bus(void);
    void handle_csr_bus(void);
    void handle_csr_update(void);
    void handle_interrupt();

    std::optional<uint32_t> read_csr(const uint32_t addr);
    bool write_csr(const uint32_t addr, const uint32_t val);
    std::optional<MemoryMap> get_map(const uint32_t addr);

    template <typename T>
    int read_memory(const uint32_t addr, T& res);
    template <typename T>
    int write_memory(const uint32_t addr, const T& w);

    // Verilator model contexts
    bool m_trace_enabled;
    uint64_t m_timeui;
    std::unique_ptr<VerilatedContext> m_vctx;
    std::unique_ptr<VerilatedVcdC> m_vcd;
    std::unique_ptr<Vpipeline> m_mod;

    // Memory space mappings
    std::vector<MemoryMap> m_mappings;

    // CSR space registers
    std::map<uint32_t, uint32_t> m_csr_rw_mem;
    std::map<uint32_t, uint32_t> m_csr_ro_mem;

    // Simulated core states
    uint32_t m_reset_address;
    uint32_t m_semihost_address;
    int m_cycle_count;
    int m_csrbus_write_resp;
    int m_csrbus_read_addr;
    std::optional<uint32_t> m_trigger_interrupt;

    public:
    std::optional<int32_t> m_return_value;
};

template <typename T>
int Dut::read_memory(const uint32_t addr, T& res) {
    auto mapping = get_map(addr);
    if (!mapping) {
        return -1;
    }

    const uint32_t offset = addr - mapping.value().addr;
    res = *(T*)(mapping.value().data + offset);
    return 0;
}

template <typename T>
int Dut::write_memory(const uint32_t addr, const T& w) {
    auto mapping = get_map(addr);
    if (!mapping) {
        return -1;
    }

    const uint32_t offset = addr - mapping.value().addr;
    *(T*)(mapping.value().data + offset) = w;
    return 0;
}



