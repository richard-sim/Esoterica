#pragma once

#include "Base/Types/String.h"
#include "Base/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace EE::TypeSystem::Reflection
{
    enum class ReflectionMacroType
    {
        ReflectModule,
        ReflectEnum,
        ReflectType,
        ReflectProperty,
        Resource,
        ReflectedResource,
        EntityComponent,
        SingletonEntityComponent,
        EntitySystem,
        EntityWorldSystem,

        NumMacros,
        Unknown = NumMacros,
    };

    char const* GetReflectionMacroText( ReflectionMacroType macro );

    //-------------------------------------------------------------------------

    namespace Settings
    {
        constexpr static char const* const g_engineNamespace = "EE";
        constexpr static char const* const g_engineNamespacePlusDelimiter = "EE::";
        constexpr static char const* const g_moduleHeaderParentDirectoryName = "_Module";
        constexpr static char const* const g_moduleHeaderSuffix = "Module.h";
        constexpr static char const* const g_autogeneratedDirectory = "_Module\\_AutoGenerated\\";
        constexpr static char const* const g_autogeneratedModuleFile = "_module.cpp";
        constexpr static char const* const g_globalAutoGeneratedDirectory = "Code\\Applications\\Shared\\_AutoGenerated\\";
        constexpr static char const* const g_engineTypeRegistrationHeaderPath = "EngineTypeRegistration.h";
        constexpr static char const* const g_toolsTypeRegistrationHeaderPath = "ToolsTypeRegistration.h";
        constexpr static char const* const g_temporaryDirectoryPath = "\\..\\_Temp\\";

        constexpr static char const* const g_devToolsExclusionDefine = "-D EE_SHIPPING";

        #if defined(_WIN32) && defined(EE_DX11)
        constexpr static char const* const g_engineGraphicBackendMacroDefine = "-D EE_DX11";
        #elif defined(EE_VULKAN)
        constexpr static char const* const g_engineGraphicBackendMacroDefine = "-D EE_VULKAN";
        #endif

        //-------------------------------------------------------------------------
        // Projects
        //-------------------------------------------------------------------------

        constexpr static char const* const g_moduleNameExclusionFilters[] = { "ThirdParty" };

        char const* const g_allowedProjectNames[] =
        {
            "Esoterica.Base",
            "Esoterica.Engine.Runtime",
            "Esoterica.Game.Runtime",
            "Esoterica.Engine.Tools",
            "Esoterica.Game.Tools",
        };

        constexpr static int const g_numAllowedProjects = sizeof( g_allowedProjectNames ) / sizeof( g_allowedProjectNames[0] );

        //-------------------------------------------------------------------------
        // Core class and type names
        //-------------------------------------------------------------------------

        constexpr static char const* const g_reflectedTypeInterfaceClassName = "IReflectedType";
        constexpr static char const* const g_baseEntityClassName = "Entity";
        constexpr static char const* const g_baseEntityComponentClassName = "EntityComponent";
        constexpr static char const* const g_baseEntitySystemClassName = "IEntitySystem";

        constexpr static char const* const g_baseResourceFullTypeName = "EE::Resource::IResource";

        //-------------------------------------------------------------------------
        // Clang Parser Settings
        //-------------------------------------------------------------------------

        char const* const g_includePaths[] =
        {
            "Code\\",
            "Code\\Base\\ThirdParty\\EA\\EABase\\include\\common\\",
            "Code\\Base\\ThirdParty\\EA\\EASTL\\include\\",
            "Code\\Base\\ThirdParty\\",
            "Code\\Base\\ThirdParty\\imgui\\",
            "External\\PhysX\\physx\\include\\",
            #if EE_ENABLE_NAVPOWER
            "External\\NavPower\\include\\"
            #endif
        };
    }

    //-------------------------------------------------------------------------

    namespace Utils
    {
        inline bool IsFileUnderToolsProject( FileSystem::Path const& filePath )
        {
            auto const& filePathStr = filePath.GetString();

            if ( filePathStr.find( "\\EngineTools\\" ) != String::npos )
            {
                return true;
            }

            if ( filePathStr.find( "\\GameTools\\" ) != String::npos )
            {
                return true;
            }

            return false;
        }
    }
}