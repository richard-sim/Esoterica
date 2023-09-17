#pragma once

#include "Base/Types/Arrays.h"
#include "Base/Types/BitFlags.h"
#include "Base/Types/Set.h"
#include "Base/Render/RenderAPI.h"
#include "Base/Render/RenderShader.h"
#include "Base/Resource/ResourcePath.h"
#include "Base/Resource/ResourceTypeID.h"

namespace EE::RHI
{
    static constexpr size_t MAX_DESCRIPTOR_SET_COUNT = 4;

    //-------------------------------------------------------------------------

    enum class ERenderResourceMemoryUsage
    {
        CPUToGPU,
        GPUToCPU,
        CPUOnly,
        GPUOnly,
        CPUCopy,
        GPULazily,
    };

    enum class ERenderResourceMemoryFlag : uint8_t
    {
        // For large memory allocation.
        DedicatedMemory = 0,
        // Mapping GPU memory to CPU persistently.
        PersistentMapping,
    };

    //-------------------------------------------------------------------------

    enum class EPixelFormat
    {
        RGBA8Unorm,
        BGRA8Unorm,
    };

    enum class ETextureType
    {
        T1D,
        T1DArray,
        T2D,
        T2DArray,
        T3D,
        TCubemap
    };

    enum class ETextureSampleCount : uint8_t
    {
        SC1 = 0,
        SC2,
        SC4,
        SC8,
        SC16,
        SC32,
        SC64,
    };

    enum class ETextureUsage : uint8_t
    {
        TransferSrc = 0,
        TransferDst,
        Sampled,
        Storage,
        Color,
        DepthStencil,
        Transient,
        Input,
    };

    enum class ETextureMemoryTiling
    {
        Optimal,
        Linear,
    };

    enum class ETextureCreateFlag : uint8_t
    {
        CubeCompatible = 0,
    };

    struct RHITextureCreateDesc
    {
    public:

        static RHITextureCreateDesc New1D( uint32_t width, EPixelFormat format );
        static RHITextureCreateDesc New1DArray( uint32_t width, EPixelFormat format, uint32_t array );
        static RHITextureCreateDesc New2D( uint32_t width, uint32_t height, EPixelFormat format );
        static RHITextureCreateDesc New2DArray( uint32_t width, uint32_t height, EPixelFormat format, uint32_t array );
        static RHITextureCreateDesc New3D( uint32_t width, uint32_t height, uint32_t depth, EPixelFormat format );
        static RHITextureCreateDesc NewCubemap( uint32_t width, EPixelFormat format );

        bool IsValid() const;

    private:

        static RHITextureCreateDesc GetDefault();

    public:

        uint32_t						        m_width;
        uint32_t						        m_height;
        uint32_t						        m_depth;

        uint32_t						        m_array;
        uint16_t						        m_mipmap;

        EPixelFormat                            m_format;
        TBitFlags<ETextureUsage>                m_usage;
        ETextureMemoryTiling			        m_tiling;
        TBitFlags<ETextureSampleCount>	        m_sample;
        ETextureType                            m_type;
        TBitFlags<ETextureCreateFlag>           m_flag;

        // Actually allocated size in byte. (May be due to alignment of certain type of texture usage)
        uint32_t                                m_allocatedSize;
        ERenderResourceMemoryUsage              m_memoryUsage;
        TBitFlags<ERenderResourceMemoryFlag>    m_memoryFlag;
    };

    //-------------------------------------------------------------------------

    enum class EBufferUsage : uint8_t
    {
        TransferSrc = 0,
        TransferDst,
        UniformTexel,
        StorageTexel,
        Uniform,
        Storage,
        Index,
        Vertex,
        Indirect,
        ShaderDeviceAddress,
    };

    struct RHIBufferCreateDesc
    {
    public:

        static RHIBufferCreateDesc NewSize( uint32_t sizeInByte );
        static RHIBufferCreateDesc NewAlignedSize( uint32_t sizeInByte, uint32_t alignment );
        static RHIBufferCreateDesc NewDeviceAddressable( uint32_t sizeInByte );
        static RHIBufferCreateDesc NewVertexBuffer( uint32_t sizeInByte );
        static RHIBufferCreateDesc NewIndexBuffer( uint32_t sizeInByte );

