#pragma once

#include "Base/_Module/API.h"

#include "Base/Systems.h"
#include "Base/Resource/ResourcePtr.h"
#include "Base/Render/RenderShader.h"
#include "Base/Render/RenderPipeline.h"
#include "Base/Render/RenderPipelineState.h"
#include "Base/Types/IDVector.h"
#include "Base/Memory/Pointers.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

#include <numeric>

//-------------------------------------------------------------------------

namespace EE
{
	class TaskSystem;
	namespace { class ResourceSystem; }
}

namespace EE::RHI
{
    class RHIDevice;
}

//-------------------------------------------------------------------------

namespace EE::Render
{
	class PipelineHandle
	{
		friend class PipelineRegistry;

	public:

		PipelineHandle() = default;

		inline bool IsValid() const
		{
			return m_ID != 0 && m_ID != std::numeric_limits<uint32_t>::max();
		}

		inline uint32_t RawValue() const
		{
			return m_ID;
		}

	public:

		inline friend bool operator<( PipelineHandle const& lhs, PipelineHandle const& rhs )
		{
			return lhs.m_ID < rhs.m_ID;
		}

		inline friend bool operator==( PipelineHandle const& lhs, PipelineHandle const& rhs )
		{
			return lhs.m_ID == rhs.m_ID;
		}

	private:

		PipelineHandle( PipelineType type, uint32_t id );

	private:

		// this id will be used as ResourceRequestID, so it can not be 0.
		uint32_t				m_ID = std::numeric_limits<uint32_t>::max();
		PipelineType			m_type = PipelineType::Raster;
	};
}

//-------------------------------------------------------------------------

namespace eastl
{
	template <>
	struct hash<EE::Render::PipelineHandle>
	{
		EE_FORCE_INLINE eastl_size_t operator()( EE::Render::PipelineHandle const& handle ) const
		{
			return static_cast<eastl_size_t>( handle.RawValue() );
		}
	};
}

//-------------------------------------------------------------------------

namespace EE::Render
{
	class RasterPipelineEntry
	{
		friend class PipelineRegistry;

	public:

		inline PipelineHandle GetID() const { return m_handle; }
		inline bool IsReadyToCreatePipelineLayout() const { return m_vertexShader.IsLoaded() && m_pixelShader.IsLoaded(); }
        inline bool IsEntryRegisteredIntoRHI() const { return false; }

	private:

		// TODO: support more shader type
		TResourcePtr<VertexShader>			    m_vertexShader;
		TResourcePtr<PixelShader>			    m_pixelShader;

        RHI::RHIRasterPipelineStateCreateDesc   m_desc;

		PipelineHandle						    m_handle;
	};

	class ComputePipelineEntry
	{
		friend class PipelineRegistry;

	public:

		inline PipelineHandle GetID() const { return m_handle; }

	private:

		//ComputeShader*					m_pComputeShader = nullptr;

		PipelineHandle					m_handle;
	};

	//-------------------------------------------------------------------------

	class EE_BASE_API PipelineRegistry
	{

	public:

		PipelineRegistry() = default;
		~PipelineRegistry();

		PipelineRegistry( PipelineRegistry const& ) = delete;
		PipelineRegistry& operator=( PipelineRegistry const& ) = delete;

		PipelineRegistry( PipelineRegistry&& ) = delete;
		PipelineRegistry& operator=( PipelineRegistry&& ) = delete;

	public:

		void Initialize( SystemRegistry const& systemRegistry );
		void Shutdown();

		[[nodiscard]] PipelineHandle RegisterRasterPipeline( RHI::RHIRasterPipelineStateCreateDesc const& rasterPipelineDesc );
		[[nodiscard]] PipelineHandle RegisterComputePipeline( ComputePipelineDesc const& computePipelineDesc );

		void LoadAndUpdatePipelines( TSharedPtr<RHI::RHIDevice> const& pDevice );

	private:

		void LoadPipelineShaders();
        void CreateRasterPipelineStateLayout( TSharedPtr<RasterPipelineEntry> const& rasterEntry, TSharedPtr<RHI::RHIDevice> const& pDevice );

		void UnloadAllPipelineShaders();

	private:

		bool															    m_isInitialized = false;

		TaskSystem*														    m_pTaskSystem = nullptr;
		Resource::ResourceSystem*										    m_pResourceSystem = nullptr;

        // TODO: may be extract to single pipeline cache class
		TIDVector<PipelineHandle, TSharedPtr<RasterPipelineEntry>>		    m_rasterPipelineStatesCache;
		THashMap<RHI::RHIRasterPipelineStateCreateDesc, PipelineHandle>     m_rasterPipelineHandlesCache;

        TIDVector<PipelineHandle, ComputePipelineEntry>					    m_computePipelineStates;
        //THashMap<ComputePipelineDesc, PipelineHandle>					    m_computePipelineHandles;

        TVector<TSharedPtr<RasterPipelineEntry>>						    m_waitToSubmitRasterPipelines;
        TVector<TSharedPtr<RasterPipelineEntry>>						    m_waitToLoadRasterPipelines;
        TVector<TSharedPtr<RasterPipelineEntry>>						    m_waitToRegisteredRasterPipelines;
	};
}