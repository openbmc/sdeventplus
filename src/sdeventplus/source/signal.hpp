#pragma once

#include <functional>
#include <sdeventplus/source/base.hpp>
#include <sys/signalfd.h>

namespace sdeventplus
{
namespace source
{

class Signal : public Base
{
  public:
    using Callback =
        std::function<void(Signal& source, const struct signalfd_siginfo* si)>;

    Signal(const Event& event, int sig, Callback&& callback);

    int get_signal() const;

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const Event& event, int sig);

    static int signalCallback(sd_event_source* source,
                              const struct signalfd_siginfo* si,
                              void* userdata);
};

} // namespace source
} // namespace sdeventplus
