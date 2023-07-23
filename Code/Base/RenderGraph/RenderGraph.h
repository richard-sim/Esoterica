#pragma once

#include "Base/_Module/API.h"

#include "RenderGraphNode.h"
#include "Base/Threading/Threading.h"
#include "Base/Types/Arrays.h"
#include "Base/Types/String.h"
#include "Base/Render/RenderPipelineState.h"
#include "Base/Render/RenderResourceBarrier.h"

namespace EE
{
	namespace RG
	{
		class RGNodeBuilder;

		template <typename Tag>
		class RGHandle
		{
			friend class RenderGraph;
			friend class RGNodeBuilder;

			EE_STATIC_ASSERT( ( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value ), "Invalid render graph resource tag!");

		public:

			typedef typename Tag::DescType DescType;

		public:

			inline DescType const& GetDesc() const
			{
				return m_desc;
			}

		private:

			inline void Expire()
			{
				m_slotID.Expire();
			}

		private:

			DescType						m_desc;
			RGResourceSlotID				m_slotID;
		};

		class EE_BASE_API RenderGraph
		{

			friend class RGNodeBuilder;

		public:

			RenderGraph();
			RenderGraph( String const& graphName );

		public:

			// render graph resource
			//-------------------------------------------------------------------------

			template <typename DescType, typename RTTag = typename DescType::RGResourceTypeTag>
			RGHandle<RTTag> CreateResource( DescType const& desc );

			// render graph node
			//-------------------------------------------------------------------------

			[[nodiscard]] RGNodeBuilder AddNode( String const& nodeName );

			#if EE_DEVELOPMENT_TOOLS
			void LogGraphNodes() const;
			#endif

		private:

			template <typename RGDescType, typename RGDescCVType = typename std::add_lvalue_reference_t<std::add_const_t<RGDescType>>>
			RGResourceSlotID CreateResourceImpl( RGDescCVType rgDesc );

		private:

			String									m_name;

			// TODO: use a real graph
			TVector<RGNode>							m_graph;
			TVector<RGResource>						m_graphResources;
		};

		// Helper class to build a render graph node.
		// User can register pipeline and define the resource usage in a certain pass.
		// TODO: [Safety Consideration] It is thread safe?
		class EE_BASE_API RGNodeBuilder
		{
		public:

			RGNodeBuilder( RenderGraph const& graph, RGNode& node );

		public:

			// node render pipeline registration
			//-------------------------------------------------------------------------
			
			void RegisterRasterPipeline( RasterPipelineDesc pipelineDesc );
			void RegisterComputePipeline( ComputePipelineDesc pipelineDesc );

			// Node resource read and write operations
			//-------------------------------------------------------------------------
			template <typename Tag>
			RGNodeResourceRef<Tag, RGResourceViewType::SRV> CommonRead( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access );

			template <typename Tag>
			RGNodeResourceRef<Tag, RGResourceViewType::UAV> CommonWrite( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access );

			template <typename Tag>
			RGNodeResourceRef<Tag, RGResourceViewType::SRV> RasterRead( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access );

			template <typename Tag>
			RGNodeResourceRef<Tag, RGResourceViewType::RT> RasterWrite( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access );

		private:

			template <typename Tag, RGResourceViewType RVT>
			RGNodeResourceRef<Tag, RVT> ReadImpl( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access );

			template <typename Tag, RGResourceViewType RVT>
			RGNodeResourceRef<Tag, RVT> WriteImpl( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access );

		private:

			// Safety: All reference holds during the life time of RGNodeBuilder.
			RenderGraph const&						m_graph;
			RGNode&									m_node;
		};

		//-------------------------------------------------------------------------

		template <typename DescType, typename RTTag>
		RGHandle<RTTag> RenderGraph::CreateResource( DescType const& desc )
		{
			static_assert( std::is_base_of<RGResourceTypeBase<RTTag>, RTTag>::value, "Invalid render graph resource tag!" );
			typedef typename RTTag::RGDescType RGDescType;

			EE_ASSERT( Threading::IsMainThread() );

			RGDescType rgDesc = {};
			rgDesc.m_desc = desc;

			RGResourceSlotID const id = CreateResourceImpl<RGDescType>( rgDesc );
			RGHandle<RTTag> handle;
			handle.m_slotID = id;
			handle.m_desc = desc;
			return handle;
		}

