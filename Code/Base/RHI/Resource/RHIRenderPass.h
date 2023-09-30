#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHIRenderPass : public RHIResource
    {
    public:

        RHIRenderPass( ERHIType rhiType = ERHIType::Invalid )
            : RHIResource( rhiType )
        {}
        virtual ~RHIRenderPass() = default;

    protected:

        RHIRenderPassCreateDesc             m_desc;
    };
}