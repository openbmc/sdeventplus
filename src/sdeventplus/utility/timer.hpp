#pragma once

#include <chrono>
#include <functional>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/time.hpp>

namespace sdeventplus
{
namespace utility
{

template <ClockId Id>
class Timer
{
  public:
    using Duration = typename Clock<Id>::duration;
    using Callback = std::function<void()>;

    Timer(const Timer& other) = delete;
    Timer(Timer&& other) = default;
    Timer& operator=(const Timer& other) = delete;
    Timer& operator=(Timer&& other) = default;
    virtual ~Timer() = default;

    Timer(const Event& event, Callback&& callback, Duration interval,
          Duration accuracy = std::chrono::milliseconds{1});

    bool hasExpired() const;
    bool isEnabled() const;
    Duration getInterval() const;
    Duration getRemaining() const;

    void setEnabled(bool enabled);
    void setRemaining(Duration remaining);
    void resetRemaining();
    void setInterval(Duration interval);
    void clearExpired();
    void restart(Duration interval);

  private:
    bool expired;
    Callback callback;
    Clock<Id> clock;
    Duration interval;
    source::Time<Id> timeSource;

    void internalCallback(source::Time<Id>&,
                          typename source::Time<Id>::TimePoint);
};

} // namespace utility
} // namespace sdeventplus
