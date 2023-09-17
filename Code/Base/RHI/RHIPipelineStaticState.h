#pragma once

#include "RenderAPI.h"

namespace EE::RHI
{
    class RHIPipelineRasterizerState
    {
    public:

        FillMode                        m_fillMode = FillMode::Solid;
        CullMode                        m_cullMode = Render::CullMode::BackFace;
        WindingMode                     m_WindingMode = WindingMode::CounterClockwise;
        bool                            m_enableScissorCulling = false;
    };

    class RHIPipelineBlendState
    {
    public:

        BlendValue                      m_srcValue = BlendValue::One;
        BlendValue                      m_dstValue = BlendValue::Zero;
        BlendOp                         m_blendOp = BlendOp::Add;
        BlendValue                      m_srcAlphaValue = BlendValue::One;
        BlendValue                      m_dstAlphaValue = BlendValue::Zero;
        BlendOp                         m_blendOpAlpha = BlendOp::Add;
        bool                            m_blendEnable = false;
    };

    enum class ERHIPipelinePirmitiveTopology
    {
        PointList = 0,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,

        None,
    };
}