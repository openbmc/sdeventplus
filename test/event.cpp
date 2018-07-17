#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <type_traits>

namespace sdeventplus
{
namespace
{

using testing::DoAll;
using testing::Return;
using testing::SetArgPointee;

class EventTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event* const expected_event = reinterpret_cast<sd_event*>(1234);
};

TEST_F(EventTest, NewEventRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    Event event(expected_event, &mock);
    EXPECT_EQ(&mock, event.getSdEvent());
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, NewEventNoRef)
{
    Event event(expected_event, std::false_type(), &mock);
    EXPECT_EQ(&mock, event.getSdEvent());
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, GetNewEvent)
{
    EXPECT_CALL(mock, sd_event_new(testing::_))
        .WillOnce(DoAll(SetArgPointee<0>(expected_event), Return(0)));
    Event event = Event::get_new(&mock);
    EXPECT_EQ(&mock, event.getSdEvent());
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, GetNewEventFail)
{
    EXPECT_CALL(mock, sd_event_new(testing::_)).WillOnce(Return(-EINVAL));
    EXPECT_THROW(Event::get_new(&mock), SdEventError);
}

TEST_F(EventTest, GetDefaultEvent)
{
    EXPECT_CALL(mock, sd_event_default(testing::_))
        .WillOnce(DoAll(SetArgPointee<0>(expected_event), Return(0)));
    Event event = Event::get_default(&mock);
    EXPECT_EQ(&mock, event.getSdEvent());
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, GetDefaultEventFail)
{
    EXPECT_CALL(mock, sd_event_default(testing::_)).WillOnce(Return(-EINVAL));
    EXPECT_THROW(Event::get_default(&mock), SdEventError);
}

class EventMethodTest : public EventTest
{
  protected:
    std::unique_ptr<Event> event;

    void SetUp()
    {
        event =
            std::make_unique<Event>(expected_event, std::false_type(), &mock);
    }

    void TearDown()
    {
        EXPECT_CALL(mock, sd_event_unref(expected_event))
            .WillOnce(Return(nullptr));
    }
};

TEST_F(EventMethodTest, LoopSuccess)
{
    EXPECT_CALL(mock, sd_event_loop(expected_event)).WillOnce(Return(0));
    EXPECT_EQ(0, event->loop());
}

TEST_F(EventMethodTest, LoopUserError)
{
    const int user_error = 10;
    EXPECT_CALL(mock, sd_event_loop(expected_event))
        .WillOnce(Return(user_error));
    EXPECT_EQ(user_error, event->loop());
}

TEST_F(EventMethodTest, LoopInternalError)
{
    EXPECT_CALL(mock, sd_event_loop(expected_event)).WillOnce(Return(-EINVAL));
    EXPECT_THROW(event->loop(), SdEventError);
}

TEST_F(EventMethodTest, GetWatchdogSuccess)
{
    EXPECT_CALL(mock, sd_event_get_watchdog(expected_event))
        .WillOnce(Return(0));
    EXPECT_EQ(0, event->get_watchdog());

    EXPECT_CALL(mock, sd_event_get_watchdog(expected_event))
        .WillOnce(Return(2));
    EXPECT_EQ(2, event->get_watchdog());
}

TEST_F(EventMethodTest, GetWatchdogError)
{
    EXPECT_CALL(mock, sd_event_get_watchdog(expected_event))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(event->get_watchdog(), SdEventError);
}

TEST_F(EventMethodTest, SetWatchdogSuccess)
{
    EXPECT_CALL(mock, sd_event_set_watchdog(expected_event, 0))
        .WillOnce(Return(0));
    EXPECT_EQ(0, event->set_watchdog(0));

    EXPECT_CALL(mock, sd_event_set_watchdog(expected_event, 1))
        .WillOnce(Return(2));
    EXPECT_EQ(2, event->set_watchdog(1));
}

TEST_F(EventMethodTest, SetWatchdogError)
{
    EXPECT_CALL(mock, sd_event_set_watchdog(expected_event, 1))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(event->set_watchdog(1), SdEventError);
}

} // namespace
} // namespace sdeventplus
