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

// Helpers for sd_event callbacks to handle exceptions gracefully
template <typename Func, typename... Args>
static int performCallback(Func func, Args... args)
{
    try
    {
        func(args...);
        return 0;
    }
    catch (const std::system_error& e)
    {
        fprintf(stderr, "sdeventplus: callback: %s\n", e.what());
        return -e.code().value();
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "sdeventplus: callback: %s\n", e.what());
        return -ENOSYS;
    }
    catch (...)
    {
        fprintf(stderr, "sdeventplus: callback: Unknown error\n");
        return -ENOSYS;
    }
}

} // namespace internal
} // namespace sdeventplus
