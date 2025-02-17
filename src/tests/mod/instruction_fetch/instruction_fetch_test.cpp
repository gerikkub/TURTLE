
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vinstruction_fetch.h"

#include <gtest/gtest.h>

class InstructionFetchTest : public ::testing::Test {

    protected:

    virtual void SetUp() override {
        vctx = std::make_unique<VerilatedContext>();
        vcd = std::make_unique<VerilatedVcdC>();
        mod = std::make_unique<Vinstruction_fetch>(vctx.get());
        timeui = 0;

        vctx->traceEverOn(true);
        mod->trace(vcd.get(), 99);
        auto ut = ::testing::UnitTest::GetInstance();
        auto test = ut->current_test_info();
        std::stringstream trace_name;
        trace_name << "test_" <<
                      test->test_suite_name() << "_" <<
                      test->name() << ".vcd";
            
        vcd->open(trace_name.str().c_str());
    }

    virtual void TearDown() override {

        vcd->flush();
        vcd->close();

        mod->final();

        mod.reset();
        vcd.reset();
        vctx.reset();
    }

    void eval() {
        mod->eval();
        vcd->dump(timeui);
        timeui++;
    }

    void clk() {
        timeui += 500;
        mod->clk = 0;
        eval();

        timeui += 500;
        mod->clk = 1;
        eval();
    }

    void reset() {
        mod->reset = 1;
        clk();
        clk();
        clk();
        mod->reset = 0;
    }
    
    uint64_t timeui;
    std::unique_ptr<VerilatedContext> vctx;
    std::unique_ptr<VerilatedVcdC> vcd;
    std::unique_ptr<Vinstruction_fetch> mod;
};

TEST_F(InstructionFetchTest, Reset) {

    reset();

    ASSERT_EQ(mod->mem_fetch_addr_en, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    ASSERT_EQ(mod->inst, 0xc0defec4);
}

TEST_F(InstructionFetchTest, Fetch) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1234AABB);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 8);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x11223344;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x11223344);
}

TEST_F(InstructionFetchTest, Stall) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    mod->stall = 1;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0xc0defec4);
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 0);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0xc0defec4);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 0);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0xc0defec4);

    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1234AABB);

    clk();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 8);
}

TEST_F(InstructionFetchTest, Override) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    mod->override_pc = 1;
    mod->override_pc_addr = 0x20;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0xc0defec4);
    ASSERT_EQ(mod->mem_fetch_addr_en, 0);

    clk();
    mod->override_pc = 0;
    eval();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0x20);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0xABCD1234;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0xABCD1234);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);
    //Verilated::traceEverOn(true);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
