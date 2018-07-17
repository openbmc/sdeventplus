#pragma once

#include <experimental/optional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

class Event
{
  public:
    using Timeout = SdEventDuration;
    using MaybeTimeout = std::experimental::optional<Timeout>;

    Event(sd_event* event,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);
    Event(sd_event* event, std::false_type,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);

    static Event
        get_new(const internal::SdEvent* sdevent = &internal::sdevent_impl);
    static Event
        get_default(const internal::SdEvent* sdevent = &internal::sdevent_impl);

    sd_event* get() const;
    const internal::SdEvent* getSdEvent() const;

    int prepare() const;
    int wait(MaybeTimeout timeout) const;
    int dispatch() const;
    int run(MaybeTimeout timeout) const;
    int loop() const;
    int exit(int code) const;

    int get_exit_code() const;
    int get_watchdog() const;
    int set_watchdog(int b) const;

  private:
    const internal::SdEvent* sdevent;
    internal::SdRef<sd_event> event;
};

} // namespace sdeventplus
