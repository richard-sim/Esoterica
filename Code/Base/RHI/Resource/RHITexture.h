#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHITexture : public RHIResource
    {
    public:

        virtual ~RHITexture() = default;

    protected:

        RHITextureCreateDesc                m_desc;
    };
}