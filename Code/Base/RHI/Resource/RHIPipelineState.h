#pragma once

#include "RHIResource.h"
#include "RHIResourceCreationCommons.h"

namespace EE::RHI
{
    // Pipeline State
    //-------------------------------------------------------------------------

    class RHIPipelineState : public RHIResource
    {
    public:

        RHIPipelineState( ERHIType rhiType = ERHIType::Invalid )
            : RHIResource( rhiType )
        {}
        virtual ~RHIPipelineState() = default;

        virtual RHIPipelineType GetPipelineType() const = 0;

    };

    class RHIRasterPipelineState : public RHIPipelineState
    {
    public:

        RHIRasterPipelineState( ERHIType rhiType = ERHIType::Invalid )
            : RHIPipelineState( rhiType )
        {}
        virtual ~RHIRasterPipelineState() = default;

        inline virtual RHIPipelineType GetPipelineType() const override { return RHIPipelineType::Raster; }

    protected:

        RHIRasterPipelineStateCreateDesc                m_desc;
    };

    class RHIComputePipelineState : public RHIPipelineState
    {
    public:

        RHIComputePipelineState( ERHIType rhiType = ERHIType::Invalid )
            : RHIPipelineState( rhiType )
        {}
        virtual ~RHIComputePipelineState() = default;

        inline virtual RHIPipelineType GetPipelineType() const override { return RHIPipelineType::Compute; }

    protected:
    };
}