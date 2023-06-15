#pragma once

#include "LightWSI.h"
#include "DispatcheTable.h"

#include <iostream>

namespace LightWSI
{

template <class T>
static inline void *GetKey(T dispatchableObject)
{
    return *reinterpret_cast<void **>(dispatchableObject);
}

template <class T, T TDefault = T{VK_NULL_HANDLE}>
class Handle
{
public:
    LWSI_SWAPPABLE(Handle)

public:
    Handle() :
        handle{TDefault}
    {

    }

    Handle(T handle) :
        handle{handle}
    {
    }

    operator T() const
    {
        return handle;
    }

    void Swap(Handle &&other)
    {
        std::swap(handle, other.handle);
    }

protected:
    T handle;
};

}
