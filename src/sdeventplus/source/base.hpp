#pragma once

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
    // We don't want to allow any kind of slicing.
    Base(const Base& source) = delete;
    Base& operator=(const Base& source) = delete;
    Base(Base&& source) = delete;
    Base& operator=(Base&& source) = delete;

  protected:
    internal::SdEvent* sdevent;
    internal::SdRef<sd_event_source> source;

    // Base sources cannot be directly constructed.
    Base(sd_event_source* source,
         internal::SdEvent* sdevent = &internal::sdevent_impl);
    Base(sd_event_source* source, std::false_type,
         internal::SdEvent* sdevent = &internal::sdevent_impl);
};

} // namespace source
} // namespace sdeventplus
