#pragma once

#include "../RHITaggedType.h"

namespace EE::RHI
{
    class RHIResource : public RHITaggedType
    {
    public:

        RHIResource( ERHIType rhiType = ERHIType::Invalid )
            : RHITaggedType( rhiType )
        {}
        virtual ~RHIResource() = default;

        RHIResource( RHIResource const& ) = delete;
        RHIResource& operator=( RHIResource const& ) = delete;

        RHIResource( RHIResource&& ) = default;
        RHIResource& operator=( RHIResource&& ) = default;
    };

    class RHISynchronazationPrimitive : public RHIResource
    {
    public:

        RHISynchronazationPrimitive( ERHIType rhiType = ERHIType::Invalid )
            : RHIResource( rhiType )
        {}
        virtual ~RHISynchronazationPrimitive() = default;

    public:


    };
}


