#pragma once

#include <EASTL/numeric_limits.h>

// helper macro

#define EE_RHI_STATIC_TAGGED_TYPE( rhiType ) inline static EE::RHI::ERHIType GetStaticRHIType() { return rhiType; }

//-------------------------------------------------------------------------

namespace EE::RHI
{
    enum class ERHIType : uint8_t
    {
        Vulkan = 0,
        DX11,

        Invalid = eastl::numeric_limits<uint8_t>::max()
    };

    class RHITaggedType
    {
    public:

        RHITaggedType( ERHIType dynamicRhiType )
            : m_DynamicRHIType( dynamicRhiType )
        {}
        virtual ~RHITaggedType() = default;

        inline ERHIType GetDynamicRHIType() const { return m_DynamicRHIType; }

    protected:

        ERHIType                m_DynamicRHIType = ERHIType::Invalid;
    };
}