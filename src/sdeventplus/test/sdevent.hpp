#pragma once

#include <gmock/gmock.h>
#include <sdeventplus/sdevent.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

class SdEventMock : public SdEventInterface
{
  public:
    MOCK_CONST_METHOD1(sd_event_default, int(sd_event **));
    MOCK_CONST_METHOD1(sd_event_new, int(sd_event **));
    MOCK_CONST_METHOD1(sd_event_ref, sd_event *(sd_event *));
    MOCK_CONST_METHOD1(sd_event_unref, sd_event *(sd_event *));

    MOCK_CONST_METHOD1(sd_event_loop, int(sd_event *));
};

} // namespace sdeventplus
