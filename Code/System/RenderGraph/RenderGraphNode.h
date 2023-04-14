#pragma once

#include "System/_Module/API.h"

#include "RenderGraphResource.h"
#include "System/Types/Arrays.h"
#include "System/Types/String.h"

#include <limits>

namespace EE
{
	namespace RG
	{
		template <RGResourceType RT>
		class RGHandle
		{
		public:

			typedef typename RGResourceTypeToDescType<static_cast<uint8_t>( RT )>::DescType DescType;

		public:

			inline DescType const& GetDesc() const
			{
				return m_desc;
			}

		private:

			friend class RenderGraph;

			DescType						m_desc;
			RGResourceSlotID				m_slotID;
		};

		class RGNodeResource
		{
		public:

			RGNodeResource() = default;
			RGNodeResource( RGResourceSlotID slotID );

		private:

			RGResourceSlotID					m_slotID;
			RGResourceBarrierState				m_passAccess = RGResourceBarrierState::Undefined;
		};

		template <RGResourceType RT, RGResourceViewType RVT>
		class RGNodeResourceRef
		{
		public:

		private:

			friend class RenderGraph;

			RGResourceSlotID					m_slotID;
		};

		class EE_SYSTEM_API RGNode
		{
		public:

			RGNode();
			RGNode( String const& nodeName );

		public:

			template <RGResourceType RT, RGResourceViewType RVT>
			RGNodeResourceRef<RT, RVT> Read( RGHandle<RT> const& pResource );

			template <RGResourceType RT, RGResourceViewType RVT>
			RGNodeResourceRef<RT, RVT> Write( RGHandle<RT>& pResource );

		private:

			friend class RenderGraph;

			TVector<RGNodeResource>					m_pInputs;
			TVector<RGNodeResource>					m_pOutputs;

			String									m_passName;
		};

		//-------------------------------------------------------------------------
	
		template <RGResourceType RT, RGResourceViewType RVT>
		RGNodeResourceRef<RT, RVT> RGNode::Read( RGHandle<RT> const& pResource )
		{
		}

		template <RGResourceType RT, RGResourceViewType RVT>
		RGNodeResourceRef<RT, RVT> RGNode::Write( RGHandle<RT>& pResource )
		{

		}
	}
}