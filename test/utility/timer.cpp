#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <stdexcept>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace utility
{
namespace
{

constexpr ClockId testClock = ClockId::Monotonic;

using std::chrono::microseconds;
using std::chrono::milliseconds;
using testing::DoAll;
using testing::Return;
using testing::SaveArg;
using testing::SetArgPointee;
using TestTimer = Timer<testClock>;

ssize_t event_ref_times = 0;

ACTION(EventRef)
{
    event_ref_times++;
}

ACTION(EventUnref)
{
    ASSERT_LT(0, event_ref_times);
    event_ref_times--;
}

class TimerTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event* const expected_event = reinterpret_cast<sd_event*>(1234);
    sd_event_source* const expected_source =
        reinterpret_cast<sd_event_source*>(2345);
    const milliseconds interval{134};
    const milliseconds starting_time{10};
    sd_event_time_handler_t handler = nullptr;
    void* handler_userdata;
    std::unique_ptr<TestTimer> timer;
    std::function<void()> callback;

    void expectNow(microseconds ret)
    {
        EXPECT_CALL(mock,
                    sd_event_now(expected_event,
                                 static_cast<clockid_t>(testClock), testing::_))
            .WillOnce(DoAll(SetArgPointee<2>(ret.count()), Return(0)));
    }

    void expectSetTime(microseconds time)
    {
        EXPECT_CALL(mock,
                    sd_event_source_set_time(expected_source, time.count()))
            .WillOnce(Return(0));
    }

    void expectSetEnabled(source::Enabled enabled)
    {
        EXPECT_CALL(mock, sd_event_source_set_enabled(
                              expected_source, static_cast<int>(enabled)))
            .WillOnce(Return(0));
    }

    void expectGetEnabled(source::Enabled enabled)
    {
        EXPECT_CALL(mock,
                    sd_event_source_get_enabled(expected_source, testing::_))
            .WillOnce(
                DoAll(SetArgPointee<1>(static_cast<int>(enabled)), Return(0)));
    }

    void SetUp()
    {
        EXPECT_CALL(mock, sd_event_ref(expected_event))
            .WillRepeatedly(DoAll(EventRef(), Return(expected_event)));
        EXPECT_CALL(mock, sd_event_unref(expected_event))
            .WillRepeatedly(DoAll(EventUnref(), Return(nullptr)));
        Event event(expected_event, &mock);

        auto runCallback = [&]() {
            if (callback)
            {
                callback();
            }
        };
        expectNow(starting_time);
        EXPECT_CALL(mock, sd_event_add_time(
                              expected_event, testing::_,
                              static_cast<clockid_t>(testClock),
                              microseconds(starting_time + interval).count(),
                              1000, testing::_, nullptr))
            .WillOnce(DoAll(SetArgPointee<1>(expected_source),
                            SaveArg<5>(&handler), Return(0)));
        EXPECT_CALL(mock,
                    sd_event_source_set_userdata(expected_source, testing::_))
            .WillOnce(DoAll(SaveArg<1>(&handler_userdata), Return(nullptr)));
        // Timer always enables the source to keep ticking
        expectSetEnabled(source::Enabled::On);
        timer = std::make_unique<TestTimer>(event, runCallback, interval);
    }

    void TearDown()
    {
        expectSetEnabled(source::Enabled::Off);
        EXPECT_CALL(mock, sd_event_source_unref(expected_source))
            .WillOnce(Return(nullptr));
        timer.reset();
        EXPECT_EQ(0, event_ref_times);
    }
};

TEST_F(TimerTest, NewTimer)
{
    EXPECT_FALSE(timer->hasExpired());
    EXPECT_EQ(interval, timer->getInterval());
}

TEST_F(TimerTest, IsEnabled)
{
    expectGetEnabled(source::Enabled::On);
    EXPECT_TRUE(timer->isEnabled());
    expectGetEnabled(source::Enabled::Off);
    EXPECT_FALSE(timer->isEnabled());
}

TEST_F(TimerTest, GetRemainingDisabled)
{
    expectGetEnabled(source::Enabled::Off);
    EXPECT_THROW(timer->getRemaining(), std::runtime_error);
}

TEST_F(TimerTest, GetRemainingNegative)
{
    milliseconds now(675), end(453);
    expectGetEnabled(source::Enabled::On);
    EXPECT_CALL(mock, sd_event_source_get_time(expected_source, testing::_))
        .WillOnce(
            DoAll(SetArgPointee<1>(microseconds(end).count()), Return(0)));
    expectNow(now);
    EXPECT_EQ(milliseconds(0), timer->getRemaining());
}

TEST_F(TimerTest, GetRemainingPositive)
{
    milliseconds now(453), end(675);
    expectGetEnabled(source::Enabled::On);
    EXPECT_CALL(mock, sd_event_source_get_time(expected_source, testing::_))
        .WillOnce(
            DoAll(SetArgPointee<1>(microseconds(end).count()), Return(0)));
    expectNow(now);
    EXPECT_EQ(end - now, timer->getRemaining());
}

TEST_F(TimerTest, SetEnabled)
{
    expectSetEnabled(source::Enabled::On);
    timer->setEnabled(true);
    EXPECT_FALSE(timer->hasExpired());
    // Value should always be passed through regardless of current state
    expectSetEnabled(source::Enabled::On);
    timer->setEnabled(true);
    EXPECT_FALSE(timer->hasExpired());

    expectSetEnabled(source::Enabled::Off);
    timer->setEnabled(false);
    EXPECT_FALSE(timer->hasExpired());
    // Value should always be passed through regardless of current state
    expectSetEnabled(source::Enabled::Off);
    timer->setEnabled(false);
    EXPECT_FALSE(timer->hasExpired());
}

TEST_F(TimerTest, SetRemaining)
{
    const milliseconds now(90), remaining(30);
    expectNow(now);
    expectSetTime(now + remaining);
    timer->setRemaining(remaining);
    EXPECT_EQ(interval, timer->getInterval());
    EXPECT_FALSE(timer->hasExpired());
}

TEST_F(TimerTest, ResetRemaining)
{
    const milliseconds now(90);
    expectNow(now);
    expectSetTime(now + interval);
    timer->resetRemaining();
    EXPECT_EQ(interval, timer->getInterval());
    EXPECT_FALSE(timer->hasExpired());
}

TEST_F(TimerTest, SetInterval)
{
    const milliseconds new_interval(40);
    timer->setInterval(new_interval);
    EXPECT_EQ(new_interval, timer->getInterval());
    EXPECT_FALSE(timer->hasExpired());
}

TEST_F(TimerTest, SetValuesExpiredTimer)
{
    const milliseconds new_time(90);
    expectNow(new_time);
    expectSetTime(new_time + interval);
    EXPECT_EQ(0, handler(nullptr, 0, handler_userdata));
    EXPECT_TRUE(timer->hasExpired());
    EXPECT_EQ(interval, timer->getInterval());

    // Timer should remain expired unless clearExpired() or reset()
    expectSetEnabled(source::Enabled::On);
    timer->setEnabled(true);
    EXPECT_TRUE(timer->hasExpired());
    expectNow(milliseconds(20));
    expectSetTime(milliseconds(50));
    timer->setRemaining(milliseconds(30));
    EXPECT_TRUE(timer->hasExpired());
    timer->setInterval(milliseconds(10));
    EXPECT_TRUE(timer->hasExpired());
    expectNow(milliseconds(20));
    expectSetTime(milliseconds(30));
    timer->resetRemaining();
    EXPECT_TRUE(timer->hasExpired());

    timer->clearExpired();
    EXPECT_FALSE(timer->hasExpired());
}

TEST_F(TimerTest, Restart)
{
    const milliseconds new_time(90);
    expectNow(new_time);
    expectSetTime(new_time + interval);
    EXPECT_EQ(0, handler(nullptr, 0, handler_userdata));
    EXPECT_TRUE(timer->hasExpired());
    EXPECT_EQ(interval, timer->getInterval());

    const milliseconds new_interval(471);
    expectNow(starting_time);
    expectSetTime(starting_time + new_interval);
    expectSetEnabled(source::Enabled::On);
    timer->restart(new_interval);
    EXPECT_FALSE(timer->hasExpired());
    EXPECT_EQ(new_interval, timer->getInterval());
}

} // namespace
} // namespace utility
} // namespace sdeventplus
