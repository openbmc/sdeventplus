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

Child::Child(const Child& other) : Base(other)
{
}

Child& Child::operator=(const Child& other)
{
    static_cast<Base&>(*this) = other;
    return *this;
}

Child::Child(const Event& event, pid_t pid, int options, Callback&& callback) :
    Base(event, create_source(event, pid, options), std::false_type())
{
    set_userdata(
        std::make_unique<detail::ChildData>(*this, std::move(callback)));
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

detail::ChildData& Child::get_userdata() const
{
    return static_cast<detail::ChildData&>(Base::get_userdata());
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
    return sourceCallback<Callback, detail::ChildData, &Child::get_callback>(
        "childCallback", source, userdata, si);
}

namespace detail
{

ChildData::ChildData(const Child& base, Child::Callback&& callback) :
    Child(base, std::true_type()), BaseData(base), callback(std::move(callback))
{
}

} // namespace detail

} // namespace source
} // namespace sdeventplus
