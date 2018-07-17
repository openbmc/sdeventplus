#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sdeventplus/internal/sdref.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <systemd/sd-event.h>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace
{

class SdRefTest : public testing::Test
{
  protected:
    sd_event *const expected_event = reinterpret_cast<sd_event *>(1234);
    sd_event *const expected_event2 = reinterpret_cast<sd_event *>(2345);
    testing::StrictMock<SdEventMock> mock;
    testing::StrictMock<SdEventMock> mock2;
};

TEST_F(SdRefTest, ConstructRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(testing::Return(expected_event));
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, &mock);
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, ConstructNoRef)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, CopyConstruct)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());

    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(testing::Return(expected_event));
    SdRef<sd_event> event2(event);
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .Times(2)
        .WillRepeatedly(testing::Return(nullptr));
}

TEST_F(SdRefTest, MoveConstruct)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());

    SdRef<sd_event> event2(std::move(event));
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, CopyAssignOverValid)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());
    SdRef<sd_event> event2(expected_event2, &SdEventInterface::sd_event_ref,
                           &SdEventInterface::sd_event_unref, std::false_type(),
                           &mock2);
    EXPECT_EQ(expected_event2, event2.get());

    EXPECT_CALL(mock2, sd_event_unref(expected_event2))
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(testing::Return(expected_event));
    event2 = event;
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .Times(2)
        .WillRepeatedly(testing::Return(nullptr));
}

TEST_F(SdRefTest, CopyAssignOverMoved)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());
    SdRef<sd_event> event2(expected_event2, &SdEventInterface::sd_event_ref,
                           &SdEventInterface::sd_event_unref, std::false_type(),
                           &mock2);
    EXPECT_EQ(expected_event2, event2.get());
    {
        SdRef<sd_event> event_mover(std::move(event2));
        EXPECT_EQ(expected_event2, event_mover.get());

        EXPECT_CALL(mock2, sd_event_unref(expected_event2))
            .WillOnce(testing::Return(nullptr));
    }

    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(testing::Return(expected_event));
    event2 = event;
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .Times(2)
        .WillRepeatedly(testing::Return(nullptr));
}

TEST_F(SdRefTest, CopySelf)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());

    event = event;
    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, MoveAssignOverValid)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());
    SdRef<sd_event> event2(expected_event2, &SdEventInterface::sd_event_ref,
                           &SdEventInterface::sd_event_unref, std::false_type(),
                           &mock2);
    EXPECT_EQ(expected_event2, event2.get());

    EXPECT_CALL(mock2, sd_event_unref(expected_event2))
        .WillOnce(testing::Return(nullptr));
    event2 = std::move(event);
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, MoveAssignOverMoved)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());
    SdRef<sd_event> event2(expected_event2, &SdEventInterface::sd_event_ref,
                           &SdEventInterface::sd_event_unref, std::false_type(),
                           &mock2);
    EXPECT_EQ(expected_event2, event2.get());
    {
        SdRef<sd_event> event_mover(std::move(event2));
        EXPECT_EQ(expected_event2, event_mover.get());

        EXPECT_CALL(mock2, sd_event_unref(expected_event2))
            .WillOnce(testing::Return(nullptr));
    }

    event2 = std::move(event);
    EXPECT_EQ(expected_event, event2.get());

    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

TEST_F(SdRefTest, MoveSelf)
{
    SdRef<sd_event> event(expected_event, &SdEventInterface::sd_event_ref,
                          &SdEventInterface::sd_event_unref, std::false_type(),
                          &mock);
    EXPECT_EQ(expected_event, event.get());
    event = std::move(event);
    EXPECT_CALL(mock, sd_event_unref(expected_event))
        .WillOnce(testing::Return(nullptr));
}

} // namespace
} // namespace sdeventplus
