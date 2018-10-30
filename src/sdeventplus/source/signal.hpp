#pragma once

#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/base.hpp>
#include <sys/signalfd.h>

namespace sdeventplus
{
namespace source
{

/** @class Signal
 *  @brief A wrapper around the sd_event_source signal type
 *         See sd_event_add_signal(3) for more information
 */
class Signal : public Base
{
  public:
    /** @brief Type of the user provided callback function */
    using Callback =
        std::function<void(Signal& source, const struct signalfd_siginfo* si)>;

    /** @brief Creates a new signal event source on the provided event loop
     *         This type of source defaults to Enabled::On, executing the
     *         callback for each signal observed. You are required to block
     *         the signal in all threads prior to creating this source.
     *
     *  @param[in] event    - The event to attach the handler
     *  @param[in] sig      - Signum to watch, see signal(7)
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    Signal(const Event& event, int sig, Callback&& callback);

    /** @brief Sets the callback
     *
     *  @param[in] callback - The function executed on event dispatch
     */
    void set_callback(Callback&& callback);

    /** @brief Gets the signum watched by the source
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return Integer signum
     */
    int get_signal() const;

  private:
    Callback callback;

    /** @brief Returns a reference to the callback executed for this source
     *
     *  @return A reference to the callback
     */
    const Callback& get_callback() const;

    /** @brief Creates a new signal source attached to the Event
     *
     *  @param[in] event    - The event to attach the handler
     *  @param[in] sig      - Signum to watch, see signal(7)
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     *  @return A new sd_event_source
     */
    static sd_event_source* create_source(const Event& event, int sig);

    /** @brief A wrapper around the callback that can be called from sd-event
     *
     *  @param[in] source   - The sd_event_source associated with the call
     *  @param[in] userdata - The provided userdata for the source
     *  @return 0 on success or a negative errno otherwise
     */
    static int signalCallback(sd_event_source* source,
                              const struct signalfd_siginfo* si,
                              void* userdata);
};

} // namespace source
} // namespace sdeventplus
