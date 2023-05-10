#include <sdeventplus/internal/cexec.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/io.hpp>
#include <sdeventplus/types.hpp>

#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

IO::IO(const Event& event, int fd, uint32_t events, Callback&& callback) :
    Base(event, create_source(event, fd, events), std::false_type())
{
    set_userdata(std::make_unique<detail::IOData>(*this, std::move(callback)));
}

IO::IO(const IO& other, sdeventplus::internal::NoOwn) :
    Base(other, sdeventplus::internal::NoOwn())
{}

void IO::set_callback(Callback&& callback)
{
    get_userdata().callback = std::move(callback);
}

int IO::get_fd() const
{
    return SDEVENTPLUS_CHECK(
        "sd_event_source_get_io_fd",
        event.getSdEvent()->sd_event_source_get_io_fd(get()));
}

void IO::set_fd(int fd) const
{
    SDEVENTPLUS_CHECK("sd_event_source_set_io_fd",
                      event.getSdEvent()->sd_event_source_set_io_fd(get(), fd));
}

uint32_t IO::get_events() const
{
    uint32_t events;
    SDEVENTPLUS_CHECK(
        "sd_event_source_get_io_events",
        event.getSdEvent()->sd_event_source_get_io_events(get(), &events));
    return events;
}

void IO::set_events(uint32_t events) const
{
    SDEVENTPLUS_CHECK(
        "sd_event_source_set_io_events",
        event.getSdEvent()->sd_event_source_set_io_events(get(), events));
}

uint32_t IO::get_revents() const
{
    uint32_t revents;
    SDEVENTPLUS_CHECK(
        "sd_event_source_get_io_revents",
        event.getSdEvent()->sd_event_source_get_io_revents(get(), &revents));
    return revents;
}

detail::IOData& IO::get_userdata() const
{
    return static_cast<detail::IOData&>(Base::get_userdata());
}

IO::Callback& IO::get_callback()
{
    return get_userdata().callback;
}

sd_event_source* IO::create_source(const Event& event, int fd, uint32_t events)
{
    sd_event_source* source;
    SDEVENTPLUS_CHECK("sd_event_add_io", event.getSdEvent()->sd_event_add_io(
                                             event.get(), &source, fd, events,
                                             ioCallback, nullptr));
    return source;
}

int IO::ioCallback(sd_event_source* source, int fd, uint32_t revents,
                   void* userdata)
{
    return sourceCallback<Callback, detail::IOData, &IO::get_callback>(
        "ioCallback", source, userdata, fd, revents);
}

namespace detail
{

IOData::IOData(const IO& base, IO::Callback&& callback) :
    IO(base, sdeventplus::internal::NoOwn()), BaseData(base),
    callback(std::move(callback))
{}

} // namespace detail

} // namespace source
} // namespace sdeventplus
