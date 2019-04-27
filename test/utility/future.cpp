#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <sdeventplus/utility/future.hpp>
#include <utility>

namespace sdeventplus
{
namespace utility
{
namespace
{

using namespace std::chrono_literals;

class CallWhenReady : public testing::Test
{
  protected:
    Event event = Event::get_new();
};

TEST_F(CallWhenReady, NoFutures)
{
    size_t cbCalls = 0;
    callWhenReady(event, [&](const Event&) { cbCalls++; }).set_floating(true);

    // Ensure that the cb is run immediately, without any other events
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(1, cbCalls);
    EXPECT_EQ(0, event.wait(0s));

    // Ensure we are called only once
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(1, cbCalls);
    EXPECT_EQ(0, event.wait(0s));
}

TEST_F(CallWhenReady, BasicFuture)
{
    std::promise<int> p;
    size_t cbCalls = 0;
    auto cb = [&](const Event&, std::future<int>&& f) {
        f.get(); // Ensure the future is ready since this will block otherwise
        cbCalls++;
    };
    callWhenReady(event, std::move(cb), p.get_future()).set_floating(true);

    // Ensure that the cb is not evaluated without the promised value
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    p.set_value(10);
    // Ensure the promise setting didn't trigger new events to run
    EXPECT_EQ(0, event.wait(0s));

    // Add a new event and and ensure the promise is fullfilled
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(1, cbCalls);
    EXPECT_EQ(0, event.wait(0s));
}

TEST_F(CallWhenReady, TwoFutures)
{
    std::promise<int> p1, p2;
    size_t cbCalls = 0;
    auto cb = [&](const Event&, std::future<int>&& f1, std::future<int>&& f2) {
        f1.get();
        f2.get();
        cbCalls++;
    };
    callWhenReady(event, std::move(cb), p1.get_future(), p2.get_future())
        .set_floating(true);

    // Ensure that the cb is not evaluated without any promised value
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(0, event.wait(0s));

    // Add a new event and only fullfill a single promise
    p1.set_value(10);
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(0, cbCalls);
    EXPECT_EQ(0, event.wait(0s));

    // Add a new event and fullfill all promises
    p2.set_value(20);
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(1, cbCalls);
    EXPECT_EQ(0, event.wait(0s));
}

} // namespace
} // namespace utility
} // namespace sdeventplus
