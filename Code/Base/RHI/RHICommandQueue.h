#pragma once

#include "RHITaggedType.h"

namespace EE::RHI
{
    class RHICommandQueue : public RHITaggedType
    {
    public:

        RHICommandQueue( ERHIType rhiType )
            : RHITaggedType( rhiType )
        {
        }
        virtual ~RHICommandQueue() = default;

        RHICommandQueue( RHICommandQueue const& ) = delete;
        RHICommandQueue& operator=( RHICommandQueue const& ) = delete;

        RHICommandQueue( RHICommandQueue&& ) = default;
        RHICommandQueue& operator=( RHICommandQueue&& ) = default;

    private:

    };

}