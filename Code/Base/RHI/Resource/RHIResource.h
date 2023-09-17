#pragma once

namespace EE::RHI
{
    class RHIResource
    {
    public:

        RHIResource() = default;
        virtual ~RHIResource() = default;

        RHIResource( RHIResource const& ) = delete;
        RHIResource& operator=( RHIResource const& ) = delete;

        RHIResource( RHIResource&& ) = default;
        RHIResource& operator=( RHIResource&& ) = default;
    };

    class RHISynchronazationPrimitive : public RHIResource
    {
    public:

        virtual ~RHISynchronazationPrimitive() = default;

    public:


    };
}


