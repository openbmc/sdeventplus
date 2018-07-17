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

    virtual int
        sd_event_source_get_description(sd_event_source* source,
                                        const char** description) const = 0;
    virtual int
        sd_event_source_set_description(sd_event_source* source,
                                        const char* description) const = 0;
    virtual int
        sd_event_source_set_prepare(sd_event_source* source,
                                    sd_event_handler_t callback) const = 0;
    virtual int sd_event_source_get_pending(sd_event_source* source) const = 0;
    virtual int sd_event_source_get_priority(sd_event_source* source,
                                             int64_t* priority) const = 0;
    virtual int sd_event_source_set_priority(sd_event_source* source,
                                             int64_t priority) const = 0;
    virtual int sd_event_source_get_enabled(sd_event_source* source,
                                            int* enabled) const = 0;
    virtual int sd_event_source_set_enabled(sd_event_source* source,
                                            int enabled) const = 0;
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

    int sd_event_source_get_description(sd_event_source* source,
                                        const char** description) const override
    {
        return ::sd_event_source_get_description(source, description);
    }

    int sd_event_source_set_description(sd_event_source* source,
                                        const char* description) const override
    {
        return ::sd_event_source_set_description(source, description);
    }

    int sd_event_source_set_prepare(sd_event_source* source,
                                    sd_event_handler_t callback) const override
    {
        return ::sd_event_source_set_prepare(source, callback);
    }

    int sd_event_source_get_pending(sd_event_source* source) const override
    {
        return ::sd_event_source_get_pending(source);
    }

    int sd_event_source_get_priority(sd_event_source* source,
                                     int64_t* priority) const override
    {
        return ::sd_event_source_get_priority(source, priority);
    }

    int sd_event_source_set_priority(sd_event_source* source,
                                     int64_t priority) const override
    {
        return ::sd_event_source_set_priority(source, priority);
    }

    int sd_event_source_get_enabled(sd_event_source* source,
                                    int* enabled) const override
    {
        return ::sd_event_source_get_enabled(source, enabled);
    }

    int sd_event_source_set_enabled(sd_event_source* source,
                                    int enabled) const override
    {
        return ::sd_event_source_set_enabled(source, enabled);
    }
};

extern SdEventImpl sdevent_impl;

} // namespace internal
} // namespace sdeventplus
