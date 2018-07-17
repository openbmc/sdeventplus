#include <gtest/gtest.h>
#include <sdeventplus/internal/utils.hpp>
#include <stdexcept>
#include <system_error>

namespace sdeventplus
{
namespace internal
{
namespace
{

TEST(UtilsTest, PerformCallbackSuccess)
{
    EXPECT_EQ(0, performCallback([]() {}));
}

TEST(UtilsTest, SetPrepareSystemError)
{
    EXPECT_EQ(-EBUSY, performCallback([]() {
        throw std::system_error(EBUSY, std::generic_category());
    }));
}

TEST(UtilsTest, SetPrepareException)
{
    EXPECT_EQ(-ENOSYS,
              performCallback([]() { throw std::runtime_error("Exception"); }));
}

TEST(UtilsTest, SetPrepareUnknownException)
{
    EXPECT_EQ(-ENOSYS, performCallback([]() { throw static_cast<int>(1); }));
}

} // namespace
} // namespace internal
} // namespace sdeventplus
