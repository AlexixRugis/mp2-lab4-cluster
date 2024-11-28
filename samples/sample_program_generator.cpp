#include <iostream>
#include <random>
#include "cluster_management_system.h"
#include "basic_queue.h"
#include "priority_queue.h"
#include "planning_queue.h"

using namespace std;

random_device rd;
default_random_engine g(rd());

program_info generate_program()
{
    uniform_int_distribution<uint64_t> processors_dist(1, 64);
    uniform_int_distribution<uint64_t> ticks_dist(1, 200);

    return { processors_dist(g), ticks_dist(g) };
}

int main()
{
    g.seed(0);

    uniform_real_distribution<float> program_gen_dist;

    //cluster_management_system<basic_queue> cms(64);
    //cluster_management_system<priority_queue<1000>> cms(64);
    cluster_management_system<planning_queue<100>> cms(64);

    for (size_t i = 0; i < 1000000; ++i)
    {
        for (size_t j = 0; j < 64; ++j)
        {
            if (program_gen_dist(g) <= 0.05)
            {
                cms.add_program(generate_program());
            }
        }

        cms.tick();
    }

    cout << cms.get_statistics() << endl;
    return 0;
}