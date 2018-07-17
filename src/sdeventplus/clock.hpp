#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <sdeventplus/event.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <type_traits>

namespace sdeventplus
{

enum class ClockId : clockid_t
{
    RealTime = CLOCK_REALTIME,
    Monotonic = CLOCK_MONOTONIC,
    BootTime = CLOCK_BOOTTIME,
    RealTimeAlarm = CLOCK_REALTIME_ALARM,
    BootTimeAlarm = CLOCK_BOOTTIME_ALARM,
};

template <ClockId Id>
class Clock
{
  public:
    using rep = SdEventDuration::rep;
    using period = SdEventDuration::period;
    using duration = SdEventDuration;
    using time_point = std::chrono::time_point<Clock>;
    static constexpr bool is_steady = Id == ClockId::Monotonic;

    Clock(const Event& event);
    Clock(Event&& event);

    time_point now() const;

  private:
    const Event event;
};

} // namespace sdeventplus
