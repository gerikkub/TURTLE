
#include <verilated.h>
#include <verilated_vcd_c.h>

#include <gtest/gtest.h>

template <typename T>
class ModTest : public ::testing::Test {

    public:

    virtual void SetUp() override {
        vctx = std::make_unique<VerilatedContext>();
        vcd = std::make_unique<VerilatedVcdC>();
        mod = std::make_unique<T>(vctx.get());
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
    
    uint64_t timeui;
    std::unique_ptr<VerilatedContext> vctx;
    std::unique_ptr<VerilatedVcdC> vcd;
    std::unique_ptr<T> mod;
};

template <typename T>
class ClockedModTest : public ModTest<T> {

    public:
    void clk() {
        this->timeui += 500;
        this->mod->clk = 0;
        this->eval();

        this->timeui += 500;
        this->mod->clk = 1;
        this->eval();
    }

    void reset() {
        this->mod->reset = 1;

        clk();
        clk();
        clk();

        this->mod->reset = 0;
    }

};

