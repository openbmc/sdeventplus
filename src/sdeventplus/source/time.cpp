#include <cstdio>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/time.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

template <ClockId Id>
Time<Id>::Time(const Event& event, TimePoint time, Accuracy accuracy,
               Callback&& callback) :
    Base(event, create_source(event, time, accuracy), std::false_type()),
    callback(std::move(callback))
{
}

template <ClockId Id>
typename Time<Id>::TimePoint Time<Id>::get_time() const
{
    uint64_t usec;
    int r = event.getSdEvent()->sd_event_source_get_time(source.get(), &usec);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_time");
    }
    return Time<Id>::TimePoint(SdEventDuration(usec));
}

template <ClockId Id>
void Time<Id>::set_time(TimePoint time) const
{
    int r = event.getSdEvent()->sd_event_source_set_time(
        source.get(), SdEventDuration(time.time_since_epoch()).count());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_time");
    }
}

template <ClockId Id>
typename Time<Id>::Accuracy Time<Id>::get_accuracy() const
{
    uint64_t usec;
    int r = event.getSdEvent()->sd_event_source_get_time_accuracy(source.get(),
                                                                  &usec);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_time_accuracy");
    }
    return SdEventDuration(usec);
}

template <ClockId Id>
void Time<Id>::set_accuracy(Accuracy accuracy) const
{
    int r = event.getSdEvent()->sd_event_source_set_time_accuracy(
        source.get(), SdEventDuration(accuracy).count());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_time_accuracy");
    }
}

template <ClockId Id>
const typename Time<Id>::Callback& Time<Id>::get_callback() const
{
    return callback;
}

template <ClockId Id>
sd_event_source* Time<Id>::create_source(const Event& event, TimePoint time,
                                         Accuracy accuracy)
{
    sd_event_source* source;
    int r = event.getSdEvent()->sd_event_add_time(
        event.get(), &source, static_cast<clockid_t>(Id),
        SdEventDuration(time.time_since_epoch()).count(),
        SdEventDuration(accuracy).count(), timeCallback, nullptr);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_add_time");
    }
    return source;
}

template <ClockId Id>
int Time<Id>::timeCallback(sd_event_source* source, uint64_t usec,
                           void* userdata)
{
    return sourceCallback<Callback, Time, &Time::get_callback>(
        "timeCallback", source, userdata, TimePoint(SdEventDuration(usec)));
}

template class Time<ClockId::RealTime>;
template class Time<ClockId::Monotonic>;
template class Time<ClockId::BootTime>;
template class Time<ClockId::RealTimeAlarm>;
template class Time<ClockId::BootTimeAlarm>;

} // namespace source
} // namespace sdeventplus
