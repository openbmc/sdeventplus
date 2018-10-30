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
    int r = event.getSdEvent()->sd_event_source_get_io_fd(source.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_io_fd");
    }
    return r;
}

void IO::set_fd(int fd) const
{
    int r = event.getSdEvent()->sd_event_source_set_io_fd(source.get(), fd);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_io_fd");
    }
}

uint32_t IO::get_events() const
{
    uint32_t events;
    int r = event.getSdEvent()->sd_event_source_get_io_events(source.get(),
                                                              &events);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_io_events");
    }
    return events;
}

void IO::set_events(uint32_t events) const
{
    int r =
        event.getSdEvent()->sd_event_source_set_io_events(source.get(), events);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_set_io_events");
    }
}

uint32_t IO::get_revents() const
{
    uint32_t revents;
    int r = event.getSdEvent()->sd_event_source_get_io_revents(source.get(),
                                                               &revents);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_io_revents");
    }
    return revents;
}

const IO::Callback& IO::get_callback() const
{
    return callback;
}

sd_event_source* IO::create_source(const Event& event, int fd, uint32_t events)
{
    sd_event_source* source;
    int r = event.getSdEvent()->sd_event_add_io(event.get(), &source, fd,
                                                events, ioCallback, nullptr);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_add_io");
    }
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
