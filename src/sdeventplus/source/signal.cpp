#include <memory>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <sdeventplus/source/signal.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

Signal::Signal(const Event& event, int sig, Callback&& callback) :
    Base(event, create_source(event, sig), std::false_type())
{
    set_userdata(std::make_unique<Signal>(*this, std::true_type()));
    set_callback(std::move(callback));
}

Signal::Signal(const Signal& other, std::true_type) :
    Base(other, std::true_type())
{
}

void Signal::set_callback(Callback&& callback)
{
    get_userdata().callback = std::move(callback);
}

int Signal::get_signal() const
{
    return internal::callCheck("sd_event_source_get_signal",
                               &internal::SdEvent::sd_event_source_get_signal,
                               event.getSdEvent(), get());
}

Signal& Signal::get_userdata() const
{
    return *reinterpret_cast<Signal*>(
        event.getSdEvent()->sd_event_source_get_userdata(get()));
}

Signal::Callback& Signal::get_callback()
{
    return get_userdata().callback;
}

sd_event_source* Signal::create_source(const Event& event, int sig)
{
    sd_event_source* source;
    internal::callCheck(
        "sd_event_add_signal", &internal::SdEvent::sd_event_add_signal,
        event.getSdEvent(), event.get(), &source, sig, signalCallback, nullptr);
    return source;
}

int Signal::signalCallback(sd_event_source* source,
                           const struct signalfd_siginfo* si, void* userdata)
{
    return sourceCallback<Callback, Signal, &Signal::get_callback>(
        "signalCallback", source, userdata, si);
}

} // namespace source
} // namespace sdeventplus
