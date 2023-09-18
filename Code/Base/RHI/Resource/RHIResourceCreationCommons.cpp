#include "RHIResourceCreationCommons.h"

#include "Base/Math/Math.h"

namespace EE::RHI
{
    RHITextureCreateDesc RHITextureCreateDesc::GetDefault()
    {
        RHITextureCreateDesc desc = {};
        desc.m_width = 0;
        desc.m_height = 0;
        desc.m_depth = 0;

        desc.m_array = 1;
        desc.m_mipmap = 1;

        // we can infer usage by its resource barrier type, so user do not need to explicitly fill in here,
        // but we still give user choice to add usage flags if needed.
        desc.m_usage = ETextureUsage::Color;
        desc.m_tiling = ETextureMemoryTiling::Optimal;
        desc.m_format = EPixelFormat::RGBA8Unorm;
        desc.m_sample = ETextureSampleCount::SC1;
        desc.m_type = ETextureType::T2D;
        desc.m_memoryUsage = ERenderResourceMemoryUsage::GPUOnly;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::New1D( uint32_t width, EPixelFormat format )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = 1;
        desc.m_depth = 1;

        desc.m_format = format;
        desc.m_type = ETextureType::T1D;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::New1DArray( uint32_t width, EPixelFormat format, uint32_t array )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = 1;
        desc.m_depth = 1;

        desc.m_format = format;
        desc.m_type = ETextureType::T1DArray;
        desc.m_array = array;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::New2D( uint32_t width, uint32_t height, EPixelFormat format )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = height;
        desc.m_depth = 1;

        desc.m_format = format;
        desc.m_type = ETextureType::T2D;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::New2DArray( uint32_t width, uint32_t height, EPixelFormat format, uint32_t array )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = height;
        desc.m_depth = 1;

        desc.m_format = format;
        desc.m_type = ETextureType::T2DArray;
        desc.m_array = array;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::New3D( uint32_t width, uint32_t height, uint32_t depth, EPixelFormat format )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = height;
        desc.m_depth = depth;

        desc.m_format = format;
        desc.m_type = ETextureType::T3D;
        return desc;
    }

    RHITextureCreateDesc RHITextureCreateDesc::NewCubemap( uint32_t width, EPixelFormat format )
    {
        auto desc = RHITextureCreateDesc::GetDefault();
        desc.m_width = width;
        desc.m_height = width;
        desc.m_depth = 1;

        desc.m_format = format;
        desc.m_type = ETextureType::TCubemap;
        desc.m_array = 6;
        desc.m_flag = ETextureCreateFlag::CubeCompatible;
        return desc;
    }

	bool RHITextureCreateDesc::IsValid() const
	{
        bool isValid = m_height != 0
            && m_width != 0
            && m_depth != 0
            && m_mipmap != 0
            && m_array != 0;

        if ( !isValid )
        {
            return isValid;
        }

        switch ( m_type )
        {
            case ETextureType::T1D:
            break;
            case ETextureType::T1DArray:
            break;
            case ETextureType::T2D:
            break;
            case ETextureType::T2DArray:
            break;
            case ETextureType::T3D:
            break;
            case ETextureType::TCubemap:
            {
                return m_array == 6
                    && m_width == m_height
                    && m_depth == 1
                    && m_flag.IsFlagSet( ETextureCreateFlag::CubeCompatible );
            }
            break;
            default:
            break;
        }

        isValid = !(m_memoryUsage == ERenderResourceMemoryUsage::CPUCopy
            || m_memoryUsage == ERenderResourceMemoryUsage::CPUCopy
            || m_memoryUsage == ERenderResourceMemoryUsage::CPUToGPU
            || m_memoryUsage == ERenderResourceMemoryUsage::GPUToCPU);

        return isValid;
	}

    //-------------------------------------------------------------------------

    bool RHIBufferCreateDesc::IsValid() const
    {
        return m_desireSize != 0
            && m_usage.IsAnyFlagSet();
    }

    RHIBufferCreateDesc RHIBufferCreateDesc::NewSize( uint32_t sizeInByte )
    {
        EE_ASSERT( sizeInByte > 0 );

        RHIBufferCreateDesc bufferDesc = {};
        bufferDesc.m_desireSize = sizeInByte;
        bufferDesc.m_usage.SetFlag(EBufferUsage::Uniform);
        bufferDesc.m_memoryUsage = ERenderResourceMemoryUsage::CPUToGPU;
        return bufferDesc;
    }

    RHIBufferCreateDesc RHIBufferCreateDesc::NewAlignedSize( uint32_t sizeInByte, uint32_t alignment )
    {
        EE_ASSERT( sizeInByte > 0 && alignment >= 2 && Math::IsPowerOf2( alignment ) );

        uint32_t aligned = Math::RoundUpToNearestMultiple32( sizeInByte, alignment );

        RHIBufferCreateDesc bufferDesc = {};
        bufferDesc.m_desireSize = aligned;
        bufferDesc.m_usage.SetFlag( EBufferUsage::Uniform );
        bufferDesc.m_memoryUsage = ERenderResourceMemoryUsage::CPUToGPU;
        return bufferDesc;
    }

    RHIBufferCreateDesc RHIBufferCreateDesc::NewDeviceAddressable( uint32_t sizeInByte )
    {
        EE_ASSERT( sizeInByte > 0 );

        RHIBufferCreateDesc bufferDesc = {};
        bufferDesc.m_desireSize = sizeInByte;
        bufferDesc.m_usage.SetFlag( EBufferUsage::ShaderDeviceAddress );
        bufferDesc.m_memoryUsage = ERenderResourceMemoryUsage::GPUOnly;
        return bufferDesc;
    }

    RHIBufferCreateDesc RHIBufferCreateDesc::NewVertexBuffer( uint32_t sizeInByte )
    {
        EE_ASSERT( sizeInByte > 0 );

        RHIBufferCreateDesc bufferDesc = {};
        bufferDesc.m_desireSize = sizeInByte;
        bufferDesc.m_usage.SetFlag( EBufferUsage::Vertex );
        // default to vertex buffer
        bufferDesc.m_memoryUsage = ERenderResourceMemoryUsage::GPUOnly;
        return bufferDesc;
    }

    RHIBufferCreateDesc RHIBufferCreateDesc::NewIndexBuffer( uint32_t sizeInByte )
    {
        EE_ASSERT( sizeInByte > 0 );

        RHIBufferCreateDesc bufferDesc = {};
        bufferDesc.m_desireSize = sizeInByte;
        bufferDesc.m_usage.SetFlag( EBufferUsage::Index );
        // default to vertex buffer
        bufferDesc.m_memoryUsage = ERenderResourceMemoryUsage::GPUOnly;
        return bufferDesc;
    }

    //-------------------------------------------------------------------------

    RHIPipelineShader::RHIPipelineShader( ResourcePath shaderPath, String entryName )
        : m_entryName( entryName )
    {
        SetShaderPath( shaderPath );
        EE_ASSERT( m_shaderPath.IsValid() && m_shaderPath.IsFile() );
    }
}