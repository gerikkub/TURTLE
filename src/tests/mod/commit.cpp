#include "mod_test.hpp"
#include "Vcommit.h"

typedef ClockedModTest<Vcommit> CommitTest;

TEST_F(CommitTest, Reset) {
    reset();

    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);
}

TEST_F(CommitTest, RegisterWrite) {
    reset();

    mod->execute_valid = 1;
    mod->execute_rd = 1;
    mod->execute_rd_val = 0xABCD2233;
    mod->execute_jump_valid = 0;
    mod->execute_exception_valid = 0;
    mod->execute_store_valid = 0;
    clk();
    mod->execute_valid = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_out, 1);
    ASSERT_EQ(mod->rd_val_out, 0xABCD2233);
    ASSERT_EQ(mod->rd_valid_out, 1);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);

    mod->execute_valid = 1;
    mod->execute_rd = 0;
    mod->execute_rd_val = 0xABCD2233;
    mod->execute_jump_valid = 0;
    mod->execute_exception_valid = 0;
    mod->execute_store_valid = 0;
    clk();
    mod->execute_valid = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_out, 0);
    ASSERT_EQ(mod->rd_val_out, 0xABCD2233);
    ASSERT_EQ(mod->rd_valid_out, 0);
}

TEST_F(CommitTest, Store) {
    reset();

    mod->execute_valid = 1;
    mod->execute_rd = 0;
    mod->execute_rd_val = 0;
    mod->execute_jump_valid = 0;
    mod->execute_exception_valid = 0;
    mod->execute_store_valid = 1;
    mod->execute_store_addr = 0xAABB2244;
    mod->execute_store_val = 0x12345678;
    mod->execute_store_size = 2;
    mod->datafifo_full = 0;
    clk();
    mod->execute_valid = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_valid_out, 0);
    ASSERT_EQ(mod->datafifo_valid_out, 1);
    ASSERT_EQ(mod->datafifo_addr_out, 0xAABB2244);
    ASSERT_EQ(mod->datafifo_val_out, 0x12345678);
    ASSERT_EQ(mod->datafifo_size_out, 2);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);

    // Store with delay for fifo
    mod->execute_valid = 1;
    mod->execute_rd = 0;
    mod->execute_rd_val = 0;
    mod->execute_jump_valid = 0;
    mod->execute_exception_valid = 0;
    mod->execute_store_valid = 1;
    mod->execute_store_addr = 0xAABB2248;
    mod->execute_store_val = 0xABCDEF01;
    mod->execute_store_size = 2;
    mod->datafifo_full = 1;
    clk();
    mod->execute_valid = 0;
    eval();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 1);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_valid_out, 0);
    ASSERT_EQ(mod->datafifo_valid_out, 0);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 1);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_valid_out, 0);
    ASSERT_EQ(mod->datafifo_valid_out, 0);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 1);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_valid_out, 0);
    ASSERT_EQ(mod->datafifo_valid_out, 0);

    mod->datafifo_full = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 0);
    ASSERT_EQ(mod->rd_valid_out, 0);
    ASSERT_EQ(mod->datafifo_valid_out, 1);
    ASSERT_EQ(mod->datafifo_addr_out, 0xAABB2248);
    ASSERT_EQ(mod->datafifo_val_out, 0xABCDEF01);
    ASSERT_EQ(mod->datafifo_size_out, 2);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);
}

TEST_F(CommitTest, Jump) {
    reset();

    mod->execute_valid = 1;
    mod->execute_rd = 1;
    mod->execute_rd_val = 0xABCD2233;
    mod->execute_jump_valid = 1;
    mod->execute_jump_pc = 0x80004004;
    mod->execute_exception_valid = 0;
    mod->execute_store_valid = 0;
    clk();
    mod->execute_valid = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 1);
    ASSERT_EQ(mod->pipeline_pc, 0x80004004);
    ASSERT_EQ(mod->rd_out, 1);
    ASSERT_EQ(mod->rd_val_out, 0xABCD2233);
    ASSERT_EQ(mod->rd_valid_out, 1);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);
}

TEST_F(CommitTest, Exception) {
    /* TODO
    reset();

    mod->execute_valid = 1;
    mod->execute_rd = 1;
    mod->execute_rd_val = 0xABCD2233;
    mod->execute_jump_valid = 0;
    mod->execute_exception_valid = 1;
    mod->execute_exception_num = 6;
    mod->execute_exception_val = 0xAABB9876;
    mod->execute_store_valid = 0;
    clk();
    mod->execute_valid = 0;
    eval();

    ASSERT_EQ(mod->commit_valid, 1);
    ASSERT_EQ(mod->execute_stall, 0);
    ASSERT_EQ(mod->pipeline_flush, 1);
    ASSERT_EQ(mod->pipeline_pc, 0x80004004);
    ASSERT_EQ(mod->rd_out, 1);
    ASSERT_EQ(mod->rd_val_out, 0xABCD2233);
    ASSERT_EQ(mod->rd_valid_out, 1);

    clk();
    ASSERT_EQ(mod->commit_valid, 0);
    ASSERT_EQ(mod->execute_stall, 0);
    */
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

