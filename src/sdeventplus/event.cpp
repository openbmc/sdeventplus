#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

Event::Event(sd_event* event, internal::SdEvent* sdevent) :
    sdevent(sdevent), event(event, &internal::SdEvent::sd_event_ref,
                            &internal::SdEvent::sd_event_unref, sdevent)
{
}

Event::Event(sd_event* event, std::false_type, internal::SdEvent* sdevent) :
    sdevent(sdevent),
    event(event, &internal::SdEvent::sd_event_ref,
          &internal::SdEvent::sd_event_unref, std::false_type(), sdevent)
{
}

Event Event::get_new(internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    int r = sdevent->sd_event_new(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_new");
    }
    return Event(event, std::false_type(), sdevent);
}

Event Event::get_default(internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    int r = sdevent->sd_event_default(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_default");
    }
    return Event(event, std::false_type(), sdevent);
}

int Event::loop()
{
    int r = sdevent->sd_event_loop(event.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_loop");
    }
    return r;
}

int Event::get_watchdog()
{
    int r = sdevent->sd_event_get_watchdog(event.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_get_watchdog");
    }
    return r;
}

int Event::set_watchdog(int b)
{
    int r = sdevent->sd_event_set_watchdog(event.get(), b);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_set_watchdog");
    }
    return r;
}

} // namespace sdeventplus
