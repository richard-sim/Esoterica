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

        virtual ~RHIPipelineState() = default;

        virtual RHIPipelineType GetPipelineType() const = 0;

    };

    class RHIRasterPipelineState : public RHIPipelineState
    {
    public:

        virtual ~RHIRasterPipelineState() = default;

        inline virtual RHIPipelineType GetPipelineType() const override { return RHIPipelineType::Raster; }

    protected:

        RHIRasterPipelineStateCreateDesc                m_desc;
    };

    class RHIComputePipelineState : public RHIPipelineState
    {
    public:

        virtual ~RHIComputePipelineState() = default;

        inline virtual RHIPipelineType GetPipelineType() const override { return RHIPipelineType::Compute; }

    protected:
    };
}