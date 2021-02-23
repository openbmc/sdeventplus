#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/internal/cexec.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <systemd/sd-event.h>
#include <type_traits>
#include <utility>

namespace sdeventplus
{

Event::Event(sd_event* event, const internal::SdEvent* sdevent) :
    sdevent(sdevent), event(event, sdevent, true)
{
}

Event::Event(sd_event* event, std::false_type,
             const internal::SdEvent* sdevent) :
    sdevent(sdevent),
    event(std::move(event), sdevent, true)
{
}

Event::Event(const Event& other, sdeventplus::internal::NoOwn) :
    sdevent(other.sdevent), event(other.get(), other.getSdEvent(), false)
{
}

Event Event::get_new(const internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    SDEVENTPLUS_CHECK("sd_event_new", sdevent->sd_event_new(&event));
    return Event(event, std::false_type(), sdevent);
}

Event Event::get_default(const internal::SdEvent* sdevent)
{
    sd_event* event = nullptr;
    SDEVENTPLUS_CHECK("sd_event_default", sdevent->sd_event_default(&event));
    return Event(event, std::false_type(), sdevent);
}

sd_event* Event::get() const
{
    return event.value();
}

const internal::SdEvent* Event::getSdEvent() const
{
    return sdevent;
}

int Event::prepare() const
{
    return SDEVENTPLUS_CHECK("sd_event_prepare",
                             sdevent->sd_event_prepare(get()));
}

int Event::wait(MaybeTimeout timeout) const
{
    // An unsigned -1 timeout value means infinity in sd_event
    uint64_t timeout_usec = timeout ? timeout->count() : -1;
    return SDEVENTPLUS_CHECK("sd_event_wait",
                             sdevent->sd_event_wait(get(), timeout_usec));
}

int Event::dispatch() const
{
    return SDEVENTPLUS_CHECK("sd_event_dispatch",
                             sdevent->sd_event_dispatch(get()));
}

int Event::run(MaybeTimeout timeout) const
{
    // An unsigned -1 timeout value means infinity in sd_event
    uint64_t timeout_usec = timeout ? timeout->count() : -1;
    return SDEVENTPLUS_CHECK("sd_event_run",
                             sdevent->sd_event_run(get(), timeout_usec));
}

int Event::loop() const
{
    return SDEVENTPLUS_CHECK("sd_event_loop", sdevent->sd_event_loop(get()));
}

void Event::exit(int code) const
{
    SDEVENTPLUS_CHECK("sd_event_exit", sdevent->sd_event_exit(get(), code));
}

int Event::get_exit_code() const
{
    int code;
    SDEVENTPLUS_CHECK("sd_event_get_exit_code",
                      sdevent->sd_event_get_exit_code(get(), &code));
    return code;
}

bool Event::get_watchdog() const
{
    return SDEVENTPLUS_CHECK("sd_event_get_watchdog",
                             sdevent->sd_event_get_watchdog(get()));
}

bool Event::set_watchdog(bool b) const
{
    return SDEVENTPLUS_CHECK("sd_event_set_watchdog",
                             sdevent->sd_event_set_watchdog(get(), b));
}

sd_event* Event::ref(sd_event* const& event, const internal::SdEvent*& sdevent,
                     bool& owned)
{
    owned = true;
    return sdevent->sd_event_ref(event);
}

void Event::drop(sd_event*&& event, const internal::SdEvent*& sdevent,
                 bool& owned)
{
    if (owned)
    {
        sdevent->sd_event_unref(event);
    }
}

} // namespace sdeventplus
