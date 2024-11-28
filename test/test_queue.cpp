#include <gtest/gtest.h>
#include "basic_queue.h"
#include "planning_queue.h"
#include "priority_queue.h"
#include "cluster_state.h"
#include "program_info.h"

TEST(BasicQueueTest, can_push_and_get)
{
    basic_queue queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };
    program_info prog1{ 2, 5 };

    uint64_t id = queue.push(prog1, 0);
    EXPECT_EQ(id, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);
    EXPECT_EQ(result->second.processors, 2);
    EXPECT_EQ(result->second.executionTime, 5);
}

TEST(BasicQueueTest, cant_get_from_empty_queue)
{
    basic_queue queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };
    auto result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(BasicQueueTest, cant_get_with_insufficient_processors)
{
    basic_queue queue;
    cluster_state state{ 2, 0, std::vector<processor_state>(2) };
    program_info prog1{ 4, 5 };

    queue.push(prog1, 0);
    auto result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PlanningQueueTest, can_push_and_get)
{
    planning_queue<3> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };
    program_info prog1{ 2, 5 };

    uint64_t id = queue.push(prog1, 0);
    EXPECT_EQ(id, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);
    EXPECT_EQ(result->second.processors, 2);
    EXPECT_EQ(result->second.executionTime, 5);
}

TEST(PlanningQueueTest, cant_get_with_insufficient_processors)
{
    planning_queue<1> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };

    program_info prog1{ 2, 10 };
    program_info prog2{ 3, 5 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);

    state.freeProcessors -= 2;
    result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PlanningQueueTest, can_get_after_first_in_queue)
{
    planning_queue<2> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };

    program_info prog1{ 2, 10 };
    program_info prog2{ 3, 5 };
    program_info prog3{ 2, 6 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);
    queue.push(prog3, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);

    state.freeProcessors -= 2;
    result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 2);
}

TEST(PlanningQueueTest, cant_get_after_lookahead)
{
    planning_queue<1> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };

    program_info prog1{ 2, 10 };
    program_info prog2{ 3, 5 };
    program_info prog3{ 2, 6 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);
    queue.push(prog3, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);

    state.freeProcessors -= 2;
    result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PlanningQueueTest, cant_get_with_insufficient_time)
{
    planning_queue<1> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };

    program_info prog1{ 2, 5 };
    program_info prog2{ 3, 5 };
    program_info prog3{ 2, 6 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);
    queue.push(prog3, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);

    state.freeProcessors -= 2;
    result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PlanningQueueTest, cant_get_from_empty_queue)
{
    planning_queue<> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };

    auto result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PriorityQueueTest, can_push_and_get)
{
    priority_queue<5> queue;
    cluster_state state{ 4, 0, std::vector<processor_state>(4) };
    program_info prog1{ 2, 5 };

    uint64_t id = queue.push(prog1, 0);
    EXPECT_EQ(id, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->first, 0);
    EXPECT_EQ(result->second.processors, 2);
    EXPECT_EQ(result->second.executionTime, 5);
}

TEST(PriorityQueueTest, cant_get_from_empty_queue)
{
    priority_queue<3> queue;
    cluster_state state{ 2, 0, std::vector<processor_state>(2) };

    auto result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PriorityQueueTest, can_get_after_first)
{
    priority_queue<3> queue;
    cluster_state state{ 2, 0, std::vector<processor_state>(2) };
    
    program_info prog1{ 1, 5 };
    program_info prog2{ 2, 5 };
    program_info prog3{ 1, 5 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);
    queue.push(prog3, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().first, 0);
    
    state.freeProcessors -= 1;
    state.currentTick++;
    result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().first, 2);

    state.freeProcessors -= 1;
    state.currentTick++;
    result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}

TEST(PriorityQueueTest, cant_get_after_delay_has_expired)
{
    priority_queue<0> queue;
    cluster_state state{ 2, 0, std::vector<processor_state>(2) };

    program_info prog1{ 1, 5 };
    program_info prog2{ 2, 5 };
    program_info prog3{ 1, 5 };

    queue.push(prog1, 0);
    queue.push(prog2, 0);
    queue.push(prog3, 0);

    auto result = queue.get(state);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().first, 0);

    state.freeProcessors -= 1;
    state.currentTick++;
    result = queue.get(state);
    EXPECT_FALSE(result.has_value());
}