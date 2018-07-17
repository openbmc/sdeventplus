#pragma once

#include <gmock/gmock.h>
#include <sdeventplus/internal/sdevent.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace test
{

class SdEventMock : public internal::SdEvent
{
  public:
    MOCK_CONST_METHOD1(sd_event_default, int(sd_event**));
    MOCK_CONST_METHOD1(sd_event_new, int(sd_event**));
    MOCK_CONST_METHOD1(sd_event_ref, sd_event*(sd_event*));
    MOCK_CONST_METHOD1(sd_event_unref, sd_event*(sd_event*));

    MOCK_CONST_METHOD1(sd_event_loop, int(sd_event*));
    MOCK_CONST_METHOD1(sd_event_get_watchdog, int(sd_event*));
    MOCK_CONST_METHOD2(sd_event_set_watchdog, int(sd_event*, int b));

    MOCK_CONST_METHOD1(sd_event_source_ref, sd_event_source*(sd_event_source*));
    MOCK_CONST_METHOD1(sd_event_source_unref,
                       sd_event_source*(sd_event_source*));
};

} // namespace test
} // namespace sdeventplus
