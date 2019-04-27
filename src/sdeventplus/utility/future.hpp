#pragma once
#include <memory>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <stdplus/util/future.hpp>
#include <tuple>
#include <utility>
#include <vector>

namespace sdeventplus
{
namespace utility
{
namespace detail
{

template <typename T>
size_t numWakeable(const stdplus::util::Future<T>& future)
{
    return future.is_ready() ? 0 : 1;
}

template <typename C, typename = typename C::const_iterator>
size_t numWakeable(const C& futures)
{
    size_t n = 0;
    for (const auto& future : futures)
    {
        n += numWakeable(future);
    }
    return n;
}

template <typename... Futures>
size_t numWakeableAll(const Futures&... futures)
{
    return (0 + ... + numWakeable(futures));
}

template <typename... Futures>
size_t numWakeable(const std::tuple<Futures...>& futures)
{
    return std::apply(numWakeableAll<Futures...>, futures);
}

template <typename T>
void configureSingleWakeup(source::EventBase& source,
                           stdplus::util::Future<T>& future)
{
    if (future.is_ready())
    {
        return;
    }
    future.set_wakeup([source = source::EventBase(source, std::true_type())](
                          stdplus::util::Future<T>&) {
        source.set_enabled(source::Enabled::On);
    });
}

template <typename C, typename = typename C::iterator>
void configureSingleWakeup(source::EventBase& source, C& futures)
{
    for (auto& future : futures)
    {
        configureSingleWakeup(source, future);
    }
}

template <typename... Futures>
void configureSingleWakeupAll(source::EventBase& source, Futures&... futures)
{
    (configureSingleWakeup(source, futures), ...);
}

template <typename... Futures, std::size_t... I>
void configureSingleWakeup(source::EventBase& source,
                           std::tuple<Futures...>& futures,
                           std::index_sequence<I...>)
{
    configureSingleWakeupAll(source, std::get<I>(futures)...);
}

template <typename... Futures>
void configureSingleWakeup(source::EventBase& source,
                           std::tuple<Futures...>& futures)
{
    configureSingleWakeup(source, futures,
                          std::index_sequence_for<Futures...>());
}

template <typename T>
void configureSharedWakeup(source::EventBase& source,
                           const std::shared_ptr<size_t>& refs,
                           stdplus::util::Future<T>& future)
{
    if (future.is_ready())
    {
        return;
    }
    ++*refs;
    future.set_wakeup([source = source::EventBase(source, std::true_type()),
                       refs = refs](stdplus::util::Future<T>&) mutable {
        if (--*refs == 0)
        {
            source.set_enabled(source::Enabled::On);
        }
    });
}

template <typename C, typename = typename C::iterator>
void configureSharedWakeup(source::EventBase& source,
                           const std::shared_ptr<size_t>& refs, C& futures)
{
    for (auto& future : futures)
    {
        configureSharedWakeup(source, refs, future);
    }
}

template <typename... Futures>
void configureSharedWakeupAll(source::EventBase& source,
                              const std::shared_ptr<size_t>& refs,
                              Futures&... futures)
{
    (configureSharedWakeup(source, refs, futures), ...);
}

template <typename... Futures, std::size_t... I>
void configureSharedWakeup(source::EventBase& source,
                           const std::shared_ptr<size_t>& refs,
                           std::tuple<Futures...>& futures,
                           std::index_sequence<I...>)
{
    configureSharedWakeupAll(source, refs, std::get<I>(futures)...);
}

template <typename... Futures>
void configureSharedWakeup(source::EventBase& source,
                           const std::shared_ptr<size_t>& refs,
                           std::tuple<Futures...>& futures)
{
    configureSharedWakeup(source, refs, futures,
                          std::index_sequence_for<Futures...>());
}

template <typename... Futures>
void configureWakeups(source::EventBase& source, Futures&... futures)
{
    const size_t nwakeable = numWakeableAll(futures...);
    if (nwakeable == 1)
    {
        configureSingleWakeupAll(source, futures...);
        source.set_enabled(source::Enabled::Off);
    }
    else if (nwakeable > 1)
    {
        auto refs = std::make_shared<size_t>(0);
        configureSharedWakeupAll(source, refs, futures...);
        source.set_enabled(source::Enabled::Off);
    }
}

} // namespace detail

/** @brief Executes the callback when all of the futures are ready
 *
 *  @param[in] event      - The event loop used for executing the callback
 *  @param[in] cb         - The callback executed
 *  @param[in] futures... - The futures waited on and passed to the callback
 *  @return The event source representing this call
 */
template <typename Callback, typename... Futures>
[[nodiscard]] source::Defer callWhenReady(const Event& event, Callback&& cb,
                                          Futures&&... futures)
{
    auto ret = source::Defer(event, nullptr);
    detail::configureWakeups(ret, futures...);
    ret.set_callback(
        [cb = std::move(cb), futures = std::make_tuple(std::move(futures)...)](
            source::EventBase& source) mutable {
            // We don't want this to persist on the event loop
            source.set_enabled(source::Enabled::Off);
            source.set_floating(false);

            std::apply(cb, std::tuple_cat(std::tie(source.get_event()),
                                          std::move(futures)));
        });
    return ret;
}

} // namespace utility
} // namespace sdeventplus
