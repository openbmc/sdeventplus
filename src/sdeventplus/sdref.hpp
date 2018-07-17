#pragma once

#include <functional>
#include <memory>
#include <sdeventplus/sdevent.hpp>
#include <type_traits>

namespace sdeventplus
{

template <typename T> class SdRef
{
  public:
    using Func = std::function<T*(SdEventInterface*, T*)>;

    SdRef(T* ref, Func take_ref, Func release_ref,
          SdEventInterface* intf = sdevent_impl) :
        SdRef(take_ref(intf, ref), take_ref, release_ref, std::false_type(),
              intf)
    {
    }

    SdRef(T* ref, Func take_ref, Func release_ref, std::false_type,
          SdEventInterface* intf = sdevent_impl) :
        intf(intf),
        take_ref(take_ref), release_ref(release_ref), ref(ref)
    {
    }

    SdRef(const SdRef& other) :
        intf(other.intf), take_ref(other.take_ref),
        release_ref(other.release_ref), ref(take_ref(intf, other.ref))
    {
    }

    SdRef& operator=(const SdRef& other)
    {
        if (this != &other)
        {
            // release_ref will be invalid if moved
            if (release_ref)
                release_ref(intf, ref);

            intf = other.intf;
            take_ref = other.take_ref;
            release_ref = other.release_ref;
            ref = take_ref(intf, other.ref);
        }
        return *this;
    }

    SdRef(SdRef&& other) = default;

    SdRef& operator=(SdRef&& other)
    {
        if (this != &other)
        {
            // release_ref will be invalid if move
            if (release_ref)
                release_ref(intf, ref);

            intf = std::move(other.intf);
            take_ref = std::move(other.take_ref);
            release_ref = std::move(other.release_ref);
            ref = std::move(other.ref);
        }
        return *this;
    }

    virtual ~SdRef()
    {
        // release_ref will be invalid after a move
        if (release_ref)
            release_ref(intf, ref);
    }

    T* get() const
    {
        return ref;
    }

  private:
    SdEventInterface* intf;
    Func take_ref;
    Func release_ref;
    T* ref;
};

} // namespace sdeventplus
