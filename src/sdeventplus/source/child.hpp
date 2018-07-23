#pragma once

#include <functional>
#include <sdeventplus/source/base.hpp>
#include <signal.h>

namespace sdeventplus
{
namespace source
{

class Child : public Base
{
  public:
    using Callback = std::function<void(Child& source, const siginfo_t* si)>;

    Child(const Event& event, pid_t pid, int options, Callback&& callback);

    pid_t get_pid() const;

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const Event& event, pid_t pid,
                                          int options);

    static int childCallback(sd_event_source* source, const siginfo_t* si,
                             void* userdata);
};

} // namespace source
} // namespace sdeventplus
