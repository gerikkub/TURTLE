
#include "dut.hpp"

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Vpipeline.h"
#include "Vpipeline___024root.h"

#include <format>
#include <cstdlib>
#include <optional>

Dut::Dut(uint32_t reset_address,
         uint32_t semihost_address) :
    m_trace_enabled(false),
    m_timeui(0),
    m_vctx(std::make_unique<VerilatedContext>()),
    m_vcd(std::make_unique<VerilatedVcdC>()),
    m_mod(std::make_unique<Vpipeline>(m_vctx.get())),
    m_mappings(),
    m_csr_rw_mem(),
    m_csr_ro_mem(),
    m_cycle_count(0),
    m_reset_address(reset_address),
    m_semihost_address(semihost_address),
    m_csrbus_write_resp(0),
    m_csrbus_read_addr(0),
    m_trigger_interrupt(std::nullopt) {

    m_csr_ro_mem[MVENDORID] = 0xABCD8899;
    m_csr_ro_mem[MARCHID] = 0x09876543;
    m_csr_ro_mem[MIMPID] = 1;
    m_csr_ro_mem[MHARDIT] = 0;
    m_csr_ro_mem[MCONFIGPTR] = 0;
    m_csr_ro_mem[MISA] = (1 << 30) | // 32-bit
                         (1 << 4); // RV32E

    m_csr_rw_mem[MSTATUS] = 0;
    m_csr_rw_mem[MEDELEG] = 0;
    m_csr_rw_mem[MIDELEG] = 0;
    m_csr_rw_mem[MIE] = 0;
    // Exception vector at 0x400 by default
    m_csr_rw_mem[MTVEC] = 0x400;
    m_csr_rw_mem[MCOUNTEREN] = 0;
    m_csr_rw_mem[MSTATUSH] = 0;
    m_csr_rw_mem[MEDELEGH] = 0;

    m_csr_rw_mem[MSCRATCH] = 0;
    m_csr_rw_mem[MEPC] = 0;
    m_csr_rw_mem[MCAUSE] = 0;
    m_csr_rw_mem[MTVAL] = 0;
    m_csr_rw_mem[MIP] = 0;
    m_csr_rw_mem[MTINST] = 0;
    m_csr_rw_mem[MTVAL2] = 0;
}

Dut::~Dut() {
    if (m_trace_enabled) {
        m_vcd->flush();
        m_vcd->close();
    }

    m_mod->final();
}

void Dut::enableTrace(std::string trace_name) {

    m_vctx->traceEverOn(true);
    m_mod->trace(m_vcd.get(), 99);
    m_vcd->open(trace_name.c_str());

    m_trace_enabled = true;
}

int Dut::simulate(int clks) {
    reset();

    m_mod->datafifo_full = 0;

    m_cycle_count = 0;
    while (clks < 0 || clks--) {
        if (m_mod->datafifo_addr_out == m_semihost_address &&
            m_mod->datafifo_size_out == 2) {

            m_return_value = m_mod->datafifo_val_out;

            return m_cycle_count;
        }

        handle_inst_bus();
        handle_data_bus();
        handle_csr_bus();
        handle_csr_update();
        handle_interrupt();

        clk();
        m_cycle_count++;
    }

    return 0;
}

void Dut::handle_inst_bus() {
    if (m_mod->mem_fetch_addr_en) {
        int ok;
        uint32_t inst = 0;
        ok = read_memory(m_mod->mem_fetch_addr, inst);
        m_mod->mem_inst_in = inst;
        m_mod->mem_inst_valid = 1;
        m_mod->mem_inst_access_fault = ok != 0;
    }
}

void Dut::handle_data_bus() {
    if (m_mod->mem_data_addr_valid) {
        int ok;
        uint32_t val;
        switch (m_mod->mem_data_size) {
        case 0:
        {
            uint8_t data8;
            ok = read_memory(m_mod->mem_data_addr, data8);
            val = data8;
            break;
        }
        case 1:
        {
            uint16_t data16;
            ok = read_memory(m_mod->mem_data_addr, data16);
            val = data16;
            break;
        }
        case 2:
        {
            ok = read_memory(m_mod->mem_data_addr, val);
            break;
        }
        default:
            assert(false);
            break;
        }

        if (ok == 0) {
            m_mod->mem_data_in = val;
            m_mod->mem_data_access_fault = 0;
            m_mod->mem_data_valid = 1;
        } else {
            m_mod->mem_data_in = 0;
            m_mod->mem_data_access_fault = 1;
            m_mod->mem_data_valid = 1;
        }
    } else {
        m_mod->mem_data_in = 0;
        m_mod->mem_data_access_fault = 0;
        m_mod->mem_data_valid = 0;
    }

    if (m_mod->datafifo_valid_out) {

        int ok;
        switch (m_mod->datafifo_size_out) {
        case 0:
        {
            const uint8_t data8 = m_mod->datafifo_val_out & 0xFF;
            ok = write_memory(m_mod->datafifo_addr_out, data8);
            break;
        }
        case 1:
        {
            const uint16_t data16 = m_mod->datafifo_val_out & 0xFFFF;
            ok = write_memory(m_mod->datafifo_addr_out, data16);
            break;
        }
        case 2:
        {
            const uint32_t data32 = m_mod->datafifo_val_out;
            ok = write_memory(m_mod->datafifo_addr_out, data32);
            break;
        }
        default:
            assert(false);
            break;
        }

        if (ok != 0) {
            std::printf("Bad Data Write at addr: %8x (%d)\n",
                        m_mod->datafifo_addr_out,
                        m_cycle_count);
        }
    }
}

