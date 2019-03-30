#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/base.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

Base::Base(const Base& other) : event(other.event), source(other.source)
{
}

Base& Base::operator=(const Base& other)
{
    if (this != &other)
    {
        event = other.event;
        source = other.source;
    }
    return *this;
}

sd_event_source* Base::get() const
{
    return source.value();
}

const Event& Base::get_event() const
{
    return event;
}

const char* Base::get_description() const
{
    const char* description;
    internal::callCheck("sd_event_source_get_description",
                        &internal::SdEvent::sd_event_source_get_description,
                        event.getSdEvent(), get(), &description);
    return description;
}

void Base::set_description(const char* description) const
{
    internal::callCheck("sd_event_source_set_description",
                        &internal::SdEvent::sd_event_source_set_description,
                        event.getSdEvent(), get(), description);
}

void Base::set_prepare(Callback&& callback)
{
    try
    {
        internal::callCheck("sd_event_source_set_prepare",
                            &internal::SdEvent::sd_event_source_set_prepare,
                            event.getSdEvent(), get(),
                            callback ? prepareCallback : nullptr);
        get_userdata().prepare = std::move(callback);
    }
    catch (...)
    {
        get_userdata().prepare = nullptr;
        throw;
    }
}

bool Base::get_pending() const
{
    return internal::callCheck("sd_event_source_get_pending",
                               &internal::SdEvent::sd_event_source_get_pending,
                               event.getSdEvent(), get());
}

int64_t Base::get_priority() const
{
    int64_t priority;
    internal::callCheck("sd_event_source_get_priority",
                        &internal::SdEvent::sd_event_source_get_priority,
                        event.getSdEvent(), get(), &priority);
    return priority;
}

void Base::set_priority(int64_t priority) const
{
    internal::callCheck("sd_event_source_set_priority",
                        &internal::SdEvent::sd_event_source_set_priority,
                        event.getSdEvent(), get(), priority);
}

Enabled Base::get_enabled() const
{
    int enabled;
    internal::callCheck("sd_event_source_get_enabled",
                        &internal::SdEvent::sd_event_source_get_enabled,
                        event.getSdEvent(), get(), &enabled);
    return static_cast<Enabled>(enabled);
}

void Base::set_enabled(Enabled enabled) const
{
    internal::callCheck("sd_event_source_set_enabled",
                        &internal::SdEvent::sd_event_source_set_enabled,
                        event.getSdEvent(), get(), static_cast<int>(enabled));
}

Base::Base(const Event& event, sd_event_source* source, std::false_type) :
    event(event), source(std::move(source), event.getSdEvent(), true)
{
}

Base::Base(const Base& other, std::true_type) :
    event(other.get_event(), std::true_type()),
    source(other.get(), event.getSdEvent(), false)
{
}

void Base::set_userdata(std::unique_ptr<Base> data) const
{
    internal::callCheck(
        "sd_event_source_set_destroy_callback",
        &internal::SdEvent::sd_event_source_set_destroy_callback,
        event.getSdEvent(), get(), &Base::destroy_userdata);
    event.getSdEvent()->sd_event_source_set_userdata(get(), data.release());
}

Base::Callback& Base::get_prepare()
{
    return get_userdata().prepare;
}

sd_event_source* Base::ref(sd_event_source* const& source,
                           const internal::SdEvent*& sdevent, bool& owned)
{
    owned = true;
    return sdevent->sd_event_source_ref(source);
}

void Base::drop(sd_event_source*&& source, const internal::SdEvent*& sdevent,
                bool& owned)
{
    if (owned)
    {
        sdevent->sd_event_source_unref(source);
    }
}

Base& Base::get_userdata() const
{
    return *reinterpret_cast<Base*>(
        event.getSdEvent()->sd_event_source_get_userdata(get()));
}

void Base::destroy_userdata(void* userdata)
{
    delete reinterpret_cast<Base*>(userdata);
}

int Base::prepareCallback(sd_event_source* source, void* userdata)
{
    return sourceCallback<Callback, Base, &Base::get_prepare>("prepareCallback",
                                                              source, userdata);
}

} // namespace source
} // namespace sdeventplus
