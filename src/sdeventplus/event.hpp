#pragma once

#include <memory>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

class Event
{
  public:
    Event(sd_event* event, SdEventInterface* intf = &sdevent_impl);
    Event(sd_event* event, std::false_type,
          SdEventInterface* intf = &sdevent_impl);
    static Event get_new(SdEventInterface* intf = &sdevent_impl);
    static Event get_default(SdEventInterface* intf = &sdevent_impl);

    int loop();

  private:
    SdEventInterface* intf;
    SdRef<sd_event> event;
};

} // namespace sdeventplus
