#pragma once
#include <optional>
#include <cstdint>
#include <vector>
#include <exception>
#include "program_info.h"
#include "cluster_statistics.h"
#include "cluster_state.h"
#include "processor_state.h"

template<typename T>
class cluster_management_system
{
private:
    T mQueue;
    cluster_state mState;

    uint64_t mProgramsAdded;
    uint64_t mProgramsDone;
    uint64_t mProgramsStarted;
    uint64_t mTotalLoad;

    void finish_programs()
    {
        for (auto& p : mState.processors)
        {
            if (p.busy && p.tickEnd <= mState.currentTick)
            {
                p.busy = false;
                ++mState.freeProcessors;
                ++mProgramsDone;
            }
        }
    }

    bool try_start_program()
    {
        std::optional<program_info> program = mQueue.get(mState);
        if (!program.has_value())
        {
            return false;
        }

        const program_info& program_ref = program.value();
        uint64_t processorsRemain = program_ref.processors;

        for (auto& p : mState.processors)
        {
            if (!p.busy)
            {
                p = {
                    mState.currentTick,
                    mState.currentTick + program_ref.executionTime,
                    true
                };

                --mState.freeProcessors;
                --processorsRemain;
            }
        }

        ++mProgramsStarted;

        return true;
    }

    uint64_t get_tick_load()
    {
        return mState.processors.size() - mState.freeProcessors;
    }
public:
    cluster_management_system(uint64_t processors)
        : mState{ processors, 0, std::vector<processor_state>(processors) },
        mProgramsAdded(0), mProgramsDone(0), mProgramsStarted(0), mTotalLoad(0)
    {
        if (processors < 1)
        {
            throw std::invalid_argument(__FUNCTION__ ": cluster must have at least one processor.");
        }
    }

    void add_program(const program_info& info)
    {
        if (info.processors > mState.processors.size())
        {
            throw std::invalid_argument(__FUNCTION__ ": can't add program that requires more processors than cluster has.");
        }

        mQueue.push(info);
        ++mProgramsAdded;
    }

    cluster_statistics get_statistics()
    {
        double averageLoad = (double)mTotalLoad / (mState.processors.size() * mState.currentTick);

        return cluster_statistics(
            mProgramsAdded,
            mProgramsDone,
            mProgramsStarted,

            mState.currentTick,
            averageLoad
        );
    }

    void tick()
    {
        ++mState.currentTick;

        finish_programs();
        while (try_start_program()) {}

        mTotalLoad += get_tick_load();
    }
};