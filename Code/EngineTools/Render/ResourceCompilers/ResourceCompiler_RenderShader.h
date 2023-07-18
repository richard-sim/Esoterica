#pragma once

#include "EngineTools/Resource/ResourceCompiler.h"

#include "EngineTools/Render/ResourceDescriptors/ResourceDescriptor_RenderShader.h"
#include "Base/Render/RenderShader.h"

//-------------------------------------------------------------------------

namespace EE::Render
{
    class Shader;

    class ShaderCompiler : public Resource::Compiler
    {
        EE_REFLECT_TYPE( ShaderCompiler );

    protected:

        using Resource::Compiler::Compiler;

    protected:

        Resource::CompilationResult CompileShader( Resource::CompileContext const& ctx, int32_t compilerVersion ) const;

    private:

        Resource::CompilationResult CompileDX11Shader( Resource::CompileContext const& ctx, ShaderResourceDescriptor const& desc, int32_t compilerVersion ) const;
        Resource::CompilationResult CompileVulkanShader( Resource::CompileContext const& ctx, ShaderResourceDescriptor const& desc, int32_t compilerVersion ) const;
    };

    //-------------------------------------------------------------------------

    class VertexShaderCompiler final : public ShaderCompiler
    {
        EE_REFLECT_TYPE( VertexShaderCompiler );
        static const int32_t s_version = 1;

    public:

        VertexShaderCompiler();

        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    };

    //-------------------------------------------------------------------------

    class PixelShaderCompiler final : public ShaderCompiler
    {
        EE_REFLECT_TYPE( PixelShaderCompiler );
        static const int32_t s_version = 1;

    public:

        PixelShaderCompiler();

        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    };

    //-------------------------------------------------------------------------

    class ComputeShaderCompiler final : public ShaderCompiler
    {
        EE_REFLECT_TYPE( ComputeShaderCompiler );
        static const int32_t s_version = 1;

    public:

        ComputeShaderCompiler();

        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    };
}