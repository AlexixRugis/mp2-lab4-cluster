#pragma once
#include <cstdint>
#include <vector>
#include "processor_state.h"

struct cluster_state
{
    uint64_t freeProcessors;
    uint64_t currentTick;

    std::vector<processor_state> processors;
};