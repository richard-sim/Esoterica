#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHIBuffer : public RHIResource
    {
    public:

        RHIBuffer( ERHIType rhiType = ERHIType::Invalid )
            : RHIResource( rhiType )
        {}
        virtual ~RHIBuffer() = default;

    protected:

        RHIBufferCreateDesc                m_desc;
    };
}