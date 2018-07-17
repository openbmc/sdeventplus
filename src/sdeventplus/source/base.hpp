#pragma once

#include <cstdint>
#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <systemd/sd-bus.h>
#include <type_traits>

namespace sdeventplus
{
namespace source
{

class Base
{
  public:
    using Callback = std::function<void(Base& source)>;

    virtual ~Base();

    int prepareCallback();

    sd_event_source* get() const;
    const Event& get_event() const;

    const char* get_description() const;
    void set_description(const char* description) const;
    void set_prepare(Callback&& callback);
    const Callback& get_prepare() const;
    int get_pending() const;
    int64_t get_priority() const;
    void set_priority(int64_t priority) const;
    int get_enabled() const;
    void set_enabled(int enabled) const;

  protected:
    Event event;
    internal::SdRef<sd_event_source> source;

    // Base sources cannot be directly constructed.
    Base(const Event& event, sd_event_source* source);
    Base(const Event& event, sd_event_source* source, std::false_type);

    // We can't ever copy an event_source because the callback
    // data has to be unique.
    Base(const Base& other) = delete;
    Base& operator=(const Base& other) = delete;
    // We don't want to allow any kind of slicing.
    Base(Base&& other) = default;
    Base& operator=(Base&& other);

  private:
    Callback prepare;
};

} // namespace source
} // namespace sdeventplus