		//-------------------------------------------------------------------------
	
		template <typename RGDescType, typename RGDescCVType>
		RGResourceSlotID RenderGraph::CreateResourceImpl( RGDescCVType rgDesc )
		{
			size_t slotID = m_graphResources.size();
			EE_ASSERT( slotID >= 0 && slotID < std::numeric_limits<uint32_t>::max() );

			RGResourceSlotID id( static_cast<uint32_t>( slotID ) );
			m_graphResources.emplace_back( rgDesc );
			return id;
		}

		//-------------------------------------------------------------------------

		template <typename Tag>
		RGNodeResourceRef<Tag, RGResourceViewType::SRV> RGNodeBuilder::CommonRead( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access )
		{
			// TODO: We should make a runtime check.
			EE_ASSERT( IsCommonReadOnlyAccess( access ) );
			EE_ASSERT( Threading::IsMainThread() );
			return ReadImpl<Tag, RGResourceViewType::SRV>( pResource, access );
		}

		template <typename Tag>
		RGNodeResourceRef<Tag, RGResourceViewType::UAV> RGNodeBuilder::CommonWrite( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access )
		{
			// TODO: We should make a runtime check.
			EE_ASSERT( IsCommonWriteAccess( access ) );
			EE_ASSERT( Threading::IsMainThread() );
			return WriteImpl<Tag, RGResourceViewType::UAV>( pResource, access );
		}

		template <typename Tag>
		RGNodeResourceRef<Tag, RGResourceViewType::SRV> RGNodeBuilder::RasterRead( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access )
		{
			// TODO: We should make a runtime check.
			EE_ASSERT( IsRasterReadOnlyAccess( access ) );
			EE_ASSERT( Threading::IsMainThread() );
			return ReadImpl<Tag, RGResourceViewType::SRV>( pResource, access );
		}

		template <typename Tag>
		RGNodeResourceRef<Tag, RGResourceViewType::RT> RGNodeBuilder::RasterWrite( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access )
		{
			// TODO: We should make a runtime check.
			EE_ASSERT( IsRasterWriteAccess( access ) );
			EE_ASSERT( Threading::IsMainThread() );
			return WriteImpl<Tag, RGResourceViewType::RT>( pResource, access );
		}

		//-------------------------------------------------------------------------

		template <typename Tag, RGResourceViewType RVT>
		RGNodeResourceRef<Tag, RVT> RGNodeBuilder::ReadImpl( RGHandle<Tag> const& pResource, Render::RenderResourceBarrierState access )
		{
			EE_STATIC_ASSERT( ( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value ), "Invalid render graph resource tag!" );

			typedef typename Tag::DescType DescType;
			typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

			EE_ASSERT( pResource.m_slotID.IsValid() );

			// Note: only true to skip sync for read access
			EE::Render::RenderResourceAccessState accessState( access, true );
			m_node.m_pInputs.emplace_back( pResource.m_slotID, std::move( accessState ) );

			// fetch graph resource from render graph
			DescCVType desc = m_graph.m_graphResources[pResource.m_slotID.m_id].GetDesc<Tag>();
			// return a life time limited reference to it
			return RGNodeResourceRef<Tag, RVT>( desc, pResource.m_slotID );
		}

		template <typename Tag, RGResourceViewType RVT>
		RGNodeResourceRef<Tag, RVT> RGNodeBuilder::WriteImpl( RGHandle<Tag>& pResource, Render::RenderResourceBarrierState access )
		{
			EE_STATIC_ASSERT( ( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value ), "Invalid render graph resource tag!" );

			typedef typename Tag::DescType DescType;
			typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

			EE_ASSERT( pResource.m_slotID.IsValid() );

			// Note: only true to skip sync for read access
			EE::Render::RenderResourceAccessState accessState( access, false );
			m_node.m_pOutputs.emplace_back( pResource.m_slotID, std::move( accessState ) );

			// fetch graph resource from render graph
			DescCVType desc = m_graph.m_graphResources[pResource.m_slotID.m_id].GetDesc<Tag>();

			// After write operation, this resource consider as new resource
			RGResourceSlotID newSlotID = pResource.m_slotID;
			newSlotID.Expire();

			// return a life time limited reference to it
			return RGNodeResourceRef<Tag, RVT>( desc, pResource.m_slotID );
		}
	}
}