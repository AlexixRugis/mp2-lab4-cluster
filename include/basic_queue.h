#pragma once
#include <optional>
#include "linked_list.h"
#include "program_info.h"
#include "cluster_state.h"

class basic_queue
{
private:
    uint64_t mNextId;
    linked_list<std::pair<uint64_t, program_info>> mPrograms;
public:
    basic_queue() : mNextId(0) {}

    std::optional<std::pair<uint64_t, program_info>> get(const cluster_state& state)
    {
        if (mPrograms.size() == 0 || mPrograms.front().second.processors > state.freeProcessors)
        {
            return std::nullopt;
        }

        std::pair<uint64_t, program_info> p = mPrograms.front();
        mPrograms.pop_front();
        return p;
    }

    uint64_t push(const program_info& program, uint64_t tick)
    {
        uint64_t id = mNextId++;
        mPrograms.push_back({ id, program });
        return id;
    }
};