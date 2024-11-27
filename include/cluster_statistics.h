#pragma once
#include <cstdint>

class cluster_statistics final
{
private:
    uint64_t mProgramsAdded;
    uint64_t mProgramsDone;
    uint64_t mProgramsStarted;

    uint64_t mTicks;
    double mAverageLoad;

public:
    cluster_statistics(uint64_t programsAdded, uint64_t programsDone, uint64_t programsStarted, uint64_t ticks, double averageLoad)
        : mProgramsAdded(programsAdded), mProgramsDone(programsDone), mProgramsStarted(programsStarted),
        mTicks(ticks), mAverageLoad(averageLoad) {}

    uint64_t programs_added() const noexcept { return mProgramsAdded; }
    uint64_t programs_done() const noexcept { return mProgramsDone; }
    uint64_t programs_started() const noexcept { return mProgramsStarted; }
    uint64_t ticks() const noexcept { return mTicks; }
    double average_load() const noexcept { return mAverageLoad; }
};