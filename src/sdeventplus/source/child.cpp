#include <memory>
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
    Base(event, create_source(event, pid, options), std::false_type())
{
    set_userdata(std::make_unique<Child>(*this, std::true_type()));
    set_callback(std::move(callback));
}

Child::Child(const Child& other, std::true_type) : Base(other, std::true_type())
{
}

void Child::set_callback(Callback&& callback)
{
    get_userdata().callback = std::move(callback);
}

pid_t Child::get_pid() const
{
    pid_t pid;
    internal::callCheck("sd_event_source_get_child_pid",
                        &internal::SdEvent::sd_event_source_get_child_pid,
                        event.getSdEvent(), get(), &pid);
    return pid;
}

Child& Child::get_userdata() const
{
    return *reinterpret_cast<Child*>(
        event.getSdEvent()->sd_event_source_get_userdata(get()));
}

Child::Callback& Child::get_callback()
{
    return get_userdata().callback;
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
