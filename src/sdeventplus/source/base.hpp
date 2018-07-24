#pragma once

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <sdeventplus/event.hpp>
#include <sdeventplus/internal/sdref.hpp>
#include <sdeventplus/internal/utils.hpp>
#include <systemd/sd-bus.h>
#include <type_traits>

namespace sdeventplus
{
namespace source
{

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
    int get_pending() const;

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
    int get_enabled() const;

    /** @brief Sets the enablement value of the source
     *
     *  @param[in] enabled - The new state of the source
     *  @throws SdEventError for underlying sd_event errors
     */
    void set_enabled(int enabled) const;

  protected:
    Event event;
    internal::SdRef<sd_event_source> source;

    // Base sources cannot be directly constructed.
    Base(const Event& event, sd_event_source* source);
    Base(const Event& event, sd_event_source* source, std::false_type);

    // We can't ever copy an event_source because the callback
    // data has to be unique.
    Base(const Base& other) = delete;
    Base& operator=(const Base& other) = delete;
    // We don't want to allow any kind of slicing.
    Base(Base&& other);
    Base& operator=(Base&& other);

    const Callback& get_prepare() const;

    template <typename Callback, class Source,
              const Callback& (Source::*getter)() const, typename... Args>
    static int sourceCallback(const char* name, sd_event_source*,
                              void* userdata, Args... args)
    {
        if (userdata == nullptr)
        {
            fprintf(stderr, "sdeventplus: %s: Missing userdata\n", name);
            return -EINVAL;
        }
        Source* source = reinterpret_cast<Source*>(userdata);
        return internal::performCallback(name, (source->*getter)(),
                                         std::ref(*source), args...);
    }

  private:
    Callback prepare;

    void set_userdata();
    static int prepareCallback(sd_event_source* source, void* userdata);
};

} // namespace source
} // namespace sdeventplus
