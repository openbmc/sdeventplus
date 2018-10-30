#pragma once

#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <systemd/sd-event.h>

namespace sdeventplus
{
namespace source
{

/** @class EventBase
 *  @brief A wrapper around the sd_event_source defer type
 *         See sd_event_add_defer(3) for more information
 *         There are multiple types of defer sources, instantiate
 *         Defer, Post, or Exit depending on the required event.
 */
class EventBase : public Base
{
  public:
    using Callback = std::function<void(EventBase& source)>;

    /** @brief Sets the callback
     *
     *  @param[in] callback - The function executed on event dispatch
     */
    void set_callback(Callback&& callback);

  protected:
    using CreateFunc = decltype(&internal::SdEvent::sd_event_add_exit);

    /** @brief Adds a new event source handler to the Event
     *         This type of source defaults to Enabled::Oneshot, and needs to be
     *         reconfigured upon each callback.
     *
     *  @param[in] name   - The name identifying the create function
     *  @param[in] create - The SdEvent function called to create the source
     *  @param[in] event  - The event to attach the handler
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    EventBase(const char* name, CreateFunc create, const Event& event,
              Callback&& callback);

  private:
    Callback callback;

    /** @brief Returns a reference to the callback executed for this source
     *
     *  @return A reference to the callback
     */
    const Callback& get_callback() const;

    /** @brief Creates a new source attached to the Event
     *
     *  @param[in] name   - The name identifying the create function
     *  @param[in] create - The SdEvent function called to create the source
     *  @param[in] event  - The event to attach the handler
     *  @throws SdEventError for underlying sd_event errors
     *  @return A new sd_event_source
     */
    static sd_event_source* create_source(const char* name, CreateFunc create,
                                          const Event& event);

    /** @brief A wrapper around the callback that can be called from sd-event
     *
     *  @param[in] source   - The sd_event_source associated with the call
     *  @param[in] userdata - The provided userdata for the source
     *  @return 0 on success or a negative errno otherwise
     */
    static int eventCallback(sd_event_source* source, void* userdata);
};

class Defer : public EventBase
{
  public:
    /** @brief Adds a new defer source handler to the Event
     *         Executes the callback upon events occurring
     *
     *  @param[in] event  - The event to attach the handler
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    Defer(const Event& event, Callback&& Callback);
};

class Post : public EventBase
{
  public:
    /** @brief Adds a new post source handler to the Event
     *         Executes the callback upon events occurring
     *
     *  @param[in] event  - The event to attach the handler
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    Post(const Event& event, Callback&& callback);
};

class Exit : public EventBase
{
  public:
    /** @brief Adds a new exit source handler to the Event
     *         Executes the callback upon events occurring
     *
     *  @param[in] event  - The event to attach the handler
     *  @param[in] callback - The function executed on event dispatch
     *  @throws SdEventError for underlying sd_event errors
     */
    Exit(const Event& event, Callback&& callback);
};

} // namespace source
} // namespace sdeventplus
