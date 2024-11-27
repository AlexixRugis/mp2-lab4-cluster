#include "cluster_statistics.h"

std::ostream& operator<<(std::ostream& ostr, const cluster_statistics& stats)
{
    ostr << "Ticks: " << stats.ticks()
        << "\nAdded programs: " << stats.programs_added()
        << "\nStarted programs: " << stats.programs_started()
        << "\nDone programs: " << stats.programs_done()
        << "\nAverage load: " << stats.average_load() * 100.0 << "%";
    return ostr;
}