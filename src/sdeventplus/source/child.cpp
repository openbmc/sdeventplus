#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/utils.hpp>
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

void Child::set_callback(Callback&& callback)
{
    this->callback = std::move(callback);
}

pid_t Child::get_pid() const
{
    pid_t pid;
    internal::callCheck("sd_event_source_get_child_pid",
                        &internal::SdEvent::sd_event_source_get_child_pid,
                        event.getSdEvent(), get(), &pid);
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
    internal::callCheck("sd_event_add_child",
                        &internal::SdEvent::sd_event_add_child,
                        event.getSdEvent(), event.get(), &source, pid, options,
                        childCallback, nullptr);
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
