#include <gmock/gmock.h>
#include <gtest/gtest.h>
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
    testing::StrictMock<SdEventMock> mock;
    sd_event *const expected_event = reinterpret_cast<sd_event *>(1234);
};

TEST_F(EventTest, NewEventRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    Event event(expected_event, &mock);

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, NewEventNoRef)
{
    Event event(expected_event, std::false_type(), &mock);

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, GetNewEvent)
{
    EXPECT_CALL(mock, sd_event_new(testing::_))
        .WillOnce(DoAll(SetArgPointee<0>(expected_event), Return(0)));
    Event event = Event::get_new(&mock);

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

    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(EventTest, GetDefaultEventFail)
{
    EXPECT_CALL(mock, sd_event_default(testing::_)).WillOnce(Return(-EINVAL));
    EXPECT_THROW(Event::get_default(&mock), SdEventError);
}

TEST_F(EventTest, LoopSuccess)
{
    EXPECT_CALL(mock, sd_event_loop(expected_event)).WillOnce(Return(0));
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
    EXPECT_EQ(0, Event(expected_event, std::false_type(), &mock).loop());
}

TEST_F(EventTest, LoopUserError)
{
    const int user_error = 10;
    EXPECT_CALL(mock, sd_event_loop(expected_event))
        .WillOnce(Return(user_error));
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
    EXPECT_EQ(user_error,
              Event(expected_event, std::false_type(), &mock).loop());
}

TEST_F(EventTest, LoopInternalError)
{
    EXPECT_CALL(mock, sd_event_loop(expected_event)).WillOnce(Return(-EINVAL));
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
    EXPECT_THROW(Event(expected_event, std::false_type(), &mock).loop(),
                 SdEventError);
}

} // namespace
} // namespace sdeventplus
