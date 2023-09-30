#pragma once

#include "RenderBuffer.h"
#include "Base/Resource/IResource.h"
#include "Base/TypeSystem/ReflectedType.h"
#include "Base/RHI/Resource/RHIShader.h"

//-------------------------------------------------------------------------

namespace EE
{
    namespace Render
    {
        //-------------------------------------------------------------------------

        class EE_BASE_API Shader : public Resource::IResource
        {
            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;
            friend class PipelineRegistry;

            EE_SERIALIZE( m_cbuffers, m_resourceBindings, m_byteCode, m_shaderEntryName, m_pushConstants, m_pipelineStage );
            EE_RESOURCE( 'shdr', "Render Shader" );

        public:

            static constexpr size_t NumMaxResourceBindingSet = 4;

            enum class BindingCountType : uint8_t
            {
                // Exact one resource binding.
                // Example:
                // StructuredBuffer<uint> buffer;
                One,
                // Compile-determined sized resource bindings.
                // Example:
                // StructuredBuffer<uint> buffer[2];
                Static,
                // Variable number of resource bindings.
                // Used inside bindless descriptors;
                // Example:
                // StructuredBuffer<uint> buffer[];
                Dynamic
            };

            struct ReflectedBindingCount
            {
                EE_SERIALIZE( m_type, m_count );

                BindingCountType            m_type;
                size_t                      m_count;

                inline friend bool operator==( ReflectedBindingCount const& lhs, ReflectedBindingCount const& rhs )
                {
                    return lhs.m_type == rhs.m_type && lhs.m_count == rhs.m_count;
                }
            };

            enum class ReflectedBindingResourceType
            {
                Sampler = 0,
                CombinedImageSampler,
                SampledImage,
                StorageImage,
                UniformTexelBuffer,
                StorageTexelBuffer,
                UniformBuffer,
                StorageBuffer,
                InputAttachment
            };

            struct ResourceBinding
            {
                EE_SERIALIZE( m_ID, m_slot, m_bindingCount, m_bindingResourceType, m_extraInfos );

                ResourceBinding() : m_slot( 0 ) {}
                ResourceBinding( uint32_t ID, uint32_t slot ) : m_ID( ID ), m_slot( slot ) {}
                ResourceBinding( uint32_t ID, uint32_t slot, ReflectedBindingCount bindingCount, ReflectedBindingResourceType resourceType, String const& extraInfos )
                    : m_ID( ID ), m_slot( slot ), m_bindingCount( bindingCount ), m_bindingResourceType( resourceType ), m_extraInfos( extraInfos ) {}

                inline bool operator<( ResourceBinding const& rhs ) const { return m_slot < rhs.m_slot; }

                uint32_t                            m_ID;
                uint32_t                            m_slot;

                ReflectedBindingCount               m_bindingCount;
                ReflectedBindingResourceType        m_bindingResourceType;
                String                              m_extraInfos;
            };

            using ResourceBindingSetLayout = TVector<TVector<ResourceBinding>>;

            struct PushConstant
            {
                EE_SERIALIZE( m_ID, m_size, m_offset );

                uint32_t                m_ID;
                uint32_t                m_size;
                uint32_t                m_offset;
            };

        public:

            Shader() = default;
            Shader( Shader const& ) = default;
            virtual ~Shader() { EE_ASSERT( !m_shaderHandle.IsValid() ); }

            Shader& operator=( Shader const& ) = default;

            inline PipelineStage GetPipelineStage() const { return m_pipelineStage; }

            inline ShaderHandle const& GetShaderHandle() const { return m_shaderHandle; }
            inline RHI::RHIShader* const& GetRHIShader() const { return m_rhiShader; }
            inline uint32_t GetNumConstBuffers() const { return (uint32_t) m_cbuffers.size(); }
            inline RenderBuffer const& GetConstBuffer( uint32_t i ) const { EE_ASSERT( i < m_cbuffers.size() ); return m_cbuffers[i]; }
            inline ResourceBindingSetLayout const& GetResourceBindingSetLayout() const { return m_resourceBindings; }
            inline PushConstant const& GetPushConstant() const { return m_pushConstants; }
            inline String const& GetEntryName() const { return m_shaderEntryName; }

            inline bool operator==( Shader const& rhs ) const { return m_shaderHandle.m_pData == rhs.m_shaderHandle.m_pData; }
            inline bool operator!=( Shader const& rhs ) const { return m_shaderHandle.m_pData != rhs.m_shaderHandle.m_pData; }

            virtual bool IsValid() const override
            {
                return ( m_rhiShader != nullptr && m_rhiShader->IsValid() ) || m_shaderHandle.IsValid();
            }

        protected:

            Shader( PipelineStage stage );
            Shader( PipelineStage stage, uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

        protected:

            ShaderHandle                        m_shaderHandle; // TODO: remove this with RHI
            RHI::RHIShader*                     m_rhiShader = nullptr;
            String                              m_shaderEntryName;
            TVector<RenderBuffer>               m_cbuffers;
            ResourceBindingSetLayout            m_resourceBindings;
            Blob                                m_byteCode;
            PushConstant                        m_pushConstants;
            PipelineStage                       m_pipelineStage;
        };

        //-------------------------------------------------------------------------

        class EE_BASE_API VertexShader : public Shader
        {
            EE_SERIALIZE( EE_SERIALIZE_BASE( Shader ), m_vertexLayoutDesc );
            EE_RESOURCE( 'vsdr', "Vertex Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;
            friend class PipelineRegistry;

        public:

            VertexShader() : Shader( PipelineStage::Vertex )
            {}
            VertexShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers, VertexLayoutDescriptor const& vertexLayoutDesc );

            inline VertexLayoutDescriptor const& GetVertexLayoutDesc() const
            {
                return m_vertexLayoutDesc;
            }

        private:

            VertexLayoutDescriptor m_vertexLayoutDesc;
        };

        //-------------------------------------------------------------------------

        class EE_BASE_API PixelShader : public Shader
        {
            EE_SERIALIZE( EE_SERIALIZE_BASE( Shader ) );
            EE_RESOURCE( 'psdr', "Pixel Shader" );

            friend class PipelineRegistry;

        public:

            PixelShader() : Shader( PipelineStage::Pixel ) {}
            PixelShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

        };

        //-------------------------------------------------------------------------

        class EE_BASE_API GeometryShader : public Shader
        {
            EE_SERIALIZE( EE_SERIALIZE_BASE( Shader ) );
            EE_RESOURCE( 'gsdr', "Geometry Shader");

            friend class PipelineRegistry;

        public:

            GeometryShader() : Shader( PipelineStage::Geometry ) {}
            GeometryShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );
        };

        //-------------------------------------------------------------------------

        class EE_BASE_API ComputeShader : public Shader
        {
            EE_SERIALIZE( EE_SERIALIZE_BASE( Shader ) );
            EE_RESOURCE( 'csdr', "Compute Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;
            friend class PipelineRegistry;

        public:

            ComputeShader() : Shader( PipelineStage::Compute ) {}
            ComputeShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );
        };
    }
}