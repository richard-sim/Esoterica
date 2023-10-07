#include "RHITexture.h"
#include "Base/Logging/Log.h"

namespace EE::RHI
{
    RHITexture::~RHITexture()
    {
        if ( !m_viewCache.empty() )
        {
            EE_LOG_ERROR( "RHI", "RHITexture", "Did you forget to call ClearAllViews() before destroy the texture?" );
            EE_ASSERT( false );
        }
    }

    //-------------------------------------------------------------------------

    RHITextureView* RHITexture::GetOrCreateView( RHIDevice* pDevice, RHITextureViewCreateDesc const& desc )
    {
        auto iter = m_viewCache.find( desc );
        if ( iter != m_viewCache.end() )
        {
            return iter->second;
        }

        if ( !pDevice )
        {
            return nullptr;
        }

        auto* pTextureView = CreateView( pDevice, desc );
        if ( pTextureView )
        {
            m_viewCache.insert( { desc, pTextureView } );
            return pTextureView;
        }

        return nullptr;
    }

    void RHITexture::ClearAllViews( RHIDevice* pDevice )
    {
        for ( auto& view : m_viewCache )
        {
            DestroyView( pDevice, view.second );
        }
        m_viewCache.clear();
    }

}