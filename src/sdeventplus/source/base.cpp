#include <cerrno>
#include <cstdio>
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
    if (source)
    {
        set_enabled(SD_EVENT_OFF);
    }
}

int Base::prepareCallback()
{
    try
    {
        prepare(*this);
        return 0;
    }
    catch (const std::system_error& e)
    {
        fprintf(stderr, "sdeventplus: prepareCallback: %s\n", e.what());
        return -e.code().value();
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "sdeventplus: prepareCallback: %s\n", e.what());
        return -ENOSYS;
    }
    catch (...)
    {
        fprintf(stderr, "sdeventplus: prepareCallback: Unknown error\n");
        return -ENOSYS;
    }
}

sd_event_source* Base::get() const
{
    return source.get();
}

const Event& Base::get_event() const
{
    return event;
}

const char* Base::get_description() const
{
    const char* description;
    int r = event.getSdEvent()->sd_event_source_get_description(source.get(),
                                                                &description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_description");
    }
    return description;
}

void Base::set_description(const char* description) const
{
    int r = event.getSdEvent()->sd_event_source_set_description(source.get(),
                                                                description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_description");
    }
}

static int prepare_callback(sd_event_source*, void* userdata)
{
    if (userdata == nullptr)
    {
        fprintf(stderr, "sdeventplus: prepare_callback: Missing userdata\n");
        return -EINVAL;
    }
    return reinterpret_cast<Base*>(userdata)->prepareCallback();
}

void Base::set_prepare(Callback&& callback)
{
    int r = event.getSdEvent()->sd_event_source_set_prepare(
        source.get(), callback ? prepare_callback : nullptr);
    if (r < 0)
    {
        prepare = nullptr;
        throw SdEventError(-r, "sd_event_source_set_prepare");
    }
    prepare = std::move(callback);
}

const Base::Callback& Base::get_prepare() const
{
    return prepare;
}

int Base::get_pending() const
{
    int r = event.getSdEvent()->sd_event_source_get_pending(source.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_pending");
    }
    return r;
}

int64_t Base::get_priority() const
{
    int64_t priority;
    int r = event.getSdEvent()->sd_event_source_get_priority(source.get(),
                                                             &priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_priority");
    }
    return priority;
}

void Base::set_priority(int64_t priority) const
{
    int r = event.getSdEvent()->sd_event_source_set_priority(source.get(),
                                                             priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_priority");
    }
}

int Base::get_enabled() const
{
    int enabled;
    int r =
        event.getSdEvent()->sd_event_source_get_enabled(source.get(), &enabled);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_enabled");
    }
    return enabled;
}

void Base::set_enabled(int enabled) const
{
    int r =
        event.getSdEvent()->sd_event_source_set_enabled(source.get(), enabled);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_enabled");
    }
}

Base::Base(const Event& event, sd_event_source* source) :
    event(event),
    source(source, &internal::SdEvent::sd_event_source_ref,
           &internal::SdEvent::sd_event_source_unref, event.getSdEvent())
{
}

Base::Base(const Event& event, sd_event_source* source, std::false_type) :
    event(event), source(source, &internal::SdEvent::sd_event_source_ref,
                         &internal::SdEvent::sd_event_source_unref,
                         std::false_type(), event.getSdEvent())
{
}

} // namespace source
} // namespace sdeventplus
