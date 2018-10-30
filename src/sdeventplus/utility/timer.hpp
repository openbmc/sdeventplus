#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/time.hpp>

namespace sdeventplus
{
namespace utility
{

/** @class Timer<Id>
 *  @brief A simple, repeating timer around an sd_event time source
 *  @details Adds a timer to the SdEvent loop that runs a user defined callback
 *           at specified intervals. If no interval is provided to the timer,
 *           it can be used for oneshot actions. Besides running callbacks, the
 *           timer tracks whether or not it has expired since creation or since
 *           the last clearExpired() or restart(). The concept of expiration is
 *           orthogonal to the callback mechanism and can be ignored.
 *
 *           See example/{heartbeat_timer,delayed_echo}.cpp for usage examples.
 */
template <ClockId Id>
class Timer
{
  public:
    /** @brief Type used to represent a time duration for the timer
     *         interval or time remaining.
     */
    using Duration = typename Clock<Id>::duration;

    /** @brief Type of the user provided callback function when the
     *         timer elapses.
     */
    using Callback = std::function<void(Timer<Id>&)>;

    Timer(const Timer& other) = delete;
    Timer(Timer&& other);
    Timer& operator=(const Timer& other) = delete;
    Timer& operator=(Timer&& other);
    virtual ~Timer() = default;

    /** @brief Creates a new timer on the given event loop.
     *         This timer is created enabled by default if passed an interval.
     *
     *  @param[in] event    - The event we are attaching to
     *  @param[in] callback - The user provided callback run when elapsing
     *                        This can be empty
     *  @param[in] interval - Optional amount of time in-between timer
     *                        expirations. std::nullopt means the interval
     *                        will be provided later.
     *  @param[in] accuracy - Optional amount of error tolerable in timer
     *                        expiration. Defaults to 1ms.
     *  @throws SdEventError for underlying sd_event errors
     */
    Timer(const Event& event, Callback&& callback,
          std::optional<Duration> interval = std::nullopt,
          typename source::Time<Id>::Accuracy accuracy =
              std::chrono::milliseconds{1});

    /** @brief Sets the callback
     *
     *  @param[in] callback - The function executed on timer elapse
     */
    void set_callback(Callback&& callback);

    /** @brief Gets the associated Event object
     *
     *  @return The Event
     */
    const Event& get_event() const;

    /** @brief Has the timer expired since creation or reset of expiration
     *         state.
     *
     *  @return True if expired, false otherwise
     */
    bool hasExpired() const;

    /** @brief Is the timer currently running on the event loop.
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return True if running, false otherwise
     */
    bool isEnabled() const;

    /** @brief Gets interval between timer expirations
     *         The timer may not have a configured interval and is instead
     *         operating as a one-shot timer.
     *
     *  @return The interval as an std::chrono::duration
     */
    std::optional<Duration> getInterval() const;

    /** @brief Gets time left before the timer expirations
     *
     *  @throws std::runtime_error if the timer is not enabled
     *  @throws SdEventError for underlying sd_event errors
     *  @return The remaining time as an std::chrono::duration
     */
    Duration getRemaining() const;

    /** @brief Sets whether or not the timer is running on the event loop
     *         This does not alter the expiration time of the timer.
     *
     *  @param[in] enabled - Should the timer be enabled or disabled
     *  @throws std::runtime_error If the timer has not been initialized
     *  @throws SdEventError for underlying sd_event errors
     */
    void setEnabled(bool enabled);

    /** @brief Sets the amount of time left until the timer expires.
     *         This does not affect the interval used for subsequent runs.
     *
     *  @param[in] remaining - The new amount of time left on the timer
     *  @throws SdEventError for underlying sd_event errors
     */
    void setRemaining(Duration remaining);

    /** @brief Resets the amount of time left to the interval of the timer.
     *
     *  @throws SdEventError for underlying sd_event errors
     */
    void resetRemaining();

    /** @brief Sets the interval of the timer for future timer expirations.
     *         This does not alter the current expiration time of the timer.
     *
     *  @param[in] interval - The new interval for the timer
     */
    void setInterval(std::optional<Duration> interval);

    /** @brief Resets the expired status of the timer. */
    void clearExpired();

    /** @brief Restarts the timer as though it has been completely
     *         re-initialized. Expired status is reset, interval is updated,
     *         time remaining is set to the new interval, and the timer is
     *         enabled if the interval is populated.
     *
     *  @param[in] interval - The new interval for the timer
     *  @throws SdEventError for underlying sd_event errors
     */
    void restart(std::optional<Duration> interval);

    /** @brief Restarts the timer as though it has been completely
     *         re-initialized. Expired status is reset, interval is removed,
     *         time remaining is set to the new remaining, and the timer is
     *         enabled as a one shot.
     *
     *  @param[in] interval - The new interval for the timer
     *  @throws SdEventError for underlying sd_event errors
     */
    void restartOnce(Duration remaining);

  private:
    /** @brief Tracks the expiration status of the timer */
    bool expired;
    /** @brief Tracks whether or not the expiration timeout is valid */
    bool initialized;
    /** @brief User defined callback run on each expiration */
    Callback callback;
    /** @brief Clock used for updating the time source */
    Clock<Id> clock;
    /** @brief Interval between each timer expiration */
    std::optional<Duration> interval;
    /** @brief Underlying sd_event time source that backs the timer */
    source::Time<Id> timeSource;

    /** @brief Used as a helper to run our user defined callback on the
     *         timeSource
     */
    void internalCallback(source::Time<Id>&,
                          typename source::Time<Id>::TimePoint);
};

} // namespace utility
} // namespace sdeventplus
