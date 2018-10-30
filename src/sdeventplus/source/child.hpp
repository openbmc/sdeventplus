#pragma once

#include <functional>
#include <sdeventplus/source/base.hpp>
#include <signal.h>

namespace sdeventplus
{
namespace source
{

/** @class Child
 *  @brief A wrapper around the sd_event_source child type
 *         See sd_event_add_child(3) for more information
 */
class Child : public Base
{
  public:
    using Callback = std::function<void(Child& source, const siginfo_t* si)>;

    /** @brief Adds a new child source handler to the Event
     *         This type of source defaults to Enabled::Oneshot, and needs to be
     *         reconfigured upon each callback.
     *
     *  @param[in] event    - The event to attach the handler
     *  @param[in] pid      - The pid of the child to monitor
     *  @param[in] options  - An OR-ed mask that determines triggers
     *                        See waitid(2) for further information
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    Child(const Event& event, pid_t pid, int options, Callback&& callback);

    /** @brief Sets the callback
     *
     *  @param[in] callback - The function executed on event dispatch
     */
    void set_callback(Callback&& callback);

    /** @brief Gets the pid of the child process being watched
     *
     *  @return The child pid
     *  @throws SdEventError for underlying sd_event errors
     */
    pid_t get_pid() const;

  private:
    Callback callback;

    /** @brief Returns a reference to the callback executed for this source
     *
     *  @return A reference to the callback
     */
    const Callback& get_callback() const;

    /** @brief Creates a new child source attached to the Event
     *
     *  @param[in] event   - The event to attach the handler
     *  @param[in] pid     - The pid of the child to monitor
     *  @param[in] options - An OR-ed mask that determines triggers
     *  @throws SdEventError for underlying sd_event errors
     *  @return A new sd_event_source
     */
    static sd_event_source* create_source(const Event& event, pid_t pid,
                                          int options);

    /** @brief A wrapper around the callback that can be called from sd-event
     *
     *  @param[in] source   - The sd_event_source associated with the call
     *  @param[in] userdata - The provided userdata for the source
     *  @return 0 on success or a negative errno otherwise
     */
    static int childCallback(sd_event_source* source, const siginfo_t* si,
                             void* userdata);
};

} // namespace source
} // namespace sdeventplus
