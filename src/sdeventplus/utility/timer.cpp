#include <functional>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <stdexcept>
#include <utility>

namespace sdeventplus
{
namespace utility
{

template <ClockId Id>
Timer<Id>::Timer(Timer&& other) :
    expired(std::move(other.expired)),
    initialized(std::move(other.initialized)),
    callback(std::move(other.callback)), clock(std::move(other.clock)),
    interval(std::move(other.interval)), timeSource(std::move(other.timeSource))
{
    timeSource.set_callback(std::bind(&Timer::internalCallback, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
}

template <ClockId Id>
Timer<Id>& Timer<Id>::operator=(Timer&& other)
{
    if (this != &other)
    {
        expired = std::move(other.expired);
        initialized = std::move(other.initialized);
        callback = std::move(other.callback);
        clock = std::move(other.clock);
        interval = std::move(other.interval);
        timeSource = std::move(other.timeSource);
        timeSource.set_callback(std::bind(&Timer::internalCallback, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
    }
    return *this;
}

template <ClockId Id>
Timer<Id>::Timer(const Event& event, Callback&& callback,
                 std::optional<Duration> interval,
                 typename source::Time<Id>::Accuracy accuracy) :
    expired(false),
    initialized(interval.has_value()), callback(callback), clock(event),
    interval(interval),
    timeSource(event, clock.now() + interval.value_or(Duration::zero()),
               accuracy,
               std::bind(&Timer::internalCallback, this, std::placeholders::_1,
                         std::placeholders::_2))
{
    setEnabled(interval.has_value());
}

template <ClockId Id>
void Timer<Id>::set_callback(Callback&& callback)
{
    this->callback = std::move(callback);
}

template <ClockId Id>
const Event& Timer<Id>::get_event() const
{
    return timeSource.get_event();
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
std::optional<typename Timer<Id>::Duration> Timer<Id>::getInterval() const
{
    return interval;
}

template <ClockId Id>
typename Timer<Id>::Duration Timer<Id>::getRemaining() const
{
    if (!isEnabled())
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
    if (enabled && !initialized)
    {
        throw std::runtime_error("Timer was never initialized");
    }
    timeSource.set_enabled(enabled ? source::Enabled::On
                                   : source::Enabled::Off);
}

template <ClockId Id>
void Timer<Id>::setRemaining(Duration remaining)
{
    timeSource.set_time(clock.now() + remaining);
    initialized = true;
}

template <ClockId Id>
void Timer<Id>::resetRemaining()
{
    setRemaining(interval.value());
}

template <ClockId Id>
void Timer<Id>::setInterval(std::optional<Duration> interval)
{
    this->interval = interval;
}

template <ClockId Id>
void Timer<Id>::clearExpired()
{
    expired = false;
}

template <ClockId Id>
void Timer<Id>::restart(std::optional<Duration> interval)
{
    clearExpired();
    initialized = false;
    setInterval(interval);
    if (interval)
    {
        resetRemaining();
    }
    setEnabled(interval.has_value());
}

template <ClockId Id>
void Timer<Id>::restartOnce(Duration remaining)
{
    clearExpired();
    initialized = false;
    setInterval(std::nullopt);
    setRemaining(remaining);
    setEnabled(true);
}

template <ClockId Id>
void Timer<Id>::internalCallback(source::Time<Id>&,
                                 typename source::Time<Id>::TimePoint)
{
    expired = true;
    initialized = false;
    if (interval)
    {
        resetRemaining();
    }
    else
    {
        setEnabled(false);
    }

    if (callback)
    {
        callback(*this);
    }
}

template class Timer<ClockId::RealTime>;
template class Timer<ClockId::Monotonic>;
template class Timer<ClockId::BootTime>;
template class Timer<ClockId::RealTimeAlarm>;
template class Timer<ClockId::BootTimeAlarm>;

} // namespace utility
} // namespace sdeventplus
