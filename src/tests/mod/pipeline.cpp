#include "mod_test.hpp"
#include "Vpipeline.h"

#include <optional>
#include <cstdio>

class PipelineTest : public ClockedModTest<Vpipeline> {

    public:

    void simulate(int clks) {

        mod->datafifo_full = 0;

        int cycle_count = 0;
        while (clks < 0 || clks--) {
            if (mod->mem_fetch_addr_en) {
                int ok;
                uint32_t inst = 0;
                ok = read_memory(mod->mem_fetch_addr, inst);
                mod->mem_inst_in = inst;
                mod->mem_inst_valid = 1;
                mod->mem_inst_access_fault = ok != 0;
            }

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

                if (mod->datafifo_addr_out == 0xABCDE000 &&
                    mod->datafifo_val_out == 0x11223000 &&
                    mod->datafifo_size_out == 2) {

                    return;
                }

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
                                cycle_count);
                }
                ASSERT_EQ(ok, 0);
            }
            clk();
            cycle_count++;
        }

        ASSERT_TRUE(false);
    }

    private:

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

