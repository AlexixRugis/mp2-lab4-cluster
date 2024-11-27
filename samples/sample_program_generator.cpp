#include <iostream>
#include <random>
#include "cluster_management_system.h"
#include "queue.h"

using namespace std;

random_device rd;
default_random_engine g(rd());

program_info generate_program()
{
    uniform_int_distribution<uint64_t> processors_dist(1, 64);
    uniform_int_distribution<uint64_t> ticks_dist(1, 200);

    return { processors_dist(g), ticks_dist(g) };
}

int main() {
    uniform_real_distribution<float> program_gen_dist;

    cluster_management_system<basic_queue> cms(64);

    for (size_t i = 0; i < 100000; ++i)
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