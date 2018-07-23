#pragma once

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <exception>
#include <sdeventplus/exception.hpp>
#include <stdexcept>

namespace sdeventplus
{

// Defined by systemd taking uint64_t usec params
using SdEventDuration =
    std::chrono::duration<uint64_t, std::chrono::microseconds::period>;

namespace internal
{

/** @brief Handle sd_event callback exception gracefully
 *  @details A generic wrapper that turns exceptions into
 *           error messages and return codes.
 */
template <typename Func, typename... Args>
inline int performCallback(const char* name, Func func, Args... args)
{
    try
    {
        func(args...);
        return 0;
    }
    catch (const std::system_error& e)
    {
        fprintf(stderr, "sdeventplus: %s: %s\n", name, e.what());
        return -e.code().value();
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "sdeventplus: %s: %s\n", name, e.what());
        return -ENOSYS;
    }
    catch (...)
    {
        fprintf(stderr, "sdeventplus: %s: Unknown error\n", name);
        return -ENOSYS;
    }
}

} // namespace internal
} // namespace sdeventplus
