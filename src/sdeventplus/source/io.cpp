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
    Base(event, create_source(event, fd, events), std::false_type())
{
    set_userdata(std::make_unique<IO>(*this, std::true_type()));
    set_callback(std::move(callback));
}

IO::IO(const IO& other, std::true_type) : Base(other, std::true_type())
{
}

void IO::set_callback(Callback&& callback)
{
    get_userdata().callback = std::move(callback);
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

IO& IO::get_userdata() const
{
    return *reinterpret_cast<IO*>(
        event.getSdEvent()->sd_event_source_get_userdata(get()));
}

IO::Callback& IO::get_callback()
{
    return get_userdata().callback;
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
