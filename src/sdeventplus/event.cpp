#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

Event::Event(sd_event* event, SdEventInterface* intf) :
    intf(intf), event(event, &SdEventInterface::sd_event_ref,
                      &SdEventInterface::sd_event_unref, intf)
{
}

Event::Event(sd_event* event, std::false_type, SdEventInterface* intf) :
    intf(intf),
    event(event, &SdEventInterface::sd_event_ref,
          &SdEventInterface::sd_event_unref, std::false_type(), intf)
{
}

Event Event::get_new(SdEventInterface* intf)
{
    sd_event* event = nullptr;
    int r = intf->sd_event_new(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_new");
    }
    return Event(event, std::false_type(), intf);
}

Event Event::get_default(SdEventInterface* intf)
{
    sd_event* event = nullptr;
    int r = intf->sd_event_default(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_default");
    }
    return Event(event, std::false_type(), intf);
}

int Event::loop()
{
    int r = intf->sd_event_loop(event.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_loop");
    }
    return r;
}

int Event::get_watchdog()
{
    int r = intf->sd_event_get_watchdog(event.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_get_watchdog");
    }
    return r;
}

int Event::set_watchdog(int b)
{
    int r = intf->sd_event_set_watchdog(event.get(), b);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_set_watchdog");
    }
    return r;
}

} // namespace sdeventplus