void Dut::handle_csr_bus() {

    // Clear all signals
    m_mod->csrbus_wready = 0;
    m_mod->csrbus_bresp = 0;
    m_mod->csrbus_bvalid = 0;
    m_mod->csrbus_rdata = 0;
    m_mod->csrbus_rresp = 3;

    // Prioritize Writes over reads to make forward progress
    if (m_mod->csrbus_wvalid ||
        m_mod->csrbus_bready) {
        assert(m_mod->csrbus_wvalid +
               m_mod->csrbus_bready == 1);
        if (m_mod->csrbus_wvalid) {
            auto ok = write_csr(m_mod->csrbus_waddr,
                                m_mod->csrbus_wdata);
            m_csrbus_write_resp = ok ? 0 : 3;
            m_mod->csrbus_wready = 1;
        } else {
            m_mod->csrbus_wready = 0;
        }
        if (m_mod->csrbus_bready) {
            m_mod->csrbus_bresp = m_csrbus_write_resp;
            m_mod->csrbus_bvalid = 1;
        } else {
            m_mod->csrbus_bresp = 0;
            m_mod->csrbus_bvalid = 0;
        }

    } else if (m_mod->csrbus_arvalid) {
        auto read = read_csr(m_mod->csrbus_araddr);
        if (read) {
            m_mod->csrbus_rdata = *read;
            m_mod->csrbus_rresp = 0;
        } else {
            m_mod->csrbus_rdata = 0;
            m_mod->csrbus_rresp = 3;
        }
        m_mod->csrbus_rvalid = 1;
    } else {
        m_mod->csrbus_rvalid = 0;
        m_mod->csrbus_rdata = 0;
        m_mod->csrbus_rresp = 0;
    }
}

void Dut::handle_csr_update() {
    if (m_mod->exception_valid_out) {
        m_csr_rw_mem[MEPC] = m_mod->exception_mepc_out;
        m_csr_rw_mem[MCAUSE] = m_mod->exception_mcause_out;
        m_csr_rw_mem[MTVAL] = m_mod->exception_mtval_out;
    }

    m_mod->exception_mtvec_base_in = m_csr_rw_mem[MTVEC] >> 2;
    m_mod->exception_mepc_in = m_csr_rw_mem[MEPC];
}

void Dut::handle_interrupt() {
    if (m_mod->interrupt_valid_out) {
        m_trigger_interrupt = std::nullopt;
    }

    if ((m_csr_rw_mem[MSCRATCH] & 0xFFAA0000) == 0xFFAA0000) {
        // Pseudo CPU interrupt
        m_trigger_interrupt = std::optional(m_csr_rw_mem[MSCRATCH] & 0xFFFF);
        m_csr_rw_mem[MSCRATCH] = 0;
    }

    if (m_trigger_interrupt) {
        m_mod->interrupt_valid = 1;
        m_mod->interrupt_num = *m_trigger_interrupt;
    } else {
        m_mod->interrupt_valid = 0;
    }

}

std::optional<uint32_t> Dut::read_csr(const uint32_t addr) {
    return m_csr_ro_mem.contains(addr) ? std::optional(m_csr_ro_mem[addr]) :
           m_csr_rw_mem.contains(addr) ? std::optional(m_csr_rw_mem[addr]) :
                                         std::nullopt;
}

bool Dut::write_csr(const uint32_t addr, const uint32_t val) {
    if (m_csr_rw_mem.contains(addr)) {
        m_csr_rw_mem[addr] = val;
        return true;
    } else {
        return false;
    }
}

std::optional<Dut::MemoryMap> Dut::get_map(const uint32_t addr) {
    for (auto m : m_mappings) {
        if ((addr >= m.addr) &&
            (addr < (m.addr + m.size))) {
            return std::optional(m);
        }
    }
    return std::nullopt;
}

void Dut::set_memory_map(const std::vector<Dut::MemoryMap>& mappings) {
    for (auto m : mappings) {
        m_mappings.push_back(m);
    }
}

void Dut::add_memory_map(const Dut::MemoryMap& map) {
    m_mappings.push_back(map);
}

void Dut::eval() {
    m_mod->eval();
    if (m_trace_enabled) m_vcd->dump(m_timeui);
    m_timeui++;
}

void Dut::clk() {
    m_timeui += 500;
    m_mod->clk = 0;
    eval();

    m_timeui += 500;
    m_mod->clk = 1;
    eval();
}

void Dut::reset() {
    m_mod->reset = 1;
    m_mod->reset_address = m_reset_address;

    clk();
    clk();
    clk();

    m_mod->reset = 0;
}

