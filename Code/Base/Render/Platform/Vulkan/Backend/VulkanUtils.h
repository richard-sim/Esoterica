#pragma once
#if defined(EE_VULKAN)

#include "Base/Application/Application.h"
#include "Base/Math/Math.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

namespace EE::Render
{
	namespace Backend::Util
	{
		Int2 GetCurrentActiveWindowUserExtent( Application* pApplication );

        inline bool IsUniformBuffer( RHI::RHIBufferCreateDesc const& bufferCreateDesc )
        {
            if ( bufferCreateDesc.m_usage.AreAnyFlagsSet(
                RHI::EBufferUsage::Uniform,
                RHI::EBufferUsage::UniformTexel
                ) )
            {
                return true;
            }
            return false;
        }

        inline bool IsStorageBuffer( RHI::RHIBufferCreateDesc const& bufferCreateDesc )
        {
            if ( bufferCreateDesc.m_usage.AreAnyFlagsSet(
                RHI::EBufferUsage::Storage,
                RHI::EBufferUsage::StorageTexel
                ) )
            {
                return true;
            }
            return false;
        }
	}
}

#endif