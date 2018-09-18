#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sdeventplus/test/sdevent.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace utility
{
namespace
{

class TimerTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event* const expected_event = reinterpret_cast<sd_event*>(1234);
};

TEST_F(TimerTest, NewEventRef)
{
}

} // namespace
} // namespace utility
} // namespace sdeventplus
