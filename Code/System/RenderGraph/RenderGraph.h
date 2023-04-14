#pragma once

#include "System/_Module/API.h"

#include "RenderGraphNode.h"
#include "System/Types/Arrays.h"
#include "System/Types/String.h"

namespace EE
{
	namespace RG
	{
		class EE_SYSTEM_API RenderGraph
		{
		public:

			RenderGraph();
			RenderGraph( String const& graphName );

		public:

			// render graph resource
			//-------------------------------------------------------------------------

			template <typename DescType, typename RGDescType = DescType::RGDescType, RGResourceType RT = RGDescType::ResourceType>
			RGHandle<RT> CreateResource( DescType const& desc );

			// render graph node
			//-------------------------------------------------------------------------

			RGNode& AddNode( String const& nodeName );

			#if EE_DEVELOPMENT_TOOLS
			void LogGraphNodes() const;
			#endif

		private:

			template <typename RGDescType>
			RGResourceSlotID CreateResourceImpl( RGResourceDesc<RGDescType> const& rgDesc );

		private:

			String									m_name;

			// TODO: use a real graph
			TVector<RGNode>							m_graph;
			TVector<RGResource>						m_graphResources;
		};

		//-------------------------------------------------------------------------

		template <typename DescType, typename RGDescType, RGResourceType RT>
		RGHandle<RT> RenderGraph::CreateResource( DescType const& desc )
		{
			RGDescType rgDesc = {};
			rgDesc.m_desc = desc;

			RGResourceSlotID const id = CreateResourceImpl( rgDesc );
			RGHandle<RT> handle;
			handle.m_slotID = id;
			handle.m_desc = desc;
			return handle;
		}

		//-------------------------------------------------------------------------
	
		template <typename RGDescType>
		RGResourceSlotID RenderGraph::CreateResourceImpl( RGResourceDesc<RGDescType> const& rgDesc )
		{
			size_t slotID = m_graphResources.size();
			EE_ASSERT( slotID >= 0 && slotID < std::numeric_limits<uint32_t>::max() );

			RGResourceSlotID id( static_cast<uint32_t>( slotID ) );
			m_graphResources.emplace_back( rgDesc );
			return id;
		}
	}
}