#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/io.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

IO::IO(const Event& event, int fd, uint32_t events, Callback&& callback) :
    Base(event, create_source(event, fd, events), std::false_type()),
    callback(std::move(callback))
{
}

void IO::set_callback(Callback&& callback)
{
    this->callback = std::move(callback);
}

int IO::get_fd() const
{
    return internal::callCheck("sd_event_source_get_io_fd",
                               &internal::SdEvent::sd_event_source_get_io_fd,
                               event.getSdEvent(), get());
}

void IO::set_fd(int fd) const
{
    internal::callCheck("sd_event_source_set_io_fd",
                        &internal::SdEvent::sd_event_source_set_io_fd,
                        event.getSdEvent(), get(), fd);
}

uint32_t IO::get_events() const
{
    uint32_t events;
    internal::callCheck("sd_event_source_get_io_events",
                        &internal::SdEvent::sd_event_source_get_io_events,
                        event.getSdEvent(), get(), &events);
    return events;
}

void IO::set_events(uint32_t events) const
{
    internal::callCheck("sd_event_source_set_io_events",
                        &internal::SdEvent::sd_event_source_set_io_events,
                        event.getSdEvent(), get(), events);
}

uint32_t IO::get_revents() const
{
    uint32_t revents;
    internal::callCheck("sd_event_source_get_io_revents",
                        &internal::SdEvent::sd_event_source_get_io_revents,
                        event.getSdEvent(), get(), &revents);
    return revents;
}

const IO::Callback& IO::get_callback() const
{
    return callback;
}

sd_event_source* IO::create_source(const Event& event, int fd, uint32_t events)
{
    sd_event_source* source;
    internal::callCheck("sd_event_add_io", &internal::SdEvent::sd_event_add_io,
                        event.getSdEvent(), event.get(), &source, fd, events,
                        ioCallback, nullptr);
    return source;
}

int IO::ioCallback(sd_event_source* source, int fd, uint32_t revents,
                   void* userdata)
{
    return sourceCallback<Callback, IO, &IO::get_callback>(
        "ioCallback", source, userdata, fd, revents);
}

} // namespace source
} // namespace sdeventplus
