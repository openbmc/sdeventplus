#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>

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

} // namespace sdeventplus
