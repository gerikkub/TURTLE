
#include <verilated.h>
#include <verilated_vcd_c.h>

#include <format>
#include <cstdlib>

#include <gtest/gtest.h>

std::tuple<uint8_t*,std::size_t> bin_from_asm(const std::string_view& s, const uint32_t addr, const std::string_view& tmp_name);

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

    struct MemoryMap {
        uint32_t addr;
        uint32_t size;
        uint8_t* data;
    };

    void set_memory_map(const std::vector<MemoryMap>& mappings) {
        for (auto m : mappings) {
            m_mappings.push_back(m);
        }
    }

    void set_memory_map(const MemoryMap& map) {
        auto mappings = std::vector<MemoryMap>();
        mappings.push_back(map);
        set_memory_map(mappings);
    }

    void memory_map_from_asm(const std::string_view& s, uint32_t addr = 0) {
        auto ut = ::testing::UnitTest::GetInstance();
        auto test = ut->current_test_info();
        auto tmp_name = std::format(
                "asmbuild_{}_{}_{:x}",
                test->test_suite_name(),
                test->name(),
                addr);
        auto data = bin_from_asm(s, addr, tmp_name);
        MemoryMap map = {
            .addr = addr,
            .size = static_cast<uint32_t>(std::get<1>(data)),
            .data = std::get<0>(data)
        };
        set_memory_map(map);
    }

    void memory_map_from_new(uint32_t addr, uint32_t size) {
        uint8_t* data = new uint8_t[size];
        std::memset(data, 0, size);
        MemoryMap map = {
            .addr = addr,
            .size = size,
            .data = data
        };
        set_memory_map(map);
    }

    
    uint64_t timeui;
    std::unique_ptr<VerilatedContext> vctx;
    std::unique_ptr<VerilatedVcdC> vcd;
    std::unique_ptr<T> mod;

    std::vector<MemoryMap> m_mappings;
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




