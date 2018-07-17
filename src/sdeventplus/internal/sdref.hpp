#pragma once

#include <functional>
#include <sdeventplus/internal/sdevent.hpp>
#include <type_traits>

namespace sdeventplus
{
namespace internal
{

template <typename T>
class SdRef
{
  public:
    using Func = std::function<T*(const SdEvent*, T*)>;

    SdRef(T* ref, Func take_ref, Func release_ref,
          const SdEvent* sdevent = &sdevent_impl);
    SdRef(T* ref, Func take_ref, Func release_ref, std::false_type,
          const SdEvent* sdevent = &sdevent_impl);
    virtual ~SdRef();

    SdRef(const SdRef& other);
    SdRef& operator=(const SdRef& other);
    SdRef(SdRef&& other) = default;
    SdRef& operator=(SdRef&& other);

    T* get() const;

  private:
    const SdEvent* sdevent;
    Func take_ref;
    Func release_ref;
    T* ref;
};

} // namespace internal
} // namespace sdeventplus
