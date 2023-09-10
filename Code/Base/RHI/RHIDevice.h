#pragma once

#include "Base/Render/RenderAPI.h"
#include "Base/Memory/Pointers.h"
#include "Base/Types/Arrays.h"
#include "Base/Render/RenderShader.h"
#include "Base/Resource/ResourcePtr.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"

namespace EE::RHI
{
    class RHIShader;
    class RHISemaphore;
    class RHITexture;
    class RHIBuffer;
    class RHIPipelineState;

    class RHIDevice
    {
    public:

        using CompiledShaderArray = TInlineVector<Render::Shader const*, static_cast<size_t>( Render::NumPipelineStages )>;

        //-------------------------------------------------------------------------

        RHIDevice() = default;
        virtual ~RHIDevice() = default;

        RHIDevice( RHIDevice const& ) = delete;
        RHIDevice& operator=( RHIDevice const& ) = delete;

        RHIDevice( RHIDevice&& ) = default;
        RHIDevice& operator=( RHIDevice&& ) = default;

        //-------------------------------------------------------------------------

        virtual RHITexture* CreateTexture( RHITextureCreateDesc const& createDesc ) = 0;
        virtual void        DestroyTexture( RHITexture* pTexture ) = 0;

        virtual RHIBuffer* CreateBuffer( RHIBufferCreateDesc const& createDesc ) = 0;
        virtual void       DestroyBuffer( RHIBuffer* pBuffer ) = 0;

        virtual RHIShader* CreateShader( RHIShaderCreateDesc const& createDesc ) = 0;
        virtual void       DestroyShader( RHIShader* pShader ) = 0;

        virtual RHISemaphore* CreateSyncSemaphore( RHISemaphoreCreateDesc const& createDesc ) = 0;
        virtual void          DestroySyncSemaphore( RHISemaphore* pSemaphore ) = 0;

        //-------------------------------------------------------------------------

        virtual RHIPipelineState* CreateRasterPipelineState( RHI::RHIRasterPipelineStateCreateDesc const& createDesc, CompiledShaderArray const& compiledShaders ) = 0;
        virtual void              DestroyRasterPipelineState( RHIPipelineState* pPipelineState ) = 0;

    private:
    };
}