        bool IsValid() const;

    public:

        // User requested size in byte.
        uint32_t                                m_desireSize;
        // The alignment of allocated memory in the GPU.
        TBitFlags<EBufferUsage>                 m_usage;

        // Actually allocated size in byte. (May be due to alignment of certain type of buffer usage)
        uint32_t                                m_allocatedSize;
        ERenderResourceMemoryUsage              m_memoryUsage;
        TBitFlags<ERenderResourceMemoryFlag>    m_memoryFlag;
    };

    //-------------------------------------------------------------------------

    enum class RHIPipelineType
    {
        Raster,
        Compute,
        Transfer
    };

    class RHIRenderpass;

    struct RHIPipelineRasterizerState
    {
        Render::FillMode                m_fillMode = Render::FillMode::Solid;
        Render::CullMode                m_cullMode = Render::CullMode::BackFace;
        Render::WindingMode             m_WindingMode = Render::WindingMode::CounterClockwise;
        bool                            m_enableScissorCulling = false;
    };

    struct RHIPipelineBlendState
    {
        Render::BlendValue              m_srcValue = Render::BlendValue::One;
        Render::BlendValue              m_dstValue = Render::BlendValue::Zero;
        Render::BlendOp                 m_blendOp = Render::BlendOp::Add;
        Render::BlendValue              m_srcAlphaValue = Render::BlendValue::One;
        Render::BlendValue              m_dstAlphaValue = Render::BlendValue::Zero;
        Render::BlendOp                 m_blendOpAlpha = Render::BlendOp::Add;
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

    struct EE_BASE_API RHIPipelineShader
    {
        RHIPipelineShader( ResourcePath shaderPath, String entryName = "main" );

        inline RHIPipelineShader& SetShaderPath( ResourcePath shaderPath )
        {
            EE_ASSERT( shaderPath.IsValid() && shaderPath.IsFile() );
            char const* pExtension = shaderPath.GetExtension();
            EE_ASSERT( strlen(pExtension) == 4 ); // Extension is a TypeFourCC

            if ( ResourceTypeID( pExtension ) == Render::VertexShader::GetStaticResourceTypeID() )
            {
                this->m_stage = Render::PipelineStage::Vertex;
            }
            else if ( ResourceTypeID( pExtension ) == Render::PixelShader::GetStaticResourceTypeID() )
            {
                this->m_stage = Render::PipelineStage::Pixel;
            }
            else if( ResourceTypeID( pExtension ) == Render::GeometryShader::GetStaticResourceTypeID() )
            {
                this->m_stage = Render::PipelineStage::Geometry;
            }
            else if ( ResourceTypeID( pExtension ) == Render::ComputeShader::GetStaticResourceTypeID() )
            {
                this->m_stage = Render::PipelineStage::Compute;
            }

            this->m_shaderPath = shaderPath;
            return *this;
        }

        inline RHIPipelineShader& SetEntryName( String entryName )
        {
            EE_ASSERT( !entryName.empty() );
            this->m_entryName = entryName;
            return *this;
        }

        inline bool IsValid() const
        {
            return m_stage != Render::PipelineStage::None && m_shaderPath.IsValid() && m_shaderPath.IsFile() && !m_entryName.empty();
        }

    public:

        friend bool operator<( RHIPipelineShader const& lhs, RHIPipelineShader const& rhs )
        {
            return static_cast<uint8_t>( lhs.m_stage ) < static_cast<uint8_t>( rhs.m_stage );
        }

    public:

        Render::PipelineStage   m_stage = Render::PipelineStage::Vertex;
        ResourcePath            m_shaderPath;
        String                  m_entryName = "main";
    };

