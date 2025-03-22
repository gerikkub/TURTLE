#include "mod_test.hpp"
#include "Vpipeline.h"

#include <optional>
#include <cstdio>

class PipelineTest : public ClockedModTest<Vpipeline> {

    public:

    PipelineTest() : ClockedModTest<Vpipeline>() {
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

    void simulate(int clks) {

        mod->datafifo_full = 0;

        m_cycle_count = 0;
        while (clks < 0 || clks--) {
            if (mod->datafifo_addr_out == 0xABCDE000 &&
                mod->datafifo_val_out == 0x11223000 &&
                mod->datafifo_size_out == 2) {

                return;
            }

            handle_inst_bus();
            handle_data_bus();
            handle_csr_bus();
            handle_csr_update();

            clk();
            m_cycle_count++;
        }

        ASSERT_TRUE(false);
    }

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

    void handle_inst_bus(void) {
        if (mod->mem_fetch_addr_en) {
            int ok;
            uint32_t inst = 0;
            ok = read_memory(mod->mem_fetch_addr, inst);
            mod->mem_inst_in = inst;
            mod->mem_inst_valid = 1;
            mod->mem_inst_access_fault = ok != 0;
        }
    }

    void handle_data_bus(void) {
        if (mod->mem_data_addr_valid) {
            int ok;
            uint32_t val;
            switch (mod->mem_data_size) {
            case 0:
            {
                uint8_t data8;
                ok = read_memory(mod->mem_data_addr, data8);
                val = data8;
                break;
            }
            case 1:
            {
                uint16_t data16;
                ok = read_memory(mod->mem_data_addr, data16);
                val = data16;
                break;
            }
            case 2:
            {
                ok = read_memory(mod->mem_data_addr, val);
                break;
            }
            default:
                ASSERT_TRUE(false);
                break;
            }

            if (ok == 0) {
                mod->mem_data_in = val;
                mod->mem_data_access_fault = 0;
                mod->mem_data_valid = 1;
            } else {
                mod->mem_data_in = 0;
                mod->mem_data_access_fault = 1;
                mod->mem_data_valid = 1;
            }
        } else {
            mod->mem_data_in = 0;
            mod->mem_data_access_fault = 0;
            mod->mem_data_valid = 0;
        }

        if (mod->datafifo_valid_out) {

            int ok;
            switch (mod->datafifo_size_out) {
            case 0:
            {
                const uint8_t data8 = mod->datafifo_val_out & 0xFF;
                ok = write_memory(mod->datafifo_addr_out, data8);
                break;
            }
            case 1:
            {
                const uint16_t data16 = mod->datafifo_val_out & 0xFFFF;
                ok = write_memory(mod->datafifo_addr_out, data16);
                break;
            }
            case 2:
            {
                const uint32_t data32 = mod->datafifo_val_out;
                ok = write_memory(mod->datafifo_addr_out, data32);
                break;
            }
            default:
                ASSERT_TRUE(false);
                break;
            }

            if (ok != 0) {
                std::printf("Bad Data Write at addr: %8x (%d)\n",
                            mod->datafifo_addr_out,
                            m_cycle_count);
            }
            ASSERT_EQ(ok, 0);
        }
    }

    int m_csrbus_write_resp;
    int m_csrbus_read_addr;
    void handle_csr_bus(void) {

        // Clear all signals
        mod->csrbus_wready = 0;
        mod->csrbus_bresp = 0;
        mod->csrbus_bvalid = 0;
        mod->csrbus_rdata = 0;
        mod->csrbus_rresp = 3;

        // Prioritize Writes over reads to make forward progress
        if (mod->csrbus_wvalid ||
            mod->csrbus_bready) {
            ASSERT_EQ(mod->csrbus_wvalid +
                      mod->csrbus_bready, 1);
            if (mod->csrbus_wvalid) {
                auto ok = write_csr(mod->csrbus_waddr,
                                    mod->csrbus_wdata);
                m_csrbus_write_resp = ok ? 0 : 3;
                mod->csrbus_wready = 1;
            } else {
                mod->csrbus_wready = 0;
            }
            if (mod->csrbus_bready) {
                mod->csrbus_bresp = m_csrbus_write_resp;
                mod->csrbus_bvalid = 1;
            } else {
                mod->csrbus_bresp = 0;
                mod->csrbus_bvalid = 0;
            }

        } else if (mod->csrbus_arvalid) {
            auto read = read_csr(mod->csrbus_araddr);
            if (read) {
                mod->csrbus_rdata = *read;
                mod->csrbus_rresp = 0;
            } else {
                mod->csrbus_rdata = 0;
                mod->csrbus_rresp = 3;
            }
            mod->csrbus_rvalid = 1;
        } else {
            mod->csrbus_rvalid = 0;
            mod->csrbus_rdata = 0;
            mod->csrbus_rresp = 0;
        }
    }

    void handle_csr_update() {
        if (mod->exception_valid_out) {
            m_csr_rw_mem[MEPC] = mod->exception_mepc_out;
            m_csr_rw_mem[MCAUSE] = mod->exception_mcause_out;
            m_csr_rw_mem[MTVAL] = mod->exception_mtval_out;
        }

        mod->exception_mtvec_base_in = m_csr_rw_mem[MTVEC] >> 2;
        mod->exception_mepc_in = m_csr_rw_mem[MEPC];
    }

    std::optional<uint32_t> read_csr(const uint32_t addr) {
        return m_csr_ro_mem.contains(addr) ? std::optional(m_csr_ro_mem[addr]) :
               m_csr_rw_mem.contains(addr) ? std::optional(m_csr_rw_mem[addr]) :
                                             std::nullopt;
    }

    bool write_csr(const uint32_t addr, const uint32_t val) {
        if (m_csr_rw_mem.contains(addr)) {
            m_csr_rw_mem[addr] = val;
            return true;
        } else {
            return false;
        }
    }

    std::optional<MemoryMap> get_map(const uint32_t addr) {
        for (auto m : m_mappings) {
            if ((addr >= m.addr) &&
                (addr < (m.addr + m.size))) {
                return std::optional(m);
            }
        }
        return std::nullopt;
    }

    template <typename T>
    int read_memory(const uint32_t addr, T& res) {
        auto mapping = get_map(addr);
        if (!mapping) {
            return -1;
        }

        const uint32_t offset = addr - mapping.value().addr;
        res = *(T*)(mapping.value().data + offset);
        return 0;
    }

    template <typename T>
    int write_memory(const uint32_t addr, const T& w) {
        auto mapping = get_map(addr);
        if (!mapping) {
            return -1;
        }

        const uint32_t offset = addr - mapping.value().addr;
        *(T*)(mapping.value().data + offset) = w;
        return 0;
    }

    int m_cycle_count;

    std::map<uint32_t, uint32_t> m_csr_rw_mem;
    std::map<uint32_t, uint32_t> m_csr_ro_mem;

};

TEST_F(PipelineTest, Reset) {
    reset();
    clk();

    ASSERT_EQ(mod->mem_fetch_addr, 0);
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->datafifo_valid_out, 0);
}

