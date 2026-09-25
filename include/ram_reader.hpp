#pragma once

struct MemInfo{
    long total_kb=0;
    long available_kb=0;
    long used_kb=0;
};

MemInfo read_mem_info();