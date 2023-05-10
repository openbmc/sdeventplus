#pragma once
#include <chrono>
#include <cstdint>

namespace sdeventplus
{

// Defined by systemd taking uint64_t usec params
using SdEventDuration =
    std::chrono::duration<uint64_t, std::chrono::microseconds::period>;

namespace internal
{

/* @brief Indicates that the container should not own the underlying
 *        sd_event primative */
struct NoOwn
{};

} // namespace internal
} // namespace sdeventplus