TEST_F(PipelineTest, Count) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        lui x1, 0 \n\
        addi x2, x0, 100 \n\
        addi x3, x0, 1 \n\
        loop: \n\
        beq x1, x2, done \n\
        add x1, x1, x3 \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(1000);
}

TEST_F(PipelineTest, Call) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        lui x1, 0 \n\
        addi x2, x0, 20 \n\
        addi x3, x0, 1 \n\
        jal x0, loop \n\
        fn: \n\
        add x1, x1, x3 \n\
        jalr x0,x10 \n\
        loop: \n\
        beq x1, x2, done \n\
        jal x10, fn \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(1000);
}

TEST_F(PipelineTest, Shift) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        lui x1, 0xFA987 \n\
        sra x1, x1, 20 \n\
        addi x2, x0, -87 \n\
        beq x1, x2, done \n\
        loop: \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, LoadStoreHazard) {
    reset();
    memory_map_from_new(0x1000, 128);

    memory_map_from_asm(" \
        _start: \n\
        lui x1, 0xFA987 \n\
        addi x1, x1, 0x654 \n\
        addi x4, x0, 0xFFFFFF98 \n\
        lui x2, 1 \n\
        sw x1, 0(x2) \n\
        lb x3, 2(x2) \n\
        beq x4, x3, done \n\
        loop: \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, Fence) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        fence \n\
        fence \n\
        fence.tso \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, HazardRaW) {
    reset();

    // Read after write hazards for "addi", "add" and "beq" instructions
    memory_map_from_asm(" \
        _start: \n\
        addi x2, x0, 4 \n\
        addi x1, x0, 1 \n\
        addi x3, x1, 1 \n\
        add x3, x3, x1 \n\
        add x3, x1, x3 \n\
        beq x3, x2, done \n\
        loop: \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, HazardWaW) {
    reset();

    // Write after Write for "add" instruction
    memory_map_from_asm(" \
        _start: \n\
        addi x2, x0, 8 \n\
        addi x1, x0, 1 \n\
        add x1, x1, x1 \n\
        add x1, x1, x1 \n\
        add x1, x1, x1 \n\
        beq x2, x1, done \n\
        loop: \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, CsrBus) {
    reset();

    // Write after Write for "add" instruction
    memory_map_from_asm(" \
        _start: \n\
        lui x1, 0xAABBC \n\
        addi x1, x1, 0x123 \n\
        csrrw x0, mscratch, x1 \n\
        csrrw x2, mscratch, x0 \n\
        bne x1, x2, loop \n\
\
        csrrsi x3, mscratch, 0x16 \n\
        bne x1, x3, loop \n\
\
        ori x1, x1, 0x16 \n\
        csrrw x2, mscratch, x0 \n\
        beq x2, x1, done \n\
\
        loop: \n\
        jal x0, loop \n\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4)\n");

    simulate(100);
}

TEST_F(PipelineTest, ExceptionECall) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        add x0, x0, x0 \n\
        ecall \n\
\
        loop: \n\
        jal x0, loop \n\
\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4) \n\
\
        .org 0x400 \n\
        exp: \n\
        csrrw x1, mepc, x0 \n\
        add x2, x0, 0x4 \n\
        bne x1, x2, loop \n\
        csrrw x1, mcause, x0 \n\
        add x2, x0, 11 \n\
        bne x1, x2, loop \n\
        jal x0, done \n\
        ");

    simulate(100);
}

TEST_F(PipelineTest, ExceptionReturn) {
    reset();

    memory_map_from_asm(" \
        _start: \n\
        add x1, x0, x0 \n\
        ecall \n\
        addi x2, x0, 1 \n\
        beq x1, x2, done \n\
\
        loop: \n\
        jal x0, loop \n\
\
        done: \n\
        lui x4, 0xABCDE \n\
        lui x5, 0x11223 \n\
        sw x5,0(x4) \n\
\
        .org 0x400 \n\
        exp: \n\
        addi x1, x1, 1\n\
        csrrw x3, mepc, x0\n\
        addi x3, x3, 4\n\
        csrrw x0, mepc, x3\n\
        mret \n\
        ");

    simulate(100);

}

