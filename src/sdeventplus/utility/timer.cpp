#include <functional>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <stdexcept>

namespace sdeventplus
{
namespace utility
{

template <ClockId Id>
Timer<Id>::Timer(const Event& event, Callback&& callback, Duration interval,
                 Duration accuracy) :
    expired(false),
    callback(callback), clock(event), interval(interval),
    timeSource(event, clock.now() + interval, accuracy,
               std::bind(&Timer::internalCallback, this, std::placeholders::_1,
                         std::placeholders::_2))
{
    timeSource.set_enabled(source::Enabled::On);
}

template <ClockId Id>
bool Timer<Id>::hasExpired() const
{
    return expired;
}

template <ClockId Id>
bool Timer<Id>::isEnabled() const
{
    return timeSource.get_enabled() != source::Enabled::Off;
}

template <ClockId Id>
typename Timer<Id>::Duration Timer<Id>::getInterval() const
{
    return interval;
}

template <ClockId Id>
typename Timer<Id>::Duration Timer<Id>::getRemaining() const
{
    if (timeSource.get_enabled() == source::Enabled::Off)
    {
        throw std::runtime_error("Timer not running");
    }

    auto end = timeSource.get_time();
    auto now = clock.now();
    if (end < now)
    {
        return Duration{0};
    }
    return end - now;
}

template <ClockId Id>
void Timer<Id>::setEnabled(bool enabled)
{
    timeSource.set_enabled(enabled ? source::Enabled::On
                                   : source::Enabled::Off);
}

template <ClockId Id>
void Timer<Id>::setRemaining(Duration remaining)
{
    timeSource.set_time(clock.now() + remaining);
}

template <ClockId Id>
void Timer<Id>::resetRemaining()
{
    setRemaining(interval);
}

template <ClockId Id>
void Timer<Id>::setInterval(Duration interval)
{
    this->interval = interval;
}

template <ClockId Id>
void Timer<Id>::clearExpired()
{
    expired = false;
}

template <ClockId Id>
void Timer<Id>::restart(Duration interval)
{
    clearExpired();
    setInterval(interval);
    resetRemaining();
    setEnabled(true);
}

template <ClockId Id>
void Timer<Id>::internalCallback(source::Time<Id>&,
                                 typename source::Time<Id>::TimePoint)
{
    expired = true;
    if (callback)
    {
        callback();
    }
    resetRemaining();
}

template class Timer<ClockId::RealTime>;
template class Timer<ClockId::Monotonic>;
template class Timer<ClockId::BootTime>;
template class Timer<ClockId::RealTimeAlarm>;
template class Timer<ClockId::BootTimeAlarm>;

} // namespace utility
} // namespace sdeventplus
