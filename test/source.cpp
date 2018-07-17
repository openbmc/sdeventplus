#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sdeventplus/source.hpp>
#include <sdeventplus/test/sdevent.hpp>

namespace sdeventplus
{
namespace
{

using testing::Return;

class SourceTest : public testing::Test
{
  protected:
    testing::StrictMock<SdEventMock> mock;
    sd_event_source *const expected_source =
        reinterpret_cast<sd_event_source *>(1234);
};

TEST_F(SourceTest, NewSourceRef)
{
    EXPECT_CALL(mock, sd_event_source_ref(expected_source))
        .WillOnce(Return(expected_source));
    Source source(expected_source, &mock);

    EXPECT_CALL(mock, sd_event_source_unref(expected_source))
        .WillOnce(Return(nullptr));
}

TEST_F(SourceTest, NewSourceNoRef)
{
    Source source(expected_source, std::false_type(), &mock);

    EXPECT_CALL(mock, sd_event_source_unref(expected_source))
        .WillOnce(Return(nullptr));
}

} // namespace
} // namespace sdeventplus
