#pragma once

#include "EngineTools/Resource/ResourceDescriptor.h"
#include "System/Render/RenderShader.h"

//-------------------------------------------------------------------------

namespace EE::Render
{
    enum class ShaderType : uint8_t
    {
        EE_REGISTER_ENUM

        Vertex = 0,
        Geometry,
        Pixel,
        Hull,
        Compute,
    };

    enum class ShaderBackendLanguage : uint8_t
    {
        EE_REGISTER_ENUM

        DX11 = 0,
        Vulkan,
    };

    enum class ShaderLanguage : uint8_t
    {
        EE_REGISTER_ENUM

        Hlsl = 0,
        Glsl,
    };

    struct EE_ENGINETOOLS_API ShaderResourceDescriptor : public Resource::ResourceDescriptor
    {
        EE_REGISTER_TYPE( ShaderResourceDescriptor );

    public:

        virtual bool IsValid() const override { return m_shaderPath.IsValid(); }

        virtual void GetCompileDependencies( TVector<ResourceID>& outDependencies ) override
        {
            if ( m_shaderPath.IsValid() )
            {
                outDependencies.emplace_back( m_shaderPath );
            }
        }

    public:

        EE_REGISTER ShaderType                      m_shaderType = ShaderType::Vertex;
        EE_EXPOSE ShaderBackendLanguage             m_shaderBackendLanguage = ShaderBackendLanguage::DX11;
        EE_EXPOSE ShaderLanguage                    m_shaderLanguage = ShaderLanguage::Hlsl;
        EE_EXPOSE ResourcePath                      m_shaderPath;
    };

    //-------------------------------------------------------------------------

    struct EE_ENGINETOOLS_API VertexShaderResourceDescriptor final : public ShaderResourceDescriptor
    {
        EE_REGISTER_TYPE( VertexShaderResourceDescriptor );

        VertexShaderResourceDescriptor()
        {
            m_shaderType = ShaderType::Vertex;
        }

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return VertexShader::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------
    
    struct EE_ENGINETOOLS_API PixelShaderResourceDescriptor final : public ShaderResourceDescriptor
    {
        EE_REGISTER_TYPE( PixelShaderResourceDescriptor );

        PixelShaderResourceDescriptor()
        {
            m_shaderType = ShaderType::Pixel;
        }

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return PixelShader::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------
    
    struct EE_ENGINETOOLS_API ComputeShaderResourceDescriptor final : public ShaderResourceDescriptor
    {
        EE_REGISTER_TYPE( ComputeShaderResourceDescriptor );

        ComputeShaderResourceDescriptor()
        {
            m_shaderType = ShaderType::Compute;
        }

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return ComputeShader::GetStaticResourceTypeID(); }
    };
}