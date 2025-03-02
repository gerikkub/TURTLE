#include "mod_test.hpp"
#include "Vpipeline.h"

#include <optional>
#include <cstdio>

class PipelineTest : public ClockedModTest<Vpipeline> {

    public:
    struct MemoryMap {
        uint32_t addr;
        uint32_t size;
        uint8_t* data;
    };

    void set_memory_map(const std::vector<MemoryMap>& mappings) {
        m_mappings = mappings;
    }

    void set_memory_map(const MemoryMap& map) {
        auto mappings = std::vector<MemoryMap>();
        mappings.push_back(map);
        set_memory_map(mappings);
    }

    void simulate(int clks) {

        mod->datafifo_full = 0;

        int cycle_count = 0;
        while (clks < 0 || clks--) {
            if (mod->mem_fetch_addr_en) {
                int ok;
                uint32_t inst = 0;
                ok = read_memory(mod->mem_fetch_addr, inst);
                if (ok != 0) {
                    std::printf("Instruction Fetch Fault at addr: %8x (%d)\n",
                                mod->mem_fetch_addr,
                                cycle_count);
                }
                mod->mem_inst_in = inst;
                mod->mem_inst_valid = 1;
                mod->mem_inst_access_fault = ok != 0;
            }

            if (mod->datafifo_valid_out) {

                if (mod->datafifo_addr_out == 0xABCDE000 &&
                    mod->datafifo_val_out == 0x11223000 &&
                    mod->datafifo_size_out == 2) {

                    std::printf("Complete Nominal (%d)\n",
                                cycle_count);
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

    private:
    std::vector<MemoryMap> m_mappings;

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

    uint32_t inst_mem[] = {
//00000000 <_start>:
        0x000000b7, // lui	ra,0x0
        0x06400113, // li	sp,100
        0x00100193, // li	gp,1

//0000000c <loop>:
        0x00208663,  // beq	ra,sp,18 <done>
        0x003080b3,  // add	ra,ra,gp
        0xff9ff06f,  // j	c <loop>

//00000018 <done>:
        0xabcde237,  // lui	tp,0xabcde
        0x112232b7,  // lui	t0,0x11223
        0x00522023   // sw	t0,0(tp) # abcde000 <done+0xabcddfe8>
    };

    MemoryMap inst_mapping{
        .addr = 0, 
        .size = sizeof(inst_mem),
        .data = reinterpret_cast<uint8_t*>(inst_mem)
    };
    set_memory_map(inst_mapping);

    simulate(10000);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
