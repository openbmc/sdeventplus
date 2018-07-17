#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <type_traits>

namespace sdeventplus
{
namespace source
{
namespace
{

using testing::Return;

class BaseImpl : public Base
{
  public:
    BaseImpl(sd_event_source* source, internal::SdEvent* sdevent) :
        Base(source, sdevent)
    {
    }
    BaseImpl(sd_event_source* source, std::false_type,
             internal::SdEvent* sdevent) :
        Base(source, std::false_type(), sdevent)
    {
    }
};

class BaseTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event_source* const expected_source =
        reinterpret_cast<sd_event_source*>(1234);
};

TEST_F(BaseTest, NewBaseRef)
{
    EXPECT_CALL(mock, sd_event_source_ref(expected_source))
        .WillOnce(Return(expected_source));
    BaseImpl source(expected_source, &mock);

    EXPECT_CALL(mock, sd_event_source_unref(expected_source))
        .WillOnce(Return(nullptr));
}

TEST_F(BaseTest, NewBaseNoRef)
{
    BaseImpl source(expected_source, std::false_type(), &mock);

    EXPECT_CALL(mock, sd_event_source_unref(expected_source))
        .WillOnce(Return(nullptr));
}

} // namespace
} // namespace source
} // namespace sdeventplus
