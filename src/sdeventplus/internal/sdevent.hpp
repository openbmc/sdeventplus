#pragma once

#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace internal
{

class SdEvent
{
  public:
    virtual ~SdEvent() = default;

    virtual int sd_event_default(sd_event** event) const = 0;
    virtual int sd_event_new(sd_event** event) const = 0;
    virtual sd_event* sd_event_ref(sd_event* event) const = 0;
    virtual sd_event* sd_event_unref(sd_event* event) const = 0;

    virtual int sd_event_loop(sd_event* event) const = 0;
    virtual int sd_event_get_watchdog(sd_event* event) const = 0;
    virtual int sd_event_set_watchdog(sd_event* event, int b) const = 0;

    virtual sd_event_source*
        sd_event_source_ref(sd_event_source* source) const = 0;
    virtual sd_event_source*
        sd_event_source_unref(sd_event_source* source) const = 0;
};

class SdEventImpl : public SdEvent
{
  public:
    int sd_event_default(sd_event** event) const override
    {
        return ::sd_event_default(event);
    }

    int sd_event_new(sd_event** event) const override
    {
        return ::sd_event_default(event);
    }

    sd_event* sd_event_ref(sd_event* event) const override
    {
        return ::sd_event_ref(event);
    }

    sd_event* sd_event_unref(sd_event* event) const override
    {
        return ::sd_event_unref(event);
    }

    int sd_event_loop(sd_event* event) const override
    {
        return ::sd_event_loop(event);
    }

    int sd_event_get_watchdog(sd_event* event) const override
    {
        return ::sd_event_get_watchdog(event);
    }

    int sd_event_set_watchdog(sd_event* event, int b) const override
    {
        return ::sd_event_set_watchdog(event, b);
    }

    sd_event_source* sd_event_source_ref(sd_event_source* source) const override
    {
        return ::sd_event_source_ref(source);
    }
    sd_event_source*
        sd_event_source_unref(sd_event_source* source) const override
    {
        return ::sd_event_source_unref(source);
    }
};

extern SdEventImpl sdevent_impl;

} // namespace internal
} // namespace sdeventplus