    struct RHIRasterPipelineStateCreateDesc
    {
        inline RHIRasterPipelineStateCreateDesc& AddShader( RHIPipelineShader&& pipelineShader )
        {
            EE_ASSERT( pipelineShader.IsValid() );

            for ( RHIPipelineShader const& ps : this->m_pipelineShaders )
            {
                if ( pipelineShader.m_stage == ps.m_stage )
                {
                    // Do override
                    this->m_pipelineShaders.erase( ps );
                    this->m_pipelineShaders.emplace( std::move( pipelineShader ) );
                    return *this;
                }
            }

            this->m_pipelineShaders.emplace( std::move( pipelineShader ) );
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& AddShader( ResourcePath shaderPath, String entryName = "main" )
        {
            RHIPipelineShader pipelineShader( shaderPath, entryName );
            EE_ASSERT( pipelineShader.IsValid() );

            for ( RHIPipelineShader const& ps : this->m_pipelineShaders )
            {
                if ( pipelineShader.m_stage == ps.m_stage )
                {
                    // Do override
                    this->m_pipelineShaders.erase( ps );
                    this->m_pipelineShaders.emplace( std::move( pipelineShader ) );
                    return *this;
                }
            }

            this->m_pipelineShaders.emplace( std::move( pipelineShader ) );
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& SetRasterizerState( RHIPipelineRasterizerState rasterizerState )
        {
            this->m_rasterizerState = rasterizerState;
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& SetBlendState( RHIPipelineBlendState blendState )
        {
            this->m_blendState = blendState;
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& SetTriangleTopology( ERHIPipelinePirmitiveTopology topology )
        {
            this->m_primitiveTopology = topology;
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& DepthTest( bool enable )
        {
            this->m_enableDepthTest = enable;
            return *this;
        }

        inline RHIRasterPipelineStateCreateDesc& DepthWrite( bool enable )
        {
            this->m_enableDepthWrite = enable;
            return *this;
        }

        inline bool IsValid() const
        {
            return !m_pipelineShaders.empty() && m_pRenderpass != nullptr;
        }

    public:

        size_t GetHashCode() const
        {
            // TODO: use combine hash algorithm
            size_t hash = 482441568449451;
            for ( auto const& pipelineShader : m_pipelineShaders )
            {
                hash &= EE::Hash::GetHash64( pipelineShader.m_shaderPath.c_str() );
                hash &= EE::Hash::GetHash64( pipelineShader.m_entryName );
            }
            return hash;
        }

        friend bool operator==( RHIRasterPipelineStateCreateDesc const& lhs, RHIRasterPipelineStateCreateDesc const& rhs )
        {
            // TODO: this is not right for pipelines which share same set of shaders but have different pipeline states.
            return lhs.GetHashCode() == rhs.GetHashCode();
        }

    public:

        TSet<RHIPipelineShader>         m_pipelineShaders;
        RHIRenderpass*                  m_pRenderpass;
        RHIPipelineRasterizerState      m_rasterizerState;
        RHIPipelineBlendState           m_blendState;
        ERHIPipelinePirmitiveTopology   m_primitiveTopology;

        bool                            m_enableDepthTest = false;
        bool                            m_enableDepthWrite = false;
    };

    //-------------------------------------------------------------------------

    struct RHIShaderCreateDesc
    {
    public:

        inline bool IsValid() const { return !m_byteCode.empty(); }

    public:

        // Avoid copy, but be careful with the life time.
        Blob const&                 m_byteCode; // Compiled shader byte code.
    };

    // Synchronization Primitives
    //-------------------------------------------------------------------------

    struct RHISemaphoreCreateDesc
    {
    public:

        inline bool IsValid() const { return true; }

    public:

    };
}

// Hash
//-------------------------------------------------------------------------

namespace eastl
{
    template<>
    struct hash<EE::RHI::RHIPipelineShader>
    {
        eastl_size_t operator()( EE::RHI::RHIPipelineShader const& pipelineShader ) const
        {
            // TODO: use combine hash algorithm
            eastl_size_t hash = 884512945412378;
            hash &= eastl::hash<uint8_t>()( static_cast<uint8_t>( pipelineShader.m_stage ) );
            hash &= EE::Hash::GetHash64( pipelineShader.m_shaderPath.c_str() );
            hash &= EE::Hash::GetHash64( pipelineShader.m_entryName );
            return hash;
        }
    };

    template<>
    struct hash<EE::RHI::RHIRasterPipelineStateCreateDesc>
    {
        EE_FORCE_INLINE eastl_size_t operator()( EE::RHI::RHIRasterPipelineStateCreateDesc const& pipelineDesc ) const
        {
            return pipelineDesc.GetHashCode();
        }
    };
}