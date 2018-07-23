#include <sdeventplus/exception.hpp>
#include <sdeventplus/source/child.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

Child::Child(const Event& event, pid_t pid, int options, Callback&& callback) :
    Base(event, create_source(event, pid, options), std::false_type()),
    callback(std::move(callback))
{
}

pid_t Child::get_pid() const
{
    pid_t pid;
    int r =
        event.getSdEvent()->sd_event_source_get_child_pid(source.get(), &pid);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_source_get_child_pid");
    }
    return pid;
}

const Child::Callback& Child::get_callback() const
{
    return callback;
}

sd_event_source* Child::create_source(const Event& event, pid_t pid,
                                      int options)
{
    sd_event_source* source;
    int r = event.getSdEvent()->sd_event_add_child(
        event.get(), &source, pid, options, childCallback, nullptr);
    if (r < 0)
    {
        throw SdEventError(-r, "sd_event_add_child");
    }
    return source;
}

int Child::childCallback(sd_event_source* source, const siginfo_t* si,
                         void* userdata)
{
    return sourceCallback<Callback, Child, &Child::get_callback>(
        "childCallback", source, userdata, si);
}

} // namespace source
} // namespace sdeventplus
