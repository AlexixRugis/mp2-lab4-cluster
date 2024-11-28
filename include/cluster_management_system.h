#pragma once
#include <optional>
#include <cstdint>
#include <vector>
#include <exception>
#include <unordered_set>
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
        std::unordered_set<uint64_t> completedPrograms;

        for (auto& p : mState.processors)
        {
            if (p.busy && p.tickEnd <= mState.currentTick)
            {
                p.busy = false;
                ++mState.freeProcessors;
                completedPrograms.insert(p.programId);
            }
        }

        mProgramsDone += completedPrograms.size();
    }

    bool try_start_program()
    {
        std::optional<std::pair<uint64_t, program_info>> program = mQueue.get(mState);
        if (!program.has_value())
        {
            return false;
        }

        const uint64_t programId = program.value().first;
        const program_info& program_ref = program.value().second;
        uint64_t processorsRemain = program_ref.processors;

        for (auto& p : mState.processors)
        {
            if (processorsRemain == 0) break;
            if (!p.busy)
            {
                p = processor_state(
                    mState.currentTick,
                    mState.currentTick + program_ref.executionTime,
                    programId,
                    true
                );

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

        mQueue.push(info, mState.currentTick);
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