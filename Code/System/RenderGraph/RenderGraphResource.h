#pragma once

#include "System/_Module/API.h"

#include "System/Types/Variant.h"
#include "System/Memory/Pointers.h"
#include "System/Render/RenderAPI.h"
#include "RenderGraphResourceBarrier.h"

#include <limits>

//-------------------------------------------------------------------------
//	Render graph resource lifetime.
// 
//	primarily consists of two stage:
// 
//	1. Transient. Resource which used inside a specific render graph, it is invalid
//	to direct used it again in next time execution. This type of resources are dynamic,
//	means that they will be created during every execution and restore by render graph 
//	at the end of the execution. On next time execution, this type of resource may 
//	_NOT_ be created.
// 
//	2. Exportable. Resource which can be used when render graph execution is complete.
//	This type of resource can be imported by another render graph, or just simply store
//	and managed by the user.
//-------------------------------------------------------------------------

namespace EE
{
	namespace RG
	{
		using namespace Render;

		enum class RGResourceType : uint8_t
		{
			Buffer = 0,
			Texture,
		};

		class EE_SYSTEM_API RGResourceSlotID
		{
		public:

			RGResourceSlotID() = default;
			RGResourceSlotID( uint32_t id );

			inline bool IsValid() const { return m_id != std::numeric_limits<uint32_t>::max(); }

			inline bool operator==( RGResourceSlotID const& rhs ) const { return m_id == rhs.m_id; }
			inline bool operator!=( RGResourceSlotID const& rhs ) const { return m_id != rhs.m_id; }

		private:

			uint32_t				m_id = std::numeric_limits<uint32_t>::max();
		};

		class RGBufferDesc;
		class RGTextureDesc;

		struct BufferDesc
		{
		public:

			typedef RGBufferDesc RGDescType;

		public:

			uint32_t		m_count;
		};

		struct TextureDesc
		{
		public:

			typedef RGTextureDesc RGDescType;

		public:

			bool			m_isEnable;
			float			m_factor;
		};

		template <typename SelfType, typename DescType = typename SelfType::DescType>
		struct RGResourceDesc
		{
			inline DescType const& GetDesc() const
			{
				return reinterpret_cast<SelfType const&>( *this ).GetDesc();
			}
		};

		class EE_SYSTEM_API RGBufferDesc final : public RGResourceDesc<RGBufferDesc, BufferDesc>
		{
		public:

			typedef BufferDesc DescType;

			// TODO: static assert this.
			inline constexpr static RGResourceType ResourceType = RGResourceType::Buffer;

		public:

			inline DescType const& GetDesc() const { return m_desc; }

		private:

			friend class RenderGraph;

			DescType				m_desc;
		};

		class EE_SYSTEM_API RGTextureDesc final : public RGResourceDesc<RGTextureDesc, TextureDesc>
		{
		public:

			typedef TextureDesc DescType;

			inline constexpr static RGResourceType ResourceType = RGResourceType::Texture;

		public:

			inline DescType const& GetDesc() const { return m_desc; }

		private:

			friend class RenderGraph;

			DescType				m_desc;
		};

		template <uint8_t I>
		struct RGResourceTypeToDescType
		{};

		// TODO: use macro to auto expand
		template <>
		struct RGResourceTypeToDescType<static_cast<uint8_t>( RGResourceType::Buffer )>
		{
			typedef typename RGBufferDesc::DescType DescType;
		};

		template <>
		struct RGResourceTypeToDescType<static_cast<uint8_t>( RGResourceType::Texture )>
		{
			typedef typename RGTextureDesc::DescType DescType;
		};

		enum class RGResourceViewType
		{
			SRV, // shader resource view
			UAV, // unorder access view
			RT   // render target
		};

		struct RGLazyCreateResource
		{
		};

		struct RGImportedResource
		{
			TSharedPtr<void*>						m_pOuterResource;
			RGResourceBarrierState					m_currentAccess;
		};

		typedef RGResourceDesc<RGBufferDesc> BufferDescBaseType;
		typedef RGResourceDesc<RGTextureDesc> TextureDescBaseType;

		class EE_SYSTEM_API RGResource
		{
		public:

			RGResource( BufferDescBaseType bufferDesc );
			RGResource( TextureDescBaseType textureDesc );

		private:

			TVariant<RGLazyCreateResource, RGImportedResource>		m_resource;
			// TODO: compile-time enum element expand (use macro?)
			TVariant<
				BufferDescBaseType,
				TextureDescBaseType
			>														m_desc;
		};
	}
}