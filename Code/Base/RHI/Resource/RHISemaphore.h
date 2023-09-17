#pragma once

#include "RHIResource.h"

namespace EE::RHI
{
    class RHISemaphore : public RHISynchronazationPrimitive
    {
    public:

        virtual ~RHISemaphore() = default;
    };
}