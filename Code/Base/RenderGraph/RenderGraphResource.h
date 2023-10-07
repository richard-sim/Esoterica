#pragma once

#include "Base/_Module/API.h"
#include "Base/Logging/Log.h"
#include "Base/Types/Variant.h"
#include "Base/Types/Optional.h"
#include "Base/Memory/Pointers.h"
#include "Base/Render/RenderAPI.h"
#include "Base/RHI/Resource/RHIResourceCreationCommons.h"
#include "Base/RHI/RHICommandBuffer.h"

#include <limits>
#include "EASTL/utility.h"

namespace EE::RHI
{
    class RHIDevice;
    class RHIResource;
    class RHIBuffer;
    class RHITexture;
}

//-------------------------------------------------------------------------
//	Render graph resource lifetime.
// 
//	primarily consists of two stage:
// 
//	1. Transient. Resource which used inside a specific render graph, it is invalid
//	to direct used it again in next time execution. This type of resources are dynamic,
//	means that they will be created during every execution and restore by render graph 
//	at the end of the execution. On next time execution, this type of resource may 
//	_NOT_ be created.
// 
//	2. Exportable. Resource which can be used when render graph execution is complete.
//	This type of resource can be imported by another render graph, or just simply store
//	and managed by the user.
//-------------------------------------------------------------------------

namespace EE::RG
{
	enum class RGResourceType : uint8_t
	{
		Buffer = 0,
		Texture = 1,
		Unknown = std::numeric_limits<uint8_t>::max(),
	};

    namespace _Impl
    {
		struct EE_BASE_API RGResourceID
		{
		public:

			RGResourceID() = default;
			RGResourceID( uint32_t id )
                : m_id( id )
            {}

			inline bool IsValid() const { return m_id != std::numeric_limits<uint32_t>::max(); }

			inline bool operator==( RGResourceID const& rhs ) const { return m_id == rhs.m_id && m_generation == rhs.m_generation; }
			inline bool operator!=( RGResourceID const& rhs ) const { return m_id != rhs.m_id && m_generation != rhs.m_generation; }

			inline void Expire()
			{ 
				if ( m_generation >= std::numeric_limits<uint32_t>::max() )
					m_generation = 0;
				else
					m_generation += 1;
			}

		public:

			friend class RGNodeBuilder;

			uint32_t				m_id = std::numeric_limits<uint32_t>::max();
			uint32_t				m_generation = 0;
		};
    }

    // forward declarations
    namespace _Impl
    {
	    struct RGBufferDesc;
        struct RGTextureDesc;
    }

	struct RGResourceTagBuffer;
	struct RGResourceTagTexture;

    // User resource creation description
    //-------------------------------------------------------------------------

	struct EE_BASE_API BufferDesc
	{
        using InnerDescType = RHI::RHIBufferCreateDesc;

	public:

		typedef _Impl::RGBufferDesc RGDescType;
		typedef RGResourceTagBuffer RGResourceTypeTag;
        typedef RHI::RHIBuffer*     RGCompiledResourceType;

    public:

        // Forward functions from RHI
        //-------------------------------------------------------------------------

        inline static BufferDesc NewSize( uint32_t sizeInByte ) { return BufferDesc{ InnerDescType::NewSize( sizeInByte ) }; }
        inline static BufferDesc NewAlignedSize( uint32_t sizeInByte, uint32_t alignment ) { return BufferDesc{ InnerDescType::NewAlignedSize( sizeInByte, alignment ) }; }
        inline static BufferDesc NewDeviceAddressable( uint32_t sizeInByte ) { return BufferDesc{ InnerDescType::NewDeviceAddressable( sizeInByte ) }; }
        inline static BufferDesc NewVertexBuffer( uint32_t sizeInByte ) { return BufferDesc{ InnerDescType::NewVertexBuffer( sizeInByte ) }; }
        inline static BufferDesc NewIndexBuffer( uint32_t sizeInByte ) { return BufferDesc{ InnerDescType::NewIndexBuffer( sizeInByte ) }; }

        inline bool IsValid() const { return m_desc.IsValid(); }

	public:

        // Note: Separate RHI module with RenderGraph module, preserve a low coupling relation.
        InnerDescType                       m_desc;
	};

	struct EE_BASE_API TextureDesc
	{
        using InnerDescType = RHI::RHITextureCreateDesc;

    public:

        // Forward functions from RHI
        //-------------------------------------------------------------------------

        inline static TextureDesc New1D( uint32_t width, RHI::EPixelFormat format ) { return TextureDesc{ InnerDescType::New1D( width, format ) }; }
        inline static TextureDesc New1DArray( uint32_t width, RHI::EPixelFormat format, uint32_t array ) { return TextureDesc{ InnerDescType::New1DArray( width, format, array ) }; }
        inline static TextureDesc New2D( uint32_t width, uint32_t height, RHI::EPixelFormat format ) { return TextureDesc{ InnerDescType::New2D( width, height, format ) }; }
        inline static TextureDesc New2DArray( uint32_t width, uint32_t height, RHI::EPixelFormat format, uint32_t array ) { return TextureDesc{ InnerDescType::New2DArray( width, height, format, array ) }; }
        inline static TextureDesc New3D( uint32_t width, uint32_t height, uint32_t depth, RHI::EPixelFormat format ) { return TextureDesc{ InnerDescType::New3D( width, height, depth, format ) }; }
        inline static TextureDesc NewCubemap( uint32_t width, RHI::EPixelFormat format ) { return TextureDesc{ InnerDescType::NewCubemap( width, format ) }; }

