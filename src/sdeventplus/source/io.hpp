#pragma once

#include <cstdint>
#include <functional>
#include <sdeventplus/source/base.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace source
{

class IO : public Base
{
  public:
    using Callback = std::function<void(IO&, int fd, uint32_t revents)>;

    IO(const Event& event, int fd, uint32_t events, Callback&& callback);

    int get_fd() const;
    void set_fd(int fd) const;
    bool get_fd_own() const;
    void set_fd_own(bool own) const;
    uint32_t get_events() const;
    void set_events(uint32_t events) const;
    uint32_t get_revents() const;

  private:
    Callback callback;

    const Callback& get_callback() const;

    static sd_event_source* create_source(const Event& event, int fd,
                                          uint32_t events);

    static int ioCallback(sd_event_source* source, int fd, uint32_t revents,
                          void* userdata);
};

} // namespace source
} // namespace sdeventplus
