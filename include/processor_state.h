#pragma once
#include <cstdint>

struct processor_state
{
    uint64_t tickStart;
    uint64_t tickEnd;
    bool busy;

    processor_state() : tickStart(0), tickEnd(0), busy(false) {}
};