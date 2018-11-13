#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <systemd/sd-event.h>
#include <type_traits>

namespace sdeventplus
{

Event::Event(sd_event* event, const internal::SdEvent* sdevent) :
    sdevent(sdevent), event(event, &internal::SdEvent::sd_event_ref,
                            &internal::SdEvent::sd_event_unref, sdevent)
{
}

Event::Event(sd_event* event, std::false_type,
             const internal::SdEvent* sdevent) :
    sdevent(sdevent),
    event(event, &internal::SdEvent::sd_event_ref,
          &internal::SdEvent::sd_event_unref, std::false_type(), sdevent)
{
}

Event Event::get_new(const internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    int r = sdevent->sd_event_new(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_new");
    }
    return Event(event, std::false_type(), sdevent);
}

Event Event::get_default(const internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    int r = sdevent->sd_event_default(&event);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_default");
    }
    return Event(event, std::false_type(), sdevent);
}

sd_event* Event::get() const
{
    return event.get();
}

const internal::SdEvent* Event::getSdEvent() const
{
    return sdevent;
}

int Event::prepare() const
{
    int r = sdevent->sd_event_prepare(get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_prepare");
    }
    return r;
}

int Event::wait(MaybeTimeout timeout) const
{
    // An unsigned -1 timeout value means infinity in sd_event
    uint64_t timeout_usec = timeout ? timeout->count() : -1;
    int r = sdevent->sd_event_wait(get(), timeout_usec);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_wait");
    }
    return r;
}

int Event::dispatch() const
{
    int r = sdevent->sd_event_dispatch(get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_dispatch");
    }
    return r;
}

int Event::run(MaybeTimeout timeout) const
{
    // An unsigned -1 timeout value means infinity in sd_event
    uint64_t timeout_usec = timeout ? timeout->count() : -1;
    int r = sdevent->sd_event_run(get(), timeout_usec);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_run");
    }
    return r;
}

int Event::loop() const
{
    int r = sdevent->sd_event_loop(get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_loop");
    }
    return r;
}

void Event::exit(int code) const
{
    int r = sdevent->sd_event_exit(get(), code);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_exit");
    }
}

int Event::get_exit_code() const
{
    int code;
    int r = sdevent->sd_event_get_exit_code(get(), &code);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_get_exit_code");
    }
    return code;
}

bool Event::get_watchdog() const
{
    int r = sdevent->sd_event_get_watchdog(get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_get_watchdog");
    }
    return r;
}

bool Event::set_watchdog(bool b) const
{
    int r = sdevent->sd_event_set_watchdog(get(), b);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_set_watchdog");
    }
    return r;
}

} // namespace sdeventplus
