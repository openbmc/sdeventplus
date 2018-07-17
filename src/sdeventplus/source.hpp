#pragma once

#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <type_traits>

namespace sdeventplus
{

class Source
{
  public:
    Source(sd_event_source* source, SdEventInterface* intf = &sdevent_impl);
    Source(sd_event_source* source, std::false_type,
           SdEventInterface* intf = &sdevent_impl);

  private:
    SdEventInterface* intf;
    SdRef<sd_event_source> source;
};

} // namespace sdeventplus
