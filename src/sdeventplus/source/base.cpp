#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <type_traits>

namespace sdeventplus
{
namespace source
{

Base::Base(sd_event_source* source, internal::SdEvent* sdevent) :
    sdevent(sdevent), source(source, &internal::SdEvent::sd_event_source_ref,
                             &internal::SdEvent::sd_event_source_unref, sdevent)
{
}

Base::Base(sd_event_source* source, std::false_type,
           internal::SdEvent* sdevent) :
    sdevent(sdevent),
    source(source, &internal::SdEvent::sd_event_source_ref,
           &internal::SdEvent::sd_event_source_unref, std::false_type(),
           sdevent)
{
}

} // namespace source
} // namespace sdeventplus
