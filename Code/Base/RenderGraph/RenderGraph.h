#pragma once

#include "Base/_Module/API.h"

#include "RenderGraphContext.h"
#include "RenderGraphResourceRegistry.h"
#include "RenderGraphNodeBuilder.h"
#include "Base/Types/Arrays.h"
#include "Base/Types/String.h"
// TODO: may be decouple pipeline barrier from command buffer 
#include "Base/RHI/RHICommandBuffer.h"
#include "Base/RHI/RHIDevice.h"

namespace EE
{
	namespace RG
	{
		class EE_BASE_API RenderGraph
		{
			friend class RGNodeBuilder;
            friend class RGRenderCommandContext;

		public:

			RenderGraph();
			RenderGraph( String const& graphName );
            
            inline void AttachToPipelineRegistry( Render::PipelineRegistry& pipelineRegistry ) { m_resourceRegistry.AttachToPipelineRegistry( pipelineRegistry ); }

		public:

            // Build Stage
            //-------------------------------------------------------------------------

            void BeginFrame( RHI::RHIDevice* pRhiDevice );
            void EndFrame();

			template <typename DescType, typename RTTag = typename DescType::RGResourceTypeTag>
			RGResourceHandle<RTTag> CreateResource( DescType const& desc );

			[[nodiscard]] RGNodeBuilder AddNode( String const& nodeName );

			#if EE_DEVELOPMENT_TOOLS
			void LogGraphNodes() const;
			#endif

            // Compilation Stage
            //-------------------------------------------------------------------------

            void Compile( RHI::RHIDevice* pRhiDevice );

            // Execution Stage
            //-------------------------------------------------------------------------

            void Execute();

            // Cleanup Stage
            //-------------------------------------------------------------------------

            void ClearAllResources( RHI::RHIDevice* pRhiDevice );

		private:
        

			String									m_name;

			// TODO: use a real graph
			TVector<RGNode>							m_graph;
            RGResourceRegistry                      m_resourceRegistry;

            //TVector<RGExecutableNode>               m_executableGraph;

            // Note: this render command context will match exact the device frame index.
            RGRenderCommandContext                  m_renderCommandContexts[RHI::RHIDevice::NumDeviceFrameCount];
            size_t                                  m_currentDeviceFrameIndex;
            bool                                    m_frameExecuting = false;
		};

		//-------------------------------------------------------------------------

		template <typename DescType, typename RTTag>
		RGResourceHandle<RTTag> RenderGraph::CreateResource( DescType const& desc )
		{
			static_assert( std::is_base_of<RGResourceTagTypeBase<RTTag>, RTTag>::value, "Invalid render graph resource tag!" );
			typedef typename RTTag::RGDescType RGDescType;

			EE_ASSERT( Threading::IsMainThread() );

			RGDescType rgDesc = {};
			rgDesc.m_desc = desc;

			_Impl::RGResourceID const id = m_resourceRegistry.RegisterResource<RGDescType>( rgDesc );
			RGResourceHandle<RTTag> handle;
			handle.m_slotID = id;
			handle.m_desc = desc;
			return handle;
		}
	}
}