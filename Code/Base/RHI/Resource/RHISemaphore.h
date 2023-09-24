#pragma once

#include "RHIResource.h"

namespace EE::RHI
{
    class RHISemaphore : public RHISynchronazationPrimitive
    {
    public:

        RHISemaphore( ERHIType rhiType = ERHIType::Invalid )
            : RHISynchronazationPrimitive( rhiType )
        {}
        virtual ~RHISemaphore() = default;
    };
}