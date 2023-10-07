#pragma once

#include "Base/_Module/API.h"

#include "RenderGraphResource.h"
// TODO: may be decouple pipeline barrier from command buffer 
#include "Base/RHI/RHICommandBuffer.h"
#include "Base/Render/RenderPipelineRegistry.h"
#include "Base/Types/Arrays.h"
#include "Base/Types/String.h"

#include <EASTL/type_traits.h>

namespace EE::RG
{
	class EE_BASE_API RGNodeResource
	{
        friend class RGResourceRegistry;

	public:

		RGNodeResource() = default;
		RGNodeResource( _Impl::RGResourceID slotID, RHI::RenderResourceAccessState access );

	private:

        _Impl::RGResourceID					    m_slotID;
        RHI::RenderResourceAccessState		    m_passAccess;
	};

	template <typename Tag, RGResourceViewType RVT>
	class RGNodeResourceRef
	{
		static_assert( eastl::is_base_of<RGResourceTagTypeBase<Tag>, Tag>::value, "Invalid render graph resource tag!" );

		typedef typename Tag::DescType DescType;
		typedef typename eastl::add_lvalue_reference_t<eastl::add_const_t<DescType>> DescCVType;

	public:

		RGNodeResourceRef( DescCVType desc, _Impl::RGResourceID slotID );

		inline DescCVType GetDesc() const { return m_desc; }

	private:

		friend class RenderGraph;

		// Safety: This reference is always held by RenderGraph during the life time of RGNodeResourceRef.
		DescCVType						m_desc;
		_Impl::RGResourceID				m_slotID;
	};

	typedef uint32_t NodeID;

	class EE_BASE_API RGNode
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

        Render::PipelineHandle                  m_pipelineHandle;
	};

    class RGExecutableNode
    {
    public:

    private:

        
    };

	//-------------------------------------------------------------------------

	template<typename Tag, RGResourceViewType RVT>
	RGNodeResourceRef<Tag, RVT>::RGNodeResourceRef( DescCVType desc, _Impl::RGResourceID slotID )
		: m_desc( desc ), m_slotID( slotID )
	{}
}