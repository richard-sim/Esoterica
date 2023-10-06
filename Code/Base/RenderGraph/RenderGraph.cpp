#include "RenderGraph.h"
#include "Base/Logging/Log.h"
#include "Base/Threading/Threading.h"
#include "Base/RHI/RHIDevice.h"
#include "Base/RHI/Resource/RHIBuffer.h"
#include "Base/RHI/Resource/RHITexture.h"

namespace EE
{
	namespace RG
	{
		RenderGraph::RenderGraph()
			: RenderGraph( "Default RG" )
		{}

		RenderGraph::RenderGraph( String const& graphName )
            : m_name( graphName ), m_currentDeviceFrameIndex( 0 )   
        {}

        // Build Stage
		//-------------------------------------------------------------------------
		
        void RenderGraph::BeginFrame( RHI::RHIDevice* pRhiDevice )
        {
            EE_ASSERT( !m_frameExecuting );
            EE_ASSERT( pRhiDevice != nullptr );

            m_currentDeviceFrameIndex = pRhiDevice->GetCurrentDeviceFrameIndex();
            auto& commandContext = m_renderCommandContexts[m_currentDeviceFrameIndex];

            commandContext.SetCommandContext( this, pRhiDevice, pRhiDevice->AllocateCommandBuffer() );
            if ( !commandContext.m_pCommandBuffer )
            {
                EE_LOG_ERROR( "RenderGraph", "BeginFrame", "Failed to begin render graph frame!" );
            }

            m_frameExecuting = true;
        }

        void RenderGraph::EndFrame()
        {
            EE_ASSERT( m_frameExecuting );

            auto& commandContext = m_renderCommandContexts[m_currentDeviceFrameIndex];

            // Temporary
            EE::Delete( commandContext.m_pCommandBuffer );

            commandContext.Reset();

            m_frameExecuting = false;
        }

        RGNodeBuilder RenderGraph::AddNode( String const& nodeName )
		{
            EE_ASSERT( m_frameExecuting );
			EE_ASSERT( Threading::IsMainThread() );

            auto nextID = static_cast<uint32_t>( m_graph.size() );
			auto& newNode = m_graph.emplace_back( nodeName, nextID );

			return RGNodeBuilder( m_resourceRegistry, newNode );
		}

		#if EE_DEVELOPMENT_TOOLS
		void RenderGraph::LogGraphNodes() const
		{
            EE_ASSERT( m_frameExecuting );
			EE_ASSERT( Threading::IsMainThread() );

			size_t const count = m_graph.size();

			EE_LOG_MESSAGE( "Render Graph", "Graph", "Node Count: %u", count );

			for ( size_t i = 0; i < count; ++i )
			{
				EE_LOG_MESSAGE( "Render Graph", "Graph", "\tNode (%s)", m_graph[i].m_passName.c_str() );
			}
		}
        #endif

        // Compilation Stage
        //-------------------------------------------------------------------------

        void RenderGraph::Compile( RHI::RHIDevice* pRhiDevice )
        {
            EE_ASSERT( Threading::IsMainThread() );
            EE_ASSERT( m_frameExecuting );

            if ( pRhiDevice == nullptr )
            {
                EE_LOG_WARNING("RenderGraph", "RenderGraph::Compile()", "RHI Device missing! Cannot compile render graph!");
                return;
            }

            // Create actual RHI Resources
            //-------------------------------------------------------------------------

            m_resourceRegistry.Compile( pRhiDevice );

            // Compile and Analyze graph nodes to populate an execution sequence
            //-------------------------------------------------------------------------
        }

        // Execution Stage
        //-------------------------------------------------------------------------

        void RenderGraph::Execute()
        {
            EE_ASSERT( Threading::IsMainThread() );
            EE_ASSERT( m_frameExecuting );
        }

        // Cleanup Stage
        //-------------------------------------------------------------------------

        void RenderGraph::ClearAllResources( RHI::RHIDevice* pRhiDevice )
        {
            m_resourceRegistry.ClearAll( pRhiDevice );
        }
    }
}