#include <sdeventplus/source.hpp>
#include <type_traits>

namespace sdeventplus
{

Source::Source(sd_event_source* source, SdEventInterface* intf) :
    intf(intf), source(source, &SdEventInterface::sd_event_source_ref,
                       &SdEventInterface::sd_event_source_unref, intf)
{
}

Source::Source(sd_event_source* source, std::false_type,
               SdEventInterface* intf) :
    intf(intf),
    source(source, &SdEventInterface::sd_event_source_ref,
           &SdEventInterface::sd_event_source_unref, std::false_type(), intf)
{
}

} // namespace sdeventplus
