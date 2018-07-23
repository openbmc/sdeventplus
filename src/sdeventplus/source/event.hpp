#pragma once

#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace source
{

class EventBase : public Base
{
  public:
    using Callback = std::function<void(EventBase& source)>;

  protected:
    using CreateFunc = decltype(&internal::SdEvent::sd_event_add_exit);

    EventBase(const char* name, CreateFunc create, const Event& event,
              Callback&& callback);

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const char* name, CreateFunc create,
                                          const Event& event);

    static int eventCallback(sd_event_source* source, void* userdata);
};

class Defer : public EventBase
{
  public:
    Defer(const Event& event, Callback&& Callback);
};

class Post : public EventBase
{
  public:
    Post(const Event& event, Callback&& callback);
};

class Exit : public EventBase
{
  public:
    Exit(const Event& event, Callback&& callback);
};

} // namespace source
} // namespace sdeventplus
