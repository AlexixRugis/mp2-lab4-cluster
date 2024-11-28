#pragma once
#include <optional>
#include <cstdint>
#include "linked_list.h"
#include "program_info.h"
#include "cluster_state.h"

template<uint64_t MaxDelay = 0LL>
class priority_queue
{
private:
    struct program_record
    {
        uint64_t program_id;
        uint64_t tick_added;
        program_info program_info;
    };

    uint64_t mNextId;
    linked_list<program_record> mPrograms;
public:
    priority_queue() : mNextId(0) {}

    std::optional<std::pair<uint64_t, program_info>> get(const cluster_state& state)
    {
        for (auto iter = mPrograms.begin(); iter != mPrograms.end(); ++iter)
        {
            const auto& p = *iter;
            if (p.program_info.processors <= state.freeProcessors)
            {
                std::pair<uint64_t, program_info> res{ p.program_id, p.program_info };
                mPrograms.erase(iter);
                return res;
            } else if (state.currentTick - p.tick_added > MaxDelay)
            {
                break;
            }
        }

        return std::nullopt;
    }

    uint64_t push(const program_info& program, uint64_t tick)
    {
        uint64_t id = mNextId++;
        mPrograms.push_back({ id, tick, program });
        return id;
    }
};