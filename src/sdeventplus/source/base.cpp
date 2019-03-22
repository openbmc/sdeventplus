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

Base::~Base()
{
    if (source)
    {
        set_enabled(Enabled::Off);
    }
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
        prepare = std::move(callback);
    }
    catch (...)
    {
        prepare = nullptr;
        throw;
    }
}

const Base::Callback& Base::get_prepare() const
{
    return prepare;
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
    event(event), source(std::move(source), event.getSdEvent())
{
    set_userdata();
}

Base::Base(Base&& other) :
    event(std::move(other.event)), source(std::move(other.source)),
    prepare(std::move(other.prepare))
{
    set_userdata();
}

Base& Base::operator=(Base&& other)
{
    if (this != &other)
    {
        // We need to make sure our current event is not triggered
        // after it gets deleted in the move
        if (source)
        {
            set_enabled(Enabled::Off);
        }

        event = std::move(other.event);
        source = std::move(other.source);
        prepare = std::move(other.prepare);

        set_userdata();
    }
    return *this;
}

void Base::drop(sd_event_source*&& source, const internal::SdEvent*& sdevent)
{
    sdevent->sd_event_source_unref(source);
}

int Base::prepareCallback(sd_event_source* source, void* userdata)
{
    return sourceCallback<Callback, Base, &Base::get_prepare>("prepareCallback",
                                                              source, userdata);
}

void Base::set_userdata()
{
    event.getSdEvent()->sd_event_source_set_userdata(get(), this);
}

} // namespace source
} // namespace sdeventplus
