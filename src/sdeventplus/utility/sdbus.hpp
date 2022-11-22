#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <stdexcept>

namespace sdeventplus::utility
{

/**
 * @brief Run and event loop with a given bus, bug workaround
 * @detail There is a known issue with systemd not correctly dispatching
 *         all of the events for a given bus. We need a workaround to
 *         prevent memory leaks.
 *         https://github.com/systemd/systemd/issues/22046
 */
inline int loopWithBus(Event& event, sdbusplus::bus_t& bus)
{
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
    try
    {
        do
        {
            if (bus.is_open())
            {
                // Process all outstanding bus events before running the loop.
                // This prevents the sd-bus handling logic from leaking memory.
                while (bus.process_discard() > 0)
                    ;
            }
        } while (event.run(std::nullopt) > 0);
    }
    catch (const SdEventError& e)
    {
        if (e.code().value() == ESTALE)
        {
            return event.get_exit_code();
        }
        throw;
    }
    throw std::runtime_error("Unknown sd-event terminaton");
}

} // namespace sdeventplus::utility
