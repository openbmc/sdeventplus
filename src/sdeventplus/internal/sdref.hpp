#pragma once

#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <type_traits>

namespace sdeventplus
{
namespace internal
{

/** @class SdRef
 *  @brief Takes and releases references to T objects
 *  @details Used primarily as an RAII wrapper around sd_event
 *           and sd_event_source object references.
 */
template <typename T>
class SdRef
{
  public:
    /** @brief The type signature of ref / deref functions
     */
    using Func = std::function<T*(const SdEvent*, T*)>;

    /** @brief Constructs a new reference holder
     *         This constructor calls take_ref on ref
     *
     *  @param[in] ref         - Object which is referenced
     *  @param[in] take_ref    - Function used to take references
     *  @param[im] release_ref - Function used to release references
     *  @param[in] sdevent     - Optional underlying sd_event implementation
     */
    SdRef(T* ref, Func take_ref, Func release_ref,
          const SdEvent* sdevent = &sdevent_impl);

    /** @brief Constructs a new reference holder
     *         Does not take a new reference on the passed ref
     *         NOTE: This will still take a reference during future copies
     *         Useful for first creation of an object reference
     *
     *  @param[in] ref         - Object which is referenced
     *  @param[in] take_ref    - Function used to take references
     *  @param[im] release_ref - Function used to release references
     *  @param[in]             - Denotes no reference taken during construction
     *  @param[in] sdevent     - Optional underlying sd_event implementation
     */
    SdRef(T* ref, Func take_ref, Func release_ref, std::false_type,
          const SdEvent* sdevent = &sdevent_impl);

    virtual ~SdRef();
    SdRef(const SdRef& other);
    SdRef& operator=(const SdRef& other);
    SdRef(SdRef&& other);
    SdRef& operator=(SdRef&& other);

    /** @brief Determines if a reference is currently being held
     *
     *  @return 'true' if a reference is held
     *          'false' if empty
     */
    explicit operator bool() const;

    /** @brief Get a pointer to the object being referenced
     *
     * @return The object pointer
     */
    T* get() const;

  private:
    const SdEvent* sdevent;
    Func take_ref;
    Func release_ref;
    T* ref;
};

} // namespace internal
} // namespace sdeventplus
