#pragma once

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <stdplus/handle/managed.hpp>
#include <systemd/sd-bus.h>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{

/** @class Enabled
 *  @brief Mapping of sdeventplus source enable values to the sd-event
 *         equivalent
 */
enum class Enabled
{
    Off = SD_EVENT_OFF,
    On = SD_EVENT_ON,
    OneShot = SD_EVENT_ONESHOT,
};

/** @class Base
 *  @brief The base class for all sources implementing common source methods
 *         Not instantiated directly by end users
 */
class Base
{
  public:
    using Callback = std::function<void(Base& source)>;

    virtual ~Base();

    /** @brief Gets the underlying sd_event_source
     *
     *  @return The sd_event_source
     */
    sd_event_source* get() const;

    /** @brief Gets the associated Event object
     *
     *  @return The Event
     */
    const Event& get_event() const;

    /** @brief Gets the description of the source
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return The c-string description or a nullptr if none exists
     */
    const char* get_description() const;

    /** @brief Sets the description of the source
     *
     *  @param[in] description - The c-string description
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_description(const char* description) const;

    /** @brief Sets the callback associated with the source to be performed
     *         before the event loop goes to sleep, waiting for new events
     *
     *  @param[in] callback - Function run for preparation of the source
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_prepare(Callback&& callback);

    /** @brief Whether or not the source has any pending events that have
     *         not been dispatched yet.
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return 'true' if the source has pending events
     *          'false' otherwise
     */
    bool get_pending() const;

    /** @brief Gets the priority of the source relative to other sources
     *         The lower the priority the more important the source
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return A 64 bit integer representing the dispatch priority
     */
    int64_t get_priority() const;

    /** @brief Sets the priority of the source relative to other sources
     *         The lower the priority the more important the source
     *
     *  @param[in] priority - A 64 bit integer representing the priority
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_priority(int64_t priority) const;

    /** @brief Determines the enablement value of the source
     *
     *  @throws SdEventError for underlying sd_event errors
     *  @return The enabled status of the source
     */
    Enabled get_enabled() const;

    /** @brief Sets the enablement value of the source
     *
     *  @param[in] enabled - The new state of the source
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_enabled(Enabled enabled) const;

  protected:
    Event event;

    /** @brief Constructs a basic event source wrapper
     *         Owns the passed reference to the source
     *         This ownership is exception safe and will properly free the
     *         source in the case of an exception during construction
     *
     *  @param[in] event  - The event associated with the source
     *  @param[in] source - The underlying sd_event_source wrapped
     *  @param[in]        - Signifies that ownership is being transfered
     *  @throws SdEventError for underlying sd_event errors
     */
    Base(const Event& event, sd_event_source* source, std::false_type);

    // We can't ever copy an event_source because the callback
    // data has to be unique.
    Base(const Base& other) = delete;
    Base& operator=(const Base& other) = delete;
    // We don't want to allow any kind of slicing.
    Base(Base&& other);
    Base& operator=(Base&& other);

    /** @brief Returns a reference to the prepare callback executed for this
     *         source
     *
     *  @return A reference to the callback, this should be checked to make sure
     *          the callback is valid as there is no guarantee
     */
    const Callback& get_prepare() const;

    /** @brief A helper for subclasses to trivially wrap a c++ style callback
     *         to be called from the sd-event c library
     *
     *  @param[in] name     - The name of the callback for use in error messages
     *  @param[in] source   - The sd_event_source provided by sd-event
     *  @param[in] userdata - The userdata provided by sd-event
     *  @param[in] args...  - Extra arguments to pass to the callaback
     *  @return An negative errno on error, or 0 on success
     */
    template <typename Callback, class Source,
              const Callback& (Source::*getter)() const, typename... Args>
    static int sourceCallback(const char* name, sd_event_source*,
                              void* userdata, Args&&... args)
    {
        if (userdata == nullptr)
        {
            fprintf(stderr, "sdeventplus: %s: Missing userdata\n", name);
            return -EINVAL;
        }
        Source* source = reinterpret_cast<Source*>(userdata);
        return internal::performCallback(name, (source->*getter)(),
                                         std::ref(*source),
                                         std::forward<Args>(args)...);
    }

  private:
    static void drop(sd_event_source*&& source,
                     const internal::SdEvent*& sdevent);

    stdplus::Managed<sd_event_source*, const internal::SdEvent*>::Handle<drop>
        source;
    Callback prepare;

    /** @brief A helper used to make sure the userdata for the sd-event
     *         callback is set to the current source c++ object
     *
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_userdata();

    /** @brief A wrapper around the callback that can be called from sd-event
     *
     * @param[in] source   - The sd_event_source associated with the call
     * @param[in] userdata - The provided userdata for the source
     * @return 0 on success or a negative errno otherwise
     */
    static int prepareCallback(sd_event_source* source, void* userdata);
};

} // namespace source
} // namespace sdeventplus
