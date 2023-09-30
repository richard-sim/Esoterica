#pragma once

#include "RHITaggedType.h"

namespace EE::RHI
{
    class RHISwapchain : public RHITaggedType
    {
    public:

        RHISwapchain( ERHIType rhiType )
            : RHITaggedType( rhiType )
        {}
        virtual ~RHISwapchain() = default;

        RHISwapchain( RHISwapchain const& ) = delete;
        RHISwapchain& operator=( RHISwapchain const& ) = delete;

        RHISwapchain( RHISwapchain&& ) = default;
        RHISwapchain& operator=( RHISwapchain&& ) = default;

        //-------------------------------------------------------------------------

        inline ERHIType GetDynamicRHIType() const { return m_DynamicRHIType; }

    protected:

        ERHIType            m_DynamicRHIType;
    };

}


