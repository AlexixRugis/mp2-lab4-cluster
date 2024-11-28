#include <gtest/gtest.h>
#include "cluster_management_system.h"
#include "basic_queue.h"
#include "planning_queue.h"
#include "priority_queue.h"

TEST(ClusterManagementSystemTest, can_create)
{
    cluster_management_system<basic_queue> cms(4);
    EXPECT_NO_THROW(cms.get_statistics());
}

TEST(ClusterManagementSystemTest, cant_create_without_processors)
{
    EXPECT_THROW(cluster_management_system<basic_queue>(0), std::invalid_argument);
}

TEST(ClusterManagementSystemTest, can_add_programs_and_tick)
{
    cluster_management_system<basic_queue> cms(10);

    cms.add_program({ 10, 1 });
    auto state = cms.get_statistics();
    EXPECT_EQ(state.programs_added(), 1);
    EXPECT_EQ(state.programs_started(), 0);
    EXPECT_EQ(state.programs_done(), 0);
    cms.tick();
    state = cms.get_statistics();
    EXPECT_EQ(state.programs_added(), 1);
    EXPECT_EQ(state.programs_started(), 1);
    EXPECT_EQ(state.programs_done(), 0);

    cms.tick();
    state = cms.get_statistics();
    EXPECT_EQ(state.programs_added(), 1);
    EXPECT_EQ(state.programs_started(), 1);
    EXPECT_EQ(state.programs_done(), 1);
}

TEST(ClusterManagementSystemTest, cant_add_invalid_programs)
{
    cluster_management_system<basic_queue> cms(4);

    program_info prog1{ 5, 5 };
    EXPECT_THROW(cms.add_program(prog1), std::invalid_argument);
}

TEST(ClusterManagementSystemTest, can_run_programs)
{
    cluster_management_system<basic_queue> cms(4);

    program_info prog1{ 2, 3 };
    program_info prog2{ 1, 2 };

    cms.add_program(prog1);
    cms.add_program(prog2);

    cms.tick();
    cluster_statistics stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_started(), 2);
    EXPECT_EQ(stats.programs_done(), 0);

    cms.tick();
    cms.tick();
    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_done(), 1);

    cms.tick();
    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_done(), 2);
}

TEST(ClusterManagementSystemTest, can_fill_all_processors)
{
    cluster_management_system<basic_queue> cms(10);

    for (int i = 0; i < 100; ++i)
    {
        cms.add_program({ 1, 1 });
    }

    for (int i = 0; i < 11; ++i)
    {
        cms.tick();
    }

    auto stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_added(), 100);
    EXPECT_EQ(stats.programs_done(), 100);
    EXPECT_EQ(stats.programs_started(), 100);
}

TEST(ClusterManagementSystemTest, can_work_with_planning_queue)
{
    cluster_management_system<planning_queue<3>> cms(4);

    cms.add_program({ 2, 5 });
    cms.add_program({ 1, 3 });

    cms.tick();
    auto stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_started(), 2);
    EXPECT_EQ(stats.programs_done(), 0);

    for (int i = 0; i < 4; ++i)
    {
        cms.tick();
    }

    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_started(), 2);
    EXPECT_EQ(stats.programs_done(), 1);

    cms.tick();
    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_done(), 2);
}

TEST(ClusterManagementSystemTest, can_work_with_priority_queue)
{
    cluster_management_system<priority_queue<5>> cms(4);

    cms.add_program({ 2, 5 });
    cms.add_program({ 3, 3 });

    cms.tick();
    auto stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_started(), 1);
    EXPECT_EQ(stats.programs_done(), 0);

    for (int i = 0; i < 5; ++i)
    {
        cms.tick();
    }

    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_started(), 2);
    EXPECT_EQ(stats.programs_done(), 1);

    for (int i = 0; i < 3; i++)
    {
        cms.tick();
    }

    stats = cms.get_statistics();
    EXPECT_EQ(stats.programs_done(), 2);
}

TEST(ClusterManagementSystemTest, cant_add_program_that_requires_more_processors_than_have)
{
    cluster_management_system<basic_queue> cms(2);

    EXPECT_THROW(cms.add_program({ 3, 5 }), std::invalid_argument);
}

TEST(ClusterManagementSystemTest, AverageLoadCalculation)
{
    cluster_management_system<basic_queue> cms(2);

    cms.add_program({ 1, 2 });
    cms.add_program({ 1, 2 });
    cms.add_program({ 1, 2 });

    cms.tick();
    cms.tick();
    cms.tick();
    cms.tick();

    auto stats = cms.get_statistics();
    EXPECT_DOUBLE_EQ(stats.average_load(), (2.0 + 2.0 + 1.0 + 1.0) / 8.0);
}