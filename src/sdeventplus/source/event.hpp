#pragma once

#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace source
{

class Event : public Base
{
  public:
    using Callback = std::function<void(Event& source)>;

  protected:
    using CreateFunc = decltype(&internal::SdEvent::sd_event_add_exit);

    Event(const char* name, CreateFunc create, const sdeventplus::Event& event,
          Callback&& callback);

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const char* name, CreateFunc create,
                                          const sdeventplus::Event& event);

    static int eventCallback(sd_event_source* source, void* userdata);
};

class Defer : public Event
{
  public:
    Defer(const sdeventplus::Event& event, Callback&& Callback);
};

class Post : public Event
{
  public:
    Post(const sdeventplus::Event& event, Callback&& callback);
};

class Exit : public Event
{
  public:
    Exit(const sdeventplus::Event& event, Callback&& callback);
};

} // namespace source
} // namespace sdeventplus
