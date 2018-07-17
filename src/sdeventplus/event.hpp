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
          internal::SdEvent* sdevent = &internal::sdevent_impl);
    Event(sd_event* event, std::false_type,
          internal::SdEvent* sdevent = &internal::sdevent_impl);
    static Event get_new(internal::SdEvent* sdevent = &internal::sdevent_impl);
    static Event
        get_default(internal::SdEvent* sdevent = &internal::sdevent_impl);

    int loop();
    int get_watchdog();
    int set_watchdog(int b);

  private:
    internal::SdEvent* sdevent;
    internal::SdRef<sd_event> event;
};

} // namespace sdeventplus
