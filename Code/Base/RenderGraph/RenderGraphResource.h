#pragma once

#include "Base/_Module/API.h"

#include "Base/Types/Variant.h"
#include "Base/Memory/Pointers.h"
#include "Base/Render/RenderAPI.h"
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
			Texture = 1,
			Unknown = std::numeric_limits<uint8_t>::max(),
		};

		// TODO: put this inside impl namespace
		class EE_BASE_API RGResourceSlotID
		{
		public:

			RGResourceSlotID() = default;
			RGResourceSlotID( uint32_t id );

			inline bool IsValid() const { return m_id != std::numeric_limits<uint32_t>::max(); }

			inline bool operator==( RGResourceSlotID const& rhs ) const { return m_id == rhs.m_id && m_generation == rhs.m_generation; }
			inline bool operator!=( RGResourceSlotID const& rhs ) const { return m_id != rhs.m_id && m_generation != rhs.m_generation; }

			inline void Expire()
			{ 
				if ( m_generation >= std::numeric_limits<uint32_t>::max() )
					m_generation = 0;
				else
					m_generation += 1;
			}

		private:

			friend class RGNodeBuilder;

			uint32_t				m_id = std::numeric_limits<uint32_t>::max();
			uint32_t				m_generation = 0;
		};

		class RGBufferDesc;
		class RGTextureDesc;

		struct RGBuffer;
		struct RGTexture;

		struct BufferDesc
		{
		public:

			typedef RGBufferDesc RGDescType;
			typedef RGBuffer RGResourceTypeTag;

		public:

			uint32_t		m_count;
		};

		struct TextureDesc
		{
		public:

			typedef RGTextureDesc RGDescType;
			typedef RGTexture RGResourceTypeTag;

		public:

			bool			m_isEnable;
			float			m_factor;
		};

		template <typename SelfType, typename DescType = typename SelfType::DescType>
		struct RGResourceDesc
		{
			using SelfCVType = typename std::add_lvalue_reference_t<std::add_const_t<SelfType>>;
			using DescCVType = typename std::add_lvalue_reference_t<std::add_const_t<DescType>>;

			inline DescCVType GetDesc() const
			{
				return reinterpret_cast<SelfCVType>( *this ).GetDesc();
			}
		};

		class EE_BASE_API RGBufferDesc final : public RGResourceDesc<RGBufferDesc, BufferDesc>
		{
		public:

			typedef BufferDesc DescType;
			typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

		public:

			inline DescCVType GetDesc() const { return m_desc; }

		private:

			friend class RenderGraph;

			DescType				m_desc;
		};

		class EE_BASE_API RGTextureDesc final : public RGResourceDesc<RGTextureDesc, TextureDesc>
		{
		public:

			typedef TextureDesc DescType;
			typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

		public:

			inline DescCVType GetDesc() const { return m_desc; }

		private:

			friend class RenderGraph;

			DescType				m_desc;
		};

		template <typename Tag>
		struct RGResourceTypeBase
		{
			constexpr inline static RGResourceType GetRGResourceType()
			{
				return Tag::GetRGResourceType();
			}
		};

		struct RGBuffer : public RGResourceTypeBase<RGBuffer>
		{
			typedef RGBufferDesc RGDescType;
			typedef BufferDesc DescType;

			constexpr inline static RGResourceType GetRGResourceType()
			{
				return RGResourceType::Buffer;
			}
		};

		struct RGTexture : public RGResourceTypeBase<RGTexture>
		{
			typedef RGTextureDesc RGDescType;
			typedef TextureDesc DescType;

			constexpr inline static RGResourceType GetRGResourceType()
			{
				return RGResourceType::Texture;
			}
		};

		//EE_STATIC_ASSERT( sizeof( RGBuffer ) == 0, "Size of tag RGBuffer should be zero!");
		//EE_STATIC_ASSERT( sizeof( RGTexture ) == 0, "Size of tag RGBuffer should be zero!" );

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

		//typedef RGResourceDesc<RGBufferDesc> BufferDescBaseType;
		//typedef RGResourceDesc<RGTextureDesc> TextureDescBaseType;

		class EE_BASE_API RGResource
		{
		public:

			RGResource( RGBufferDesc const& bufferDesc );
			RGResource( RGTextureDesc const& textureDesc );

		public:

			template <typename Tag,
				typename DescType = typename Tag::DescType,
				typename DescCVType = typename std::add_lvalue_reference_t<std::add_const_t<DescType>>
			>
			DescCVType GetDesc() const;

		private:

			// TODO: compile-time enum element expand (use macro?)
			TVariant<
				RGBufferDesc,
				RGTextureDesc
			>														m_desc;
			TVariant<RGLazyCreateResource, RGImportedResource>		m_resource;
		};

		template <typename Tag, typename DescType, typename DescCVType>
		DescCVType RGResource::GetDesc() const
		{
			static_assert( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value, "Invalid render graph resource tag!" );

			constexpr uint8_t const index = static_cast<uint8_t>( Tag::GetRGResourceType() );
			return eastl::get<index>( m_desc ).GetDesc();
		}
	}
}