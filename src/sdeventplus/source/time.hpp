#pragma once

#include <cstdint>
#include <functional>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/base.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace source
{

template <ClockId Id>
class Time : public Base
{
  public:
    using TimePoint = typename Clock<Id>::time_point;
    using Accuracy = SdEventDuration;
    using Callback = std::function<void(Time& source, TimePoint time)>;

    Time(const Event& event, TimePoint time, Accuracy accuracy,
         Callback&& callback);

    TimePoint get_time() const;
    void set_time(TimePoint time) const;
    Accuracy get_accuracy() const;
    void set_accuracy(Accuracy accuracy) const;

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const Event& event, TimePoint time,
                                          Accuracy accuracy);
    static int timeCallback(sd_event_source* source, uint64_t usec,
                            void* userdata);
};

} // namespace source
} // namespace sdeventplus
