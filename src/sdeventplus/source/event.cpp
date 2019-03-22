#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/event.hpp>
#include <utility>

namespace sdeventplus
{
namespace source
{

void EventBase::set_callback(Callback&& callback)
{
    this->callback = std::move(callback);
}

EventBase::EventBase(const char* name, CreateFunc create, const Event& event,
                     Callback&& callback) :
    Base(event, create_source(name, create, event), std::false_type()),
    callback(std::move(callback))
{
}

const EventBase::Callback& EventBase::get_callback() const
{
    return callback;
}

sd_event_source* EventBase::create_source(const char* name, CreateFunc create,
                                          const Event& event)
{
    sd_event_source* source;
    internal::callCheck(name, create, event.getSdEvent(), event.get(), &source,
                        eventCallback, nullptr);
    return source;
}

int EventBase::eventCallback(sd_event_source* source, void* userdata)
{
    return sourceCallback<Callback, EventBase, &EventBase::get_callback>(
        "eventCallback", source, userdata);
}

Defer::Defer(const Event& event, Callback&& callback) :
    EventBase("sd_event_add_defer", &internal::SdEvent::sd_event_add_defer,
              event, std::move(callback))
{
}

Post::Post(const Event& event, Callback&& callback) :
    EventBase("sd_event_add_post", &internal::SdEvent::sd_event_add_post, event,
              std::move(callback))
{
}

Exit::Exit(const Event& event, Callback&& callback) :
    EventBase("sd_event_add_exit", &internal::SdEvent::sd_event_add_exit, event,
              std::move(callback))
{
}

} // namespace source
} // namespace sdeventplus
