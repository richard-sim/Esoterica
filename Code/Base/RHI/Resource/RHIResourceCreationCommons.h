#pragma once

#include "Base/Types/Arrays.h"

namespace EE::RHI
{
    enum class EPixelFormat
    {
        RGBA8Float
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

    struct RHITextureCreateDesc
    {
    public:

        static RHITextureCreateDesc GetDefault();
        static RHITextureCreateDesc New1D( uint32_t width, EPixelFormat format );
        static RHITextureCreateDesc New1DArray( uint32_t width, EPixelFormat format, uint32_t array );
        static RHITextureCreateDesc New2D( uint32_t width, uint32_t height, EPixelFormat format );
        static RHITextureCreateDesc New2DArray( uint32_t width, uint32_t height, EPixelFormat format, uint32_t array );
        static RHITextureCreateDesc New3D( uint32_t width, uint32_t height, uint32_t depth, EPixelFormat format );
        static RHITextureCreateDesc NewCubemap( uint32_t width, EPixelFormat format );

        inline bool IsValid() const { return true; }

    public:

        uint32_t						m_width;
        uint32_t						m_height;
        uint32_t						m_depth;

        uint32_t						m_array;
        uint16_t						m_mipmap;

        //VkImageUsageFlags				m_usage;
        //VkImageTiling					m_tiling;
        //VkFormat						m_format;
        //VkImageCreateFlags			m_flags;
        //VkSampleCountFlags			m_sample;
        ETextureType                    m_type;
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