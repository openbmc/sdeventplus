#include <cerrno>
#include <exception>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

Base::~Base()
{
    set_enabled(SD_EVENT_OFF);
}

const char* Base::get_description()
{
    const char* description;
    int r =
        sdevent->sd_event_source_get_description(source.get(), &description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_description");
    }
    return description;
}

void Base::set_description(const char* description)
{
    int r = sdevent->sd_event_source_set_description(source.get(), description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_description");
    }
}

int Base::get_pending()
{
    int r = sdevent->sd_event_source_get_pending(source.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_pending");
    }
    return r;
}

int64_t Base::get_priority()
{
    int64_t priority;
    int r = sdevent->sd_event_source_get_priority(source.get(), &priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_priority");
    }
    return priority;
}

void Base::set_priority(int64_t priority)
{
    int r = sdevent->sd_event_source_set_priority(source.get(), priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_priority");
    }
}

int Base::get_enabled()
{
    int enabled;
    int r = sdevent->sd_event_source_get_enabled(source.get(), &enabled);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_enabled");
    }
    return enabled;
}

void Base::set_enabled(int enabled)
{
    int r = sdevent->sd_event_source_set_enabled(source.get(), enabled);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_enabled");
    }
}

Base::Base(sd_event_source* source, internal::SdEvent* sdevent) :
    sdevent(sdevent), source(source, &internal::SdEvent::sd_event_source_ref,
                             &internal::SdEvent::sd_event_source_unref, sdevent)
{
}

Base::Base(sd_event_source* source, std::false_type,
           internal::SdEvent* sdevent) :
    sdevent(sdevent),
    source(source, &internal::SdEvent::sd_event_source_ref,
           &internal::SdEvent::sd_event_source_unref, std::false_type(),
           sdevent)
{
}

} // namespace source
} // namespace sdeventplus
