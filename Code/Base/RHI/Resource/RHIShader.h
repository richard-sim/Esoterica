#pragma once

#include "RHIResource.h"

namespace EE::RHI
{
    class RHIShader : public RHIResource
    {
    public:

        RHIShader( ERHIType rhiType = ERHIType::Invalid )
            : RHIResource( rhiType )
        {}
        virtual ~RHIShader() = default;

        virtual bool IsValid() const = 0;

    private:

    };
}
