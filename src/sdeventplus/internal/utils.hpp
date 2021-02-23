#pragma once

#include <sdeventplus/exception.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace sdeventplus
{
namespace internal
{

/** @brief Constructs an SdEventError for stdplus cexec
 */
inline SdEventError makeError(int error, const char* msg)
{
    return SdEventError(error, msg);
}

template <typename... Args>
inline auto callCheck(const char* msg, Args&&... args)
{
    return stdplus::util::callCheckRet<makeError, Args...>(
        msg, std::forward<Args>(args)...);
}

} // namespace internal
} // namespace sdeventplus