        inline bool IsValid() const { return m_desc.IsValid(); }

	public:

		typedef _Impl::RGTextureDesc RGDescType;
		typedef RGResourceTagTexture RGResourceTypeTag;
        typedef RHI::RHITexture*     RGCompiledResourceType;

	public:

        // Note: Separate RHI module with RenderGraph module, preserve a low coupling relation.
        InnerDescType                       m_desc;
	};

    //-------------------------------------------------------------------------

    namespace _Impl
    {
        // CRTP type to provide common static polymorphic interface.
	    template <typename SelfType, typename DescType = typename SelfType::DescType>
	    struct RGResourceDesc
	    {
		    using SelfCVType = typename std::add_lvalue_reference_t<std::add_const_t<SelfType>>;
		    using DescCVType = typename std::add_lvalue_reference_t<std::add_const_t<DescType>>;

		    inline DescCVType GetDesc() const
		    {
			    return static_cast<SelfCVType>( *this ).m_desc;
		    }

            inline SelfCVType GetRGDesc() const
            {
                return static_cast<SelfCVType>( *this );
            }
	    };

	    struct RGBufferDesc final : public RGResourceDesc<RGBufferDesc, BufferDesc>
	    {
	    public:

		    typedef BufferDesc DescType;
		    typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;
            typedef typename BufferDesc::RGCompiledResourceType RGCompiledResourceType;

        public:

		    DescType				m_desc;
	    };

        struct RGTextureDesc final : public RGResourceDesc<RGTextureDesc, TextureDesc>
	    {
	    public:

		    typedef TextureDesc DescType;
		    typedef typename std::add_lvalue_reference_t<std::add_const_t<DescType>> DescCVType;
            typedef typename TextureDesc::RGCompiledResourceType RGCompiledResourceType;

        public:

		    DescType				m_desc;
	    };
    }

    // CRTP type to provide common static polymorphic interface.
	template <typename Tag>
	struct RGResourceTagTypeBase
	{
		constexpr inline static RGResourceType GetRGResourceType()
		{
			return Tag::GetRGResourceType();
		}
	};

	struct RGResourceTagBuffer : public RGResourceTagTypeBase<RGResourceTagBuffer>
	{
		typedef _Impl::RGBufferDesc RGDescType;
		typedef BufferDesc          DescType;

		constexpr inline static RGResourceType GetRGResourceType()
		{
			return RGResourceType::Buffer;
		}
	};

	struct RGResourceTagTexture : public RGResourceTagTypeBase<RGResourceTagTexture>
	{
		typedef _Impl::RGTextureDesc RGDescType;
		typedef TextureDesc          DescType;

		constexpr inline static RGResourceType GetRGResourceType()
		{
			return RGResourceType::Texture;
		}
	};

	//EE_STATIC_ASSERT( sizeof( RGBuffer ) == 0, "Size of tag RGBuffer should be zero!");
	//EE_STATIC_ASSERT( sizeof( RGTexture ) == 0, "Size of tag RGBuffer should be zero!" );

	enum class RGResourceViewType
	{
		SRV, // shader resource view
		UAV, // unordered access view
		RT   // render target
	};

    // Empty struct, use as a tag.
	struct RGLazyCreateResource
	{
        //RHI::RHIResource*                       m_pRhiResource = nullptr;
	};

	struct RGImportedResource
	{
		TSharedPtr<RHI::RHIResource>			m_pImportedResource;
		RHI::RenderResourceBarrierState			m_currentAccess;
	};

    class RGCompiledResource;

    namespace _Impl
    {
        static constexpr size_t BufferDescVariantIndex = 0;
        static constexpr size_t TextureDescVariantIndex = 1;

        static constexpr size_t LazyCreateResourceVariantIndex = 0;
        static constexpr size_t ImportedResourceVariantIndex = 1;
    }

	class EE_BASE_API RGResource
	{
        friend class RenderGraph;

	public:

        template <typename RGDescType, typename DescType>
        RGResource( _Impl::RGResourceDesc<RGDescType, DescType> const& desc );
        ~RGResource() = default;

        RGResource( RGResource const& ) = delete;
        RGResource& operator=( RGResource const& ) = delete;

        RGResource( RGResource&& rhs ) noexcept
        {
            m_desc = eastl::exchange( rhs.m_desc, {} );
            m_resource = eastl::exchange( rhs.m_resource, {} );
        }
        RGResource& operator=( RGResource&& rhs ) noexcept
        {
            RGResource copy( eastl::move( rhs ) );
            copy.swap( *this );
            return *this;
        }

	public:

