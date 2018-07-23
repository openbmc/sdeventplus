#pragma once

#include <experimental/optional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{

/** @class Event
 *  @brief C++ Wrapper around sd_event objects
 */
class Event
{
  public:
    /** @brief Duration type used for specifying timeouts
     */
    using Timeout = SdEventDuration;
    using MaybeTimeout = std::experimental::optional<Timeout>;

    /** @brief Constructs a new event from sd_event
     *         Takes a reference on the passed in sd_event
     *
     *  @param[in] event   - The sd_event to wrap
     *  @param[in] sdevent - Optional underlying sd_event implementation
     *  @throws SdEventError for underlying sd_event errors
     */
    Event(sd_event* event,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);

    /** @brief Constructs a new event from sd_event
     *         Does not take a reference on the passed in sd_event
     *         NOTE: This will still take a reference during future copies
     *         Useful for first creation of an sd_event
     *
     *  @param[in] event   - The sd_event to wrap
     *  @param[in]         - Denotes no reference taken during construction
     *  @param[in] sdevent - Optional underlying sd_event implementation
     *  @throws SdEventError for underlying sd_event errors
     */
    Event(sd_event* event, std::false_type,
          const internal::SdEvent* sdevent = &internal::sdevent_impl);

    /** @brief Create a wrapped event around sd_event_new()
     *
     *  @param[in] sdevent - Optional underlying sd_event implementation
     *  @throws SdEventError for underlying sd_event errors
     *  @return A new Event
     */
    static Event
        get_new(const internal::SdEvent* sdevent = &internal::sdevent_impl);

    /** @brief Create a wrapped event around sd_event_default()
     *
     *  @param[in] sdevent - Optional underlying sd_event implementation
     *  @throws SdEventError for underlying sd_event errors
     *  @return A new default Event
     */
    static Event
        get_default(const internal::SdEvent* sdevent = &internal::sdevent_impl);

    /** @brief Get the underlying sd_event
     *
     *  @return The sd_event
     */
    sd_event* get() const;

    /** @brief Get the sd_event interface in use
     *
     *  @return The sd_event interface
     */
    const internal::SdEvent* getSdEvent() const;

    /** @brief Check for pending events and arm timers
     *
     * @throws SdEventError for underlying sd_event errors
     * @return Positive value if events are pending, 0 otherwise
     */
    int prepare() const;

    /** @brief Waits for new events to process
     *
     * @param[in] timeout - nullopt for no timeout or a finite timeout
     * @throws SdEventError for underlying sd_event errors
     * @return Positive value if events are pending, 0 otherwise
     *         Only returns 0 a finite timeout is reached
     */
    int wait(MaybeTimeout timeout) const;

    /** @brief Executes the highest priority event
     *
     * @throws SdEventError for underlying sd_event errors
     * @return Positive value if the event loop should continue
     *         0 value if the event loop should exit
     */
    int dispatch() const;

    /** @brief Runs a single iteration of the event loop
     *
     * @param[in] timeout - nullopt for no timeout or a finite timeout
     * @throws SdEventError for underlying sd_event errors
     * @return Positive value if an event was dispatched
     *         Only returns 0 a finite timeout is reached
     */
    int run(MaybeTimeout timeout) const;

    /** @brief Run the event loop to completion
     *
     * @throws SdEventError for underlying sd_event errors
     * @return Exit status of the event loop from exit()
     */
    int loop() const;

    /** @brief Sets the exit code for the loop and notifies
     *         the event loop it should terminate
     *
     * @param[in] code - The exit code
     * @throws SdEventError for underlying sd_event errors
     */
    void exit(int code) const;

    /** @brief Gets the exit code for the event loop
     *
     * @throws SdEventError for underlying sd_event errors
     * @return The exit code specified by a previous exit()
     */
    int get_exit_code() const;

    /** @brief Get the status of the event loop watchdog
     *
     * @throws SdEventError for underlying sd_event errors
     * @return 'true' if the watchdog is supported and enabled
     *         'false' otherwise
     */
    int get_watchdog() const;

    /** @brief Sets the state of the event loop watchdog
     *
     * @param[in] b - Should the event loop attempt to enable the watchdog
     * @throws SdEventError for underlying sd_event errors
     * @return 'true' if the watchdog is supported and enabled
     *         'false' otherwise
     */
    int set_watchdog(int b) const;

  private:
    const internal::SdEvent* sdevent;
    internal::SdRef<sd_event> event;
};

} // namespace sdeventplus
