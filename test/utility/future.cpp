#include <chrono>
#include <gtest/gtest.h>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <sdeventplus/utility/future.hpp>
#include <stdplus/util/future.hpp>
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
    stdplus::util::Promise<int> p;
    size_t cbCalls = 0;
    auto cb = [&](const Event&, stdplus::util::Future<int>&& f) {
        f.get(); // Ensure the future is ready since this will block otherwise
        cbCalls++;
    };
    callWhenReady(event, std::move(cb), p.get_future()).set_floating(true);

    // Ensure that the cb is not evaluated without the promised value
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(0, event.wait(0s));

    // Ensure the promise setting triggered new events to run
    p.set_value(10);
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

TEST_F(CallWhenReady, TwoFutures)
{
    stdplus::util::Promise<int> p1, p2;
    size_t cbCalls = 0;
    auto cb = [&](const Event&, stdplus::util::Future<int>&& f1,
                  stdplus::util::Future<int>&& f2) {
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

    // Only fullfill a single promise
    p1.set_value(10);

    // Add a new event and ensure we aren't executed
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(0, cbCalls);
    EXPECT_EQ(0, event.wait(0s));

    // Fullfill all promises and ensure the loop is triggered
    p2.set_value(20);
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

TEST_F(CallWhenReady, VariableSize)
{
    std::vector<stdplus::util::Future<int>> v;
    for (int i = 0; i < 5; ++i)
    {
        stdplus::util::Promise<int> p;
        v.push_back(p.get_future());
        p.set_value(i);
    }
    int val = 0;
    auto cb = [&](const Event&, std::vector<stdplus::util::Future<int>>&& vs) {
        for (const auto& v : vs)
        {
            val += v.get();
        }
    };
    callWhenReady(event, std::move(cb), std::move(v)).set_floating(true);
    while (event.prepare() > 0)
    {
        EXPECT_EQ(1, event.dispatch());
    }
    EXPECT_EQ(0, event.wait(0s));
    EXPECT_EQ(10, val);
}

} // namespace
} // namespace utility
} // namespace sdeventplus
