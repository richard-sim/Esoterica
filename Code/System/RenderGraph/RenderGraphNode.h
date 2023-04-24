#pragma once

#include "System/_Module/API.h"

#include "RenderGraphResource.h"
#include "System/Render/RenderResourceBarrier.h"
#include "System/Types/Arrays.h"
#include "System/Types/String.h"

#include <limits>

namespace EE
{
	namespace RG
	{
		class EE_SYSTEM_API RGNodeResource
		{
		public:

			RGNodeResource() = default;
			RGNodeResource( RGResourceSlotID slotID, Render::RenderResourceAccessState access );

		private:

			RGResourceSlotID							m_slotID;
			Render::RenderResourceAccessState			m_passAccess;
		};

		template <typename Tag, RGResourceViewType RVT>
		class RGNodeResourceRef
		{
			static_assert( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value, "Invalid render graph resource tag!" );

			typedef typename Tag::DescType DescType;
			typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

		public:

			RGNodeResourceRef( DescCVType desc, RGResourceSlotID slotID );

			inline DescCVType GetDesc() const { return m_desc; }

		private:

			friend class RenderGraph;

			// Safety: This reference always holded by RenderGraph during the life time of RGNodeResourceRef.
			DescCVType							m_desc;
			RGResourceSlotID					m_slotID;
		};

		typedef uint32_t NodeID;

		class EE_SYSTEM_API RGNode
		{
		public:

			RGNode();
			RGNode( String const& nodeName, NodeID id );

		private:

			friend class RenderGraph;
			friend class RGNodeBuilder;

			TVector<RGNodeResource>					m_pInputs;
			TVector<RGNodeResource>					m_pOutputs;

			String									m_passName;
			NodeID									m_id;
		};

		//-------------------------------------------------------------------------

		template<typename Tag, RGResourceViewType RVT>
		RGNodeResourceRef<Tag, RVT>::RGNodeResourceRef( DescCVType desc, RGResourceSlotID slotID )
			: m_desc( desc ), m_slotID( slotID )
		{}
	}
}