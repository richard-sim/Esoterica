#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHITexture : public RHIResource
    {
    public:

        virtual ~RHITexture() = default;

    public:

        RHITextureCreateDesc                m_desc;
    };
}