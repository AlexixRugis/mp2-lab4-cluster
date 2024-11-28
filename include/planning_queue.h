#pragma once
#include <optional>
#include <cstdint>
#include "linked_list.h"
#include "program_info.h"
#include "cluster_state.h"

template<uint64_t LookAhead = UINT64_MAX>
class planning_queue
{
private:
    uint64_t mNextId;
    linked_list<std::pair<uint64_t, program_info>> mPrograms;

    uint64_t get_ticks_wait(const cluster_state& state, uint64_t processorsRequired)
    {
        uint64_t l = 0, r = UINT64_MAX;
        while (r - l > 1)
        {
            uint64_t m = l + (r - l) / 2;
            uint64_t c = 0;

            for (const auto& p : state.processors)
                if (p.busy && p.tickEnd < state.currentTick + m) ++c;

            if (state.freeProcessors + c >= processorsRequired) r = m;
            else l = m;
        }

        return r;
    }
public:
    planning_queue() : mNextId(0) {}

    std::optional<std::pair<uint64_t, program_info>> get(const cluster_state& state)
    {
        if (mPrograms.size() == 0)
        {
            return std::nullopt;
        }

        uint64_t requiredProcessors = mPrograms.front().second.processors;
        if (state.freeProcessors >= requiredProcessors)
        {
            std::pair<uint64_t, program_info> p = mPrograms.front();
            mPrograms.pop_front();
            return p;
        }

        uint64_t timeToStart = get_ticks_wait(state, requiredProcessors);
        
        uint64_t lookAhead = LookAhead;
        for (auto iter = mPrograms.begin(); iter != mPrograms.end() && lookAhead > 0; ++iter, --lookAhead)
        {
            const auto& p = *iter;
            if (p.second.processors <= state.freeProcessors && p.second.executionTime <= timeToStart)
            {
                std::pair<uint64_t, program_info> tmp = p;
                mPrograms.erase(iter);
                return tmp;
            }
        }
        
        return std::nullopt;
    }

    uint64_t push(const program_info& program, uint64_t tick)
    {
        uint64_t id = mNextId++;
        mPrograms.push_back({ id, program });
        return id;
    }
};