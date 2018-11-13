#include <functional>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/sdevent.hpp>
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
    int r = event.getSdEvent()->sd_event_source_get_description(get(),
                                                                &description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_description");
    }
    return description;
}

void Base::set_description(const char* description) const
{
    int r =
        event.getSdEvent()->sd_event_source_set_description(get(), description);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_description");
    }
}

void Base::set_prepare(Callback&& callback)
{
    int r = event.getSdEvent()->sd_event_source_set_prepare(
        get(), callback ? prepareCallback : nullptr);
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

bool Base::get_pending() const
{
    int r = event.getSdEvent()->sd_event_source_get_pending(get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_pending");
    }
    return r;
}

int64_t Base::get_priority() const
{
    int64_t priority;
    int r = event.getSdEvent()->sd_event_source_get_priority(get(), &priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_priority");
    }
    return priority;
}

void Base::set_priority(int64_t priority) const
{
    int r = event.getSdEvent()->sd_event_source_set_priority(get(), priority);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_priority");
    }
}

Enabled Base::get_enabled() const
{
    int enabled;
    int r = event.getSdEvent()->sd_event_source_get_enabled(get(), &enabled);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_enabled");
    }
    return static_cast<Enabled>(enabled);
}

void Base::set_enabled(Enabled enabled) const
{
    int r = event.getSdEvent()->sd_event_source_set_enabled(
        get(), static_cast<int>(enabled));
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_enabled");
    }
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
