#pragma once

#include "Base/_Module/API.h"

#include "RenderGraphResource.h"
#include "Base/Render/RenderResourceBarrier.h"
#include "Base/Render/RenderPipelineRegistry.h"
#include "Base/Types/Arrays.h"
#include "Base/Types/String.h"

#include <limits>

namespace EE::RG
{
	class EE_BASE_API RGNodeResource
	{
        friend class RenderGraph;

	public:

		RGNodeResource() = default;
		RGNodeResource( _Impl::RGResourceSlotID slotID, Render::RenderResourceAccessState access );

	private:

        _Impl::RGResourceSlotID					    m_slotID;
		Render::RenderResourceAccessState			m_passAccess;
	};

	template <typename Tag, RGResourceViewType RVT>
	class RGNodeResourceRef
	{
		static_assert( std::is_base_of<RGResourceTypeBase<Tag>, Tag>::value, "Invalid render graph resource tag!" );

		typedef typename Tag::DescType DescType;
		typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;

	public:

		RGNodeResourceRef( DescCVType desc, _Impl::RGResourceSlotID slotID );

		inline DescCVType GetDesc() const { return m_desc; }

	private:

		friend class RenderGraph;

		// Safety: This reference is always held by RenderGraph during the life time of RGNodeResourceRef.
		DescCVType							m_desc;
		_Impl::RGResourceSlotID				m_slotID;
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

	//-------------------------------------------------------------------------

	template<typename Tag, RGResourceViewType RVT>
	RGNodeResourceRef<Tag, RVT>::RGNodeResourceRef( DescCVType desc, _Impl::RGResourceSlotID slotID )
		: m_desc( desc ), m_slotID( slotID )
	{}
}