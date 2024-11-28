#pragma once
#include <cstdint>

struct processor_state
{
    uint64_t tickStart;
    uint64_t tickEnd;
    uint64_t programId;
    bool busy;

    processor_state(uint64_t start = 0, uint64_t end = 0, uint64_t pid = 0, bool busy_ = false)
        : tickStart(start), tickEnd(end), programId(pid), busy(busy_)
    {}
};