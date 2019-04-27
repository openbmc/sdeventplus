#pragma once
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <stdplus/util/future.hpp>
#include <tuple>
#include <utility>

namespace sdeventplus
{
namespace utility
{

/** @brief Executes the callback when all of the futures are ready
 *
 *  @param[in] event      - The event loop used for executing the callback
 *  @param[in] cb         - The callback executed
 *  @param[in] futures... - The futures waited on and passed to the callback
 *  @return The event source representing this call
 */
template <typename Callback, typename... Futures>
[[nodiscard]] source::Post callWhenReady(const Event& event, Callback&& cb,
                                         Futures&&... futures) {
    auto outerCb = [cb = std::move(cb),
                    futures = std::make_tuple(std::move(futures)...)](
                       source::EventBase& source) mutable {
        if (!stdplus::util::isReady(futures))
        {
            return;
        }
        source.set_enabled(source::Enabled::Off);
        std::apply(cb, std::tuple_cat(std::tie(source.get_event()),
                                      std::move(futures)));
    };
    auto ret = source::Post(event, std::move(outerCb));
    // Ensure we are tested once immediately
    source::Defer(event, [](source::EventBase&) {}).set_floating(true);
    return ret;
}

} // namespace utility
} // namespace sdeventplus
