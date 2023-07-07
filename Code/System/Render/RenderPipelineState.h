#pragma once

#include "System/Log.h"
#include "System/Render/RenderAPI.h"
#include "System/Render/RenderStates.h"
#include "System/Resource/ResourcePath.h"
#include "System/Algorithm/Hash.h"
#include "System/Types/Set.h"

//-------------------------------------------------------------------------

namespace EE::Render
{
    // [ Legacy ]
    //struct PipelineState
    //{
    //    inline void Clear()
    //    {
    //        m_pVertexShader = nullptr;
    //        m_pGeometryShader = nullptr;
    //        m_pHullShader = nullptr;
    //        m_pComputeShader = nullptr;
    //        m_pPixelShader = nullptr;
    //        m_pBlendState = nullptr;
    //        m_pRasterizerState = nullptr;
    //    }

    //public:

    //    VertexShader*                   m_pVertexShader = nullptr;
    //    GeometryShader*                 m_pGeometryShader = nullptr;
    //    Shader*                         m_pHullShader = nullptr;
    //    Shader*                         m_pComputeShader = nullptr;
    //    PixelShader*                    m_pPixelShader = nullptr;
    //    BlendState*                     m_pBlendState = nullptr;
    //    RasterizerState*                m_pRasterizerState = nullptr;
    //};

    struct EE_SYSTEM_API PipelineShaderDesc
    {
        PipelineShaderDesc( PipelineStage stage, ResourcePath shaderPath, String entryName = "main" );

        inline PipelineShaderDesc& SetStage( PipelineStage stage )
        {
            EE_ASSERT( stage != PipelineStage::None );
            this->m_stage = stage;
            return *this;
        }

        inline PipelineShaderDesc& SetShaderPath( ResourcePath shaderPath )
        {
            EE_ASSERT( shaderPath.IsValid() && shaderPath.IsFile() );
            this->m_shaderPath = shaderPath;
            return *this;
        }

        inline PipelineShaderDesc& SetEntryName( String entryName )
        {
            EE_ASSERT( !entryName.empty() );
            this->m_entryName = entryName;
            return *this;
        }

        inline bool IsValid() const
        {
            return m_stage != PipelineStage::None && m_shaderPath.IsValid() && m_shaderPath.IsFile() && !m_entryName.empty();
        }

    public:

        friend bool operator<( PipelineShaderDesc const& lhs, PipelineShaderDesc const& rhs )
        {
            return static_cast<uint8_t>( lhs.m_stage ) < static_cast<uint8_t>( rhs.m_stage );
        }

    public:

        PipelineStage           m_stage = PipelineStage::Vertex;
        ResourcePath            m_shaderPath;
        String                  m_entryName = "main";
    };

    struct RasterPipelineDesc
    {
        inline RasterPipelineDesc& AddShader( PipelineShaderDesc&& shaderDesc )
        {
            for ( auto const& sd : m_shaderDescs )
            {
                if ( shaderDesc.m_stage == sd.m_stage )
                {
                    EE_LOG_ERROR( "Render", "Raster Pipeline Description", "Already have a shader in same pipeline stage!" );
                    EE_ASSERT(false);
                    return *this;
                }
            }

            this->m_shaderDescs.emplace( std::move( shaderDesc ) );
            return *this;
        }

        inline RasterPipelineDesc& SetRasterizerState( RasterizerState rasterizerState )
        {
            EE_ASSERT( rasterizerState.IsValid() );
            this->m_rasterizerState = rasterizerState;
            return *this;
        }

        inline RasterPipelineDesc& SetBlendState( BlendState blendState )
        {
            EE_ASSERT( blendState.IsValid() );
            this->m_blendState = blendState;
            return *this;
        }

        inline RasterPipelineDesc& SetTriangleTopology( Topology topology )
        {
            this->m_triangleTopology = topology;
            return *this;
        }

        inline RasterPipelineDesc& DepthTest( bool enable )
        {
            this->m_depthTest = enable;
            return *this;
        }

        inline RasterPipelineDesc& DepthWrite( bool enable )
        {
            this->m_depthWrite = enable;
            return *this;
        }

        inline bool IsValid() const
        {
            return !m_shaderDescs.empty() && m_pRenderpass.IsValid();
        }

    public:

        size_t GetHashCode() const
        {
            // TODO: use combine hash algorithm
            size_t hash = 482441568449451;
            for ( auto const& shaderDesc : m_shaderDescs )
            {
                hash &= EE::Hash::GetHash64( shaderDesc.m_shaderPath.c_str() );
                hash &= EE::Hash::GetHash64( shaderDesc.m_entryName );
            }
            return hash;
        }

        friend bool operator==( RasterPipelineDesc const& lhs, RasterPipelineDesc const& rhs )
        {
            // TODO: this is not right for pipelines which share same set of shaders but have different pipeline states.
            return lhs.GetHashCode() == rhs.GetHashCode();
        }

    public:

        TSet<PipelineShaderDesc>                        m_shaderDescs;
        RenderpassHandle                                m_pRenderpass;
        // TODO: refactor this, since it contain unneccesary data.
        RasterizerState                                 m_rasterizerState;
        BlendState                                      m_blendState;

        Topology                                        m_triangleTopology = Topology::TriangleList;

        bool                                            m_depthTest = true;
        bool                                            m_depthWrite = true;
    };

    class VertexShader;
    class PixelShader;
    class GeometryShader;
    class Shader;

    struct RasterPipelineState
    {
        inline void Clear()
        {
            m_pVertexShader = nullptr;
            m_pGeometryShader = nullptr;
            m_pHullShader = nullptr;
            m_pPixelShader = nullptr;
            m_pBlendState = nullptr;
            m_pRasterizerState = nullptr;
        }

    public:

        VertexShader*                   m_pVertexShader = nullptr;
        GeometryShader*                 m_pGeometryShader = nullptr;
        Shader*                         m_pHullShader = nullptr;
        PixelShader*                    m_pPixelShader = nullptr;

        BlendState*                     m_pBlendState = nullptr;
        RasterizerState*                m_pRasterizerState = nullptr;
    };

    struct ComputePipelineDesc
    {

    };

    class ComputeShader;

    struct ComputePipelineState
    {
        inline void Clear()
        {
            m_pComputeShader = nullptr;
        }

    public:

        ComputeShader*              m_pComputeShader = nullptr;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template<>
    struct hash<EE::Render::PipelineShaderDesc>
    {
        eastl_size_t operator()( EE::Render::PipelineShaderDesc const& pipelineShaderDesc ) const
        {
            // TODO: use combine hash algorithm
            eastl_size_t hash = 884512945412378;
            hash &= eastl::hash<uint8_t>()( static_cast<uint8_t>( pipelineShaderDesc.m_stage ) );
            hash &= EE::Hash::GetHash64( pipelineShaderDesc.m_shaderPath.c_str() );
            hash &= EE::Hash::GetHash64( pipelineShaderDesc.m_entryName );
            return hash;
        }
    };

    template<>
    struct hash<EE::Render::RasterPipelineDesc>
    {
        EE_FORCE_INLINE eastl_size_t operator()( EE::Render::RasterPipelineDesc const& pipelineDesc ) const
        {
            return pipelineDesc.GetHashCode();
        }
    };
}