#pragma once

#include <cstdint>
#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
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

    // We don't want to allow any kind of slicing.
    Base(const Base& source) = delete;
    Base& operator=(const Base& source) = delete;
    Base(Base&& source) = delete;
    Base& operator=(Base&& source) = delete;

    int prepareCallback();

    const char* get_description() const;
    void set_description(const char* description) const;
    void set_prepare(Callback&& callback);
    int get_pending() const;
    int64_t get_priority() const;
    void set_priority(int64_t priority) const;
    int get_enabled() const;
    void set_enabled(int enabled) const;

  protected:
    const internal::SdEvent* const sdevent;
    const internal::SdRef<sd_event_source> source;

    // Base sources cannot be directly constructed.
    Base(sd_event_source* source,
         internal::SdEvent* sdevent = &internal::sdevent_impl);
    Base(sd_event_source* source, std::false_type,
         internal::SdEvent* sdevent = &internal::sdevent_impl);

  private:
    Callback prepare;
};

} // namespace source
} // namespace sdeventplus
