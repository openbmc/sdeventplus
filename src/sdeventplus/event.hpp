#pragma once

#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

class Event
{
  public:
    Event(sd_event* event,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);
    Event(sd_event* event, std::false_type,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);

    static Event
        get_new(const internal::SdEvent* sdevent = &internal::sdevent_impl);
    static Event
        get_default(const internal::SdEvent* sdevent = &internal::sdevent_impl);

    int loop();
    int get_watchdog();
    int set_watchdog(int b);

  private:
    const internal::SdEvent* sdevent;
    internal::SdRef<sd_event> event;
};

} // namespace sdeventplus
