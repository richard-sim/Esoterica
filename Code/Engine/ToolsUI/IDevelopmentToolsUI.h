#pragma once
#include "Engine/_Module/API.h"
#include "Base/Resource/ResourceRequesterID.h"
#include "Base/Types/Arrays.h"

//-------------------------------------------------------------------------

namespace EE
{
    class UpdateContext;
}

//-------------------------------------------------------------------------
// Development Tools Framework
//-------------------------------------------------------------------------
// Base class for any runtime/editor development UI tools

#if EE_DEVELOPMENT_TOOLS
namespace EE::ImGuiX
{
    class ImageCache;

    //-------------------------------------------------------------------------

    class EE_ENGINE_API IDevelopmentToolsUI
    {
    public:

        IDevelopmentToolsUI() = default;
        IDevelopmentToolsUI( IDevelopmentToolsUI const& ) = default;
        virtual ~IDevelopmentToolsUI() = default;

        IDevelopmentToolsUI& operator=( IDevelopmentToolsUI const& rhs ) = default;

        virtual void Initialize( UpdateContext const& context, ImGuiX::ImageCache* pImageCache ) = 0;
        virtual void Shutdown( UpdateContext const& context ) = 0;

        // This is called at the absolute start of the frame before we update the resource system, start updating any entities, etc...
        // Any entity/world/map state changes need to be done via this update!
        virtual void StartFrame( UpdateContext const& context ) {}

        // Optional update run before we update the world at each stage
        virtual void Update( UpdateContext const& context ) {}

        // This is called at the absolute end of the frame just before we kick off rendering. It is generally NOT safe to modify any world/map/entity during this update!!!
        virtual void EndFrame( UpdateContext const& context ) {}

        // Hot Reload Support
        //-------------------------------------------------------------------------

        // Start a hot-reload operation by unloading all resource about to be reloaded
        virtual void HotReload_UnloadResources( TVector<Resource::ResourceRequesterID> const& usersToReload, TVector<ResourceID> const& resourcesToBeReloaded ) = 0;

        // Request a load on all unloaded resources
        virtual void HotReload_ReloadResources() = 0;

        // Notify UI that all load requests are complete
        virtual void HotReload_ReloadComplete() = 0;
    };
}
#endif