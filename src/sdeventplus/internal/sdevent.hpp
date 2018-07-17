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

    virtual int sd_event_add_time(sd_event* event, sd_event_source** source,
                                  clockid_t clock, uint64_t usec,
                                  uint64_t accuracy,
                                  sd_event_time_handler_t callback,
                                  void* userdata) const = 0;

    virtual int sd_event_prepare(sd_event* event) const = 0;
    virtual int sd_event_wait(sd_event* event, uint64_t usec) const = 0;
    virtual int sd_event_dispatch(sd_event* event) const = 0;
    virtual int sd_event_run(sd_event* event, uint64_t usec) const = 0;
    virtual int sd_event_loop(sd_event* event) const = 0;
    virtual int sd_event_exit(sd_event* event, int code) const = 0;

    virtual int sd_event_now(sd_event* event, clockid_t clock,
                             uint64_t* usec) const = 0;

    virtual int sd_event_get_exit_code(sd_event* event, int* code) const = 0;
    virtual int sd_event_get_watchdog(sd_event* event) const = 0;
    virtual int sd_event_set_watchdog(sd_event* event, int b) const = 0;

    virtual sd_event_source*
        sd_event_source_ref(sd_event_source* source) const = 0;
    virtual sd_event_source*
        sd_event_source_unref(sd_event_source* source) const = 0;

    virtual void* sd_event_source_set_userdata(sd_event_source* source,
                                               void* userdata) const = 0;

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
    virtual int sd_event_source_get_time(sd_event_source* source,
                                         uint64_t* usec) const = 0;
    virtual int sd_event_source_set_time(sd_event_source* source,
                                         uint64_t usec) const = 0;
    virtual int sd_event_source_get_time_accuracy(sd_event_source* source,
                                                  uint64_t* usec) const = 0;
    virtual int sd_event_source_set_time_accuracy(sd_event_source* source,
                                                  uint64_t usec) const = 0;
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

    int sd_event_add_time(sd_event* event, sd_event_source** source,
                          clockid_t clock, uint64_t usec, uint64_t accuracy,
                          sd_event_time_handler_t callback,
                          void* userdata) const override
    {
        return ::sd_event_add_time(event, source, clock, usec, accuracy,
                                   callback, userdata);
    }

    int sd_event_prepare(sd_event* event) const override
    {
        return ::sd_event_prepare(event);
    }

    int sd_event_wait(sd_event* event, uint64_t usec) const override
    {
        return ::sd_event_wait(event, usec);
    }

    int sd_event_dispatch(sd_event* event) const override
    {
        return ::sd_event_dispatch(event);
    }

    int sd_event_run(sd_event* event, uint64_t usec) const override
    {
        return ::sd_event_run(event, usec);
    }

    int sd_event_loop(sd_event* event) const override
    {
        return ::sd_event_loop(event);
    }

    int sd_event_exit(sd_event* event, int code) const override
    {
        return ::sd_event_exit(event, code);
    }

    int sd_event_now(sd_event* event, clockid_t clock,
                     uint64_t* usec) const override
    {
        return ::sd_event_now(event, clock, usec);
    }

    int sd_event_get_exit_code(sd_event* event, int* code) const override
    {
        return ::sd_event_get_exit_code(event, code);
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

    void* sd_event_source_set_userdata(sd_event_source* source,
                                       void* userdata) const override
    {
        return ::sd_event_source_set_userdata(source, userdata);
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

    int sd_event_source_get_time(sd_event_source* source,
                                 uint64_t* usec) const override
    {
        return ::sd_event_source_get_time(source, usec);
    }

    int sd_event_source_set_time(sd_event_source* source,
                                 uint64_t usec) const override
    {
        return ::sd_event_source_set_time(source, usec);
    }

    int sd_event_source_get_time_accuracy(sd_event_source* source,
                                          uint64_t* usec) const override
    {
        return ::sd_event_source_get_time_accuracy(source, usec);
    }

    int sd_event_source_set_time_accuracy(sd_event_source* source,
                                          uint64_t usec) const override
    {
        return ::sd_event_source_set_time_accuracy(source, usec);
    }
};

extern SdEventImpl sdevent_impl;

} // namespace internal
} // namespace sdeventplus