		template <typename Tag,
			typename DescType = typename Tag::DescType,
			typename DescConstRefType = typename std::add_lvalue_reference_t<std::add_const_t<DescType>>
		>
		DescConstRefType GetDesc() const;

        inline RGResourceType GetResourceType() const
        {
            if ( m_desc.index() == _Impl::BufferDescVariantIndex )
            {
                return RGResourceType::Buffer;
            }
            else if ( m_desc.index() == _Impl::TextureDescVariantIndex )
            {
                return RGResourceType::Texture;
            }
            else
            {
                return RGResourceType::Unknown;
            }
        }

        inline bool IsImportedResource() const { return m_resource.index() == _Impl::ImportedResourceVariantIndex; }

        inline RGImportedResource&       GetImportedResource()       { return eastl::get<_Impl::ImportedResourceVariantIndex>( m_resource ); }
        inline RGImportedResource const& GetImportedResource() const { return eastl::get<_Impl::ImportedResourceVariantIndex>( m_resource ); }

        //-------------------------------------------------------------------------

        // This function only operates on rvalue.
        // You must give out the ownership of origin resource to get a compiled resource.
        RGCompiledResource Compile( RHI::RHIDevice* pDevice ) &&;

    public:

        friend void swap( RGResource& lhs, RGResource& rhs ) noexcept
        {
            lhs.swap( rhs );
        }

        void swap( RGResource& rhs ) noexcept
        {
            eastl::swap( m_desc, rhs.m_desc );
            eastl::swap( m_resource, rhs.m_resource );
        }

	private:

		// TODO: compile-time enum element expand (use macro?)
		TVariant<
            _Impl::RGBufferDesc,
            _Impl::RGTextureDesc
		>														m_desc;
		TVariant<RGLazyCreateResource, RGImportedResource>		m_resource;
	};

    template <typename RGDescType, typename DescType>
    RGResource::RGResource( _Impl::RGResourceDesc<RGDescType, DescType> const& desc )
        : m_desc( desc.GetRGDesc() )
    {}

	template <typename Tag, typename DescType, typename DescConstRefType>
	DescConstRefType RGResource::GetDesc() const
	{
        EE_STATIC_ASSERT( ( std::is_base_of<RGResourceTagTypeBase<Tag>, Tag>::value ), "Invalid render graph resource tag!" );

		constexpr size_t const index = static_cast<size_t>( Tag::GetRGResourceType() );
		return eastl::get<index>( m_desc ).GetDesc();
	}

    class RGCompiledResource
    {
        friend class RGResource;

    public:

        template <typename Tag,
            typename DescType = typename Tag::DescType,
            typename DescConstRefType = typename std::add_lvalue_reference_t<std::add_const_t<DescType>>
        >
        DescConstRefType GetDesc() const;

        template <typename Tag,
            typename DescType = typename Tag::DescType,
            typename RGCompiledResourceRefType = typename std::add_lvalue_reference_t<typename DescType::RGCompiledResourceType>
        >
        RGCompiledResourceRefType GetResource();

        inline RGResourceType GetResourceType() const
        {
            if ( m_desc.index() == _Impl::BufferDescVariantIndex )
            {
                return RGResourceType::Buffer;
            }
            else if ( m_desc.index() == _Impl::TextureDescVariantIndex )
            {
                return RGResourceType::Texture;
            }
            else
            {
                return RGResourceType::Unknown;
            }
        }

        inline bool IsImportedResource() const { return m_importedResource.has_value(); }

        //-------------------------------------------------------------------------

        // TODO: change RGCompiledResource into RGRetiredResource
        void Retire( RHI::RHIDevice* pDevice );

    private:

        // TODO: compile-time enum element expand (use macro?)
        TVariant<
            _Impl::RGBufferDesc,
            _Impl::RGTextureDesc
        >														        m_desc;
        TVariant<
            typename _Impl::RGBufferDesc::RGCompiledResourceType,
            typename _Impl::RGTextureDesc::RGCompiledResourceType
        >		                                                        m_resource;
        // Note: Imported resources contain share pointer to outer resource.
        //       It is our duty to keep this share pointer alive until finish using this imported resource.
        //       So after compile RGResource to RGCompiledResource, we should keep a copy of imported resource if it is.
        TOptional<RGImportedResource>                                   m_importedResource = {};
    };

    template <typename Tag, typename DescType, typename DescConstRefType>
    DescConstRefType RGCompiledResource::GetDesc() const
    {
        EE_STATIC_ASSERT( (std::is_base_of<RGResourceTagTypeBase<Tag>, Tag>::value), "Invalid render graph resource tag!" );

        constexpr size_t const index = static_cast<size_t>( Tag::GetRGResourceType() );
        return eastl::get<index>( m_desc ).GetDesc();
    }

    template <typename Tag, typename DescType, typename RGCompiledResourceRefType>
    RGCompiledResourceRefType EE::RG::RGCompiledResource::GetResource()
    {
        EE_ASSERT( GetResourceType() == Tag::GetRGResourceType() );

        constexpr size_t const index = static_cast<size_t>( Tag::GetRGResourceType() );
        return eastl::get<index>( m_resource );
    }
}