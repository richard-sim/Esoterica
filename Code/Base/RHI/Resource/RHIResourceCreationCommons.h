#pragma once
#if defined(EE_VULKAN)

#include "Base/Types/Arrays.h"
#include "Base/Types/BitFlags.h"

namespace EE::RHI
{
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
        RGBA8Unorm
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

    struct RHIShaderCreateDesc
    {
    public:

        inline bool IsValid() const { return !m_byteCode.empty(); }

    public:

        // Avoid copy, but be careful with the life time.
        Blob const&                 m_byteCode; // Compiled shader byte code.
    };

    struct RHISemaphoreCreateDesc
    {
    public:

        inline bool IsValid() const { return true; }

    public:

    };
}

#endif