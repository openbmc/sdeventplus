#pragma once

#include <chrono>
#include <cstdint>
#include <sdeventplus/exception.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace sdeventplus
{

// Defined by systemd taking uint64_t usec params
using SdEventDuration =
    std::chrono::duration<uint64_t, std::chrono::microseconds::period>;

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
