#include <sdeventplus/internal/sdref.hpp>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace internal
{

template <typename T>
SdRef<T>::SdRef(T* ref, Func take_ref, Func release_ref, SdEvent* sdevent) :
    SdRef(take_ref(sdevent, ref), take_ref, release_ref, std::false_type(),
          sdevent)
{
}

template <typename T>
SdRef<T>::SdRef(T* ref, Func take_ref, Func release_ref, std::false_type,
                SdEvent* sdevent) :
    sdevent(sdevent),
    take_ref(take_ref), release_ref(release_ref), ref(ref)
{
}

template <typename T>
SdRef<T>::SdRef(const SdRef& other) :
    SdRef(other.ref, other.take_ref, other.release_ref, other.sdevent)
{
}

template <typename T>
SdRef<T>& SdRef<T>::operator=(const SdRef& other)
{
    if (this != &other)
    {
        // release_ref will be invalid if moved
        if (release_ref)
            release_ref(sdevent, ref);

        sdevent = other.sdevent;
        take_ref = other.take_ref;
        release_ref = other.release_ref;
        ref = take_ref(sdevent, other.ref);
    }
    return *this;
}

template <typename T>
SdRef<T>& SdRef<T>::operator=(SdRef&& other)
{
    if (this != &other)
    {
        // release_ref will be invalid if move
        if (release_ref)
            release_ref(sdevent, ref);

        sdevent = std::move(other.sdevent);
        take_ref = std::move(other.take_ref);
        release_ref = std::move(other.release_ref);
        ref = std::move(other.ref);
    }
    return *this;
}

template <typename T>
SdRef<T>::~SdRef()
{
    // release_ref will be invalid after a move
    if (release_ref)
        release_ref(sdevent, ref);
}

template <typename T>
T* SdRef<T>::get() const
{
    return ref;
}

template class SdRef<sd_event>;
template class SdRef<sd_event_source>;

} // namespace internal
} // namespace sdeventplus
