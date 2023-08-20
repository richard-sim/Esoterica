#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHIBuffer : public RHIResource
    {
    public:

        virtual ~RHIBuffer() = default;

    protected:

        RHIBufferCreateDesc                m_desc;
    };
}