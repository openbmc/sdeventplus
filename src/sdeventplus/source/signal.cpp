#include <sdeventplus/exception.hpp>
#include <sdeventplus/source/signal.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

Signal::Signal(const Event& event, int sig, Callback&& callback) :
    Base(event, create_source(event, sig), std::false_type()),
    callback(std::move(callback))
{
}

int Signal::get_signal() const
{
    int r = event.getSdEvent()->sd_event_source_get_signal(source.get());
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_signal");
    }
    return r;
}

const Signal::Callback& Signal::get_callback() const
{
    return callback;
}

sd_event_source* Signal::create_source(const Event& event, int sig)
{
    sd_event_source* source;
    int r = event.getSdEvent()->sd_event_add_signal(event.get(), &source, sig,
                                                    signalCallback, nullptr);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_add_signal");
    }
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
