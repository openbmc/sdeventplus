#include <gtest/gtest.h>
#include <sdeventplus/source/event.hpp>
#include <sdeventplus/utility/sdbus.hpp>

namespace sdeventplus::utility
{

struct LoopWithBus : testing::Test
{
    Event event = Event::get_new();
    sdbusplus::bus_t bus = sdbusplus::bus::new_bus();
};

TEST_F(LoopWithBus, ImmediateExit)
{
    event.exit(0);
    EXPECT_EQ(0, loopWithBus(event, bus));
}

TEST_F(LoopWithBus, DelayedExit)
{
    source::Defer(event, [](source::EventBase& b) {
        b.get_event().exit(1);
    }).set_floating(true);
    EXPECT_EQ(1, loopWithBus(event, bus));
}

TEST_F(LoopWithBus, ExitSources)
{
    int d1 = 0, d2 = 0;
    source::Exit(event, [&](source::EventBase&) { d1 = 1; }).set_floating(true);
    source::Defer(event, [&](source::EventBase& b) {
        source::Exit(event, [&](source::EventBase&) {
            d2 = 2;
        }).set_floating(true);
        b.get_event().exit(3);
    }).set_floating(true);
    EXPECT_EQ(3, loopWithBus(event, bus));
    EXPECT_EQ(1, d1);
    EXPECT_EQ(2, d2);
}

} // namespace sdeventplus::utility
