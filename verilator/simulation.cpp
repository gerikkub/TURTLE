
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <Vcore.h>
#include <assert.h>
#include <memory>
#include <iostream>

#include "simulation.hpp"


TurtleSimulation::TurtleSimulation(int ram_size,
                                   bool should_trace) :
    m_should_trace(should_trace),
    m_rom_mem() {

    this->m_core = std::make_unique<Vcore>();

    if (should_trace) {
        this->m_trace_vcd = std::make_unique<VerilatedVcdC>();
        this->m_core->trace(this->m_trace_vcd.get(), 99);
        this->m_trace_vcd->open("core.vcd");
    }

    this->m_ram_mem = std::unique_ptr<uint8_t[]>(new uint8_t[ram_size]);
    if (this->m_ram_mem != nullptr) {
        this->m_ram_size = ram_size;
    } else {
        this->m_ram_size = 0;
    }

    this->m_core->clk = 0;
    this->m_core->reset = 1;
    this->m_core->mem_csr_read_en = 0;
    this->m_core->mem_csr_write_en = 0;
    this->m_core->ext_interrupt = 0;
    this->m_core->mem_din = 0;
}

TurtleSimulation::~TurtleSimulation() {
    if (this->m_should_trace) {
        this->m_trace_vcd->close();
    }

    this->m_core->final();
}

bool TurtleSimulation::load_rom(const std::string& rom_filename) {

    FILE* rom_file = fopen(rom_filename.c_str(), "rb");

    if (rom_file == NULL) {
        perror("fopen: ");
        return false;
    }

    fseek(rom_file, 0, SEEK_END);
    this->m_rom_size = ftell(rom_file);
    fseek(rom_file, 0, SEEK_SET);

    this->m_rom_mem = std::unique_ptr<uint8_t[]>(new uint8_t[this->m_rom_size]);

    fread(this->m_rom_mem.get(), this->m_rom_size, 1, rom_file);

    return true;
}

void TurtleSimulation::run_reset_cycles(int cycles) {

    this->m_core->reset = 0;

    for (int i = 0; i < cycles; i++) {
        this->m_core->clk = !this->m_core->clk;

        if (this->m_should_trace) {
            m_trace_vcd->dump(i);
        }

        this->m_core->eval();
    }
}

bool TurtleSimulation::run_cycles(int cycles) {

    int res;

    this->m_core->reset = 1;

    for (int i = 0; i < cycles; i++) {

        this->m_core->eval();

        if (this->m_should_trace) {
            m_trace_vcd->dump(i);
        }

        if (this->m_core->clk == 1) {
            res = do_memory_update();
            if (!res) {
                return false;
            }
        }

        this->m_core->clk = !this->m_core->clk;
    }

    return true;
}

bool TurtleSimulation::run_to_completion(int max_cycles) {

    int res;

    this->m_core->reset = 1;

    int i;

    for (i = 0; i < max_cycles; i++) {

        this->m_core->eval();

        if (this->m_should_trace) {
            m_trace_vcd->dump(i);
        }

        if (this->m_core->clk == 1) {
            res = do_memory_update();
            if (!res) {
                return false;
            }

            if (this->m_core->mem_write_en == 1 &&
                this->m_core->mem_addr == 0xFFFFFFFF &&
                (this->m_core->mem_dout & 0xFF) == 0xA5) {
                // Magic write

                break;
            }
        }

        this->m_core->clk = !this->m_core->clk;
    }

    if (i == max_cycles) {
        return false;
    }

    return true;
}

bool TurtleSimulation::do_memory_update() {

    bool res = true;

    if (this->m_core->mem_read_en == 1) {

        uint8_t data_out;

        if (!(this->m_core->mem_width <= 2 && this->m_core->mem_width >= 0)) {
            return false;
        }

        if (this->m_core->mem_width == 0) {
            res &= get_byte(this->m_core->mem_addr, data_out);
            this->m_core->mem_din = data_out;
        } else if (this->m_core->mem_width == 1) {

            res &= get_byte(this->m_core->mem_addr, data_out);
            this->m_core->mem_din = data_out;

            res &= get_byte(this->m_core->mem_addr + 1, data_out);
            this->m_core->mem_din |= data_out << 8;
        } else {
            res &= get_byte(this->m_core->mem_addr, data_out);
            this->m_core->mem_din = data_out;
            
            res &= get_byte(this->m_core->mem_addr + 1, data_out);
            this->m_core->mem_din |= data_out << 8;
            
            res &= get_byte(this->m_core->mem_addr + 2, data_out);
            this->m_core->mem_din |= data_out << 16;
            
            res &= get_byte(this->m_core->mem_addr + 3, data_out);
            this->m_core->mem_din |= data_out << 24;
        }

        if (!res) {
            return false;
        }
    }

    if (this->m_core->mem_write_en == 1) {
        //std::cout << "Writing " << std::hex << this->m_core->mem_dout;
        //std::cout << " to " << std::hex << this->m_core->mem_addr << std::endl;
        if (!(this->m_core->mem_width <= 2 && this->m_core->mem_width >= 0)) {
            return false;
        }

        if (this->m_core->mem_width == 0) {
            res &= write_byte(this->m_core->mem_addr, this->m_core->mem_dout);
        } else if (this->m_core->mem_width == 1) {
            res &= write_byte(this->m_core->mem_addr, this->m_core->mem_dout & 0xFF);
            res &= write_byte(this->m_core->mem_addr + 1, (this->m_core->mem_dout >> 8) & 0xFF);
        } else {
            res &= write_byte(this->m_core->mem_addr, this->m_core->mem_dout & 0xFF);
            res &= write_byte(this->m_core->mem_addr + 1, (this->m_core->mem_dout >> 8) & 0xFF);
            res &= write_byte(this->m_core->mem_addr + 2, (this->m_core->mem_dout >> 16) & 0xFF);
            res &= write_byte(this->m_core->mem_addr + 3, (this->m_core->mem_dout >> 24) & 0xFF);
        }
    }

    return true;
}


bool TurtleSimulation::get_byte(uint32_t addr, uint8_t& data_out) {

    if (addr < 0x80000000) {

        if (addr < this->m_rom_size) {
            data_out = this->m_rom_mem[addr];
            return true;
        }

    } else {
        uint32_t ram_offset = addr - 0x80000000;

        if (ram_offset < this->m_ram_size) {
            data_out = this->m_ram_mem[ram_offset];
            return true;
        }
    }

    return false;
}

bool TurtleSimulation::write_byte(uint32_t addr, uint8_t data) {

    if (addr >= 0x80000000) {

        uint32_t ram_offset = addr - 0x80000000;

        if (ram_offset < this->m_ram_size) {
            this->m_ram_mem[ram_offset] = data;
            return true;
        } 
    }

    return false;
}
