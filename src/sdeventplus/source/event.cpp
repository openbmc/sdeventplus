#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/event.hpp>
#include <utility>

namespace sdeventplus
{
namespace source
{

Event::Event(const char* name, CreateFunc create,
             const sdeventplus::Event& event, Callback&& callback) :
    Base(event, create_source(name, create, event), std::false_type()),
    callback(std::move(callback))
{
}

const Event::Callback& Event::get_callback() const
{
    return callback;
}

sd_event_source* Event::create_source(const char* name, CreateFunc create,
                                      const sdeventplus::Event& event)
{
    sd_event_source* source;
    int r = (event.getSdEvent()->*create)(event.get(), &source, eventCallback,
                                          nullptr);
    if (r < 0)
    {
        throw SdEventError(-r, name);
    }
    return source;
}

int Event::eventCallback(sd_event_source* source, void* userdata)
{
    return sourceCallback<Callback, Event, &Event::get_callback>(
        "eventCallback", source, userdata);
}

Defer::Defer(const sdeventplus::Event& event, Callback&& callback) :
    Event("sd_event_add_defer", &internal::SdEvent::sd_event_add_defer, event,
          std::move(callback))
{
}

Post::Post(const sdeventplus::Event& event, Callback&& callback) :
    Event("sd_event_add_post", &internal::SdEvent::sd_event_add_post, event,
          std::move(callback))
{
}

Exit::Exit(const sdeventplus::Event& event, Callback&& callback) :
    Event("sd_event_add_exit", &internal::SdEvent::sd_event_add_exit, event,
          std::move(callback))
{
}

} // namespace source
} // namespace sdeventplus
