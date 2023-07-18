#pragma once

#include "System/Types/Atomic.h"

#include <type_traits>
#include <limits>

//-------------------------------------------------------------------------
// Reference Counting Object
//-------------------------------------------------------------------------
// Simple implementation of reference count object.
//

namespace EE
{
	template <bool THREAD_SAFE>
	class RefCountObject
	{
	public:

		RefCountObject() = default;

	private:

		typedef typename std::conditional<THREAD_SAFE, AtomicU32, uint32_t>::type CounterType;

	protected:

		template <typename T>
		friend class RefCountPtr;

		CounterType IncreaseRef()
		{
			return ++m_count;
		}

		CounterType DecreaseRef()
		{
			return --m_count;
		}

	public:

		CounterType						m_count = 0;
	};

	typedef RefCountObject<false> RcObject;
	typedef RefCountObject<true>  ThreadSafeRcObject;

	template <typename T>
	class RefCountPtr
	{
		EE_STATIC_ASSERT( ( eastl::disjunction_v<
							eastl::is_base_of< RcObject, T >,
							eastl::is_base_of< ThreadSafeRcObject, T >
		> ), "Invalid RefCountPtr object!" );

	public:

		template <typename... Args>
		static RefCountPtr<T> New( Args&&... args );

	public:

		RefCountPtr() = default;
		RefCountPtr( std::nullptr_t ) {};

		RefCountPtr( T* ptr )
			: m_pData( ptr )
		{
			this->IncreaseRef();
		}

		~RefCountPtr()
		{
			this->DecreaseRef();
			m_pData = nullptr;
		}

		RefCountPtr( RefCountPtr const& rhs )
			: m_pData( rhs.m_pData )
		{
			this->IncreaseRef();
		}

		RefCountPtr& operator=( RefCountPtr const& rhs )
		{
			// Note: it is ok that rhs is itself.
			rhs.IncreaseRef();
			this->DecreaseRef();
			m_pData = rhs.m_pData;
			return *this;
		}

		RefCountPtr( RefCountPtr&& rhs )
			: m_pData( rhs.m_pData )
		{
			rhs.m_pData = nullptr;
		}

		RefCountPtr& operator=( RefCountPtr&& rhs )
		{
			if ( this != &rhs )
			{
				this->DecreaseRef();
				m_pData = rhs.m_pData;
				rhs.m_pData = nullptr;
			}
			return *this;
		}

		RefCountPtr& operator=( nullptr_t )
		{
			this->DecreaseRef();
			m_pData = nullptr;
			return *this;
		}
		
		T* Raw() const
		{
			return m_pData;
		}

		T& operator*() const { return *m_pData; }
		T* operator->() const { return m_pData; }

		bool operator==( const RefCountPtr& rhs ) const { return m_pData == rhs.m_pData; }
		bool operator!=( const RefCountPtr& rhs ) const { return m_pData != rhs.m_pData; }

	private:

		void IncreaseRef() const
		{
			if ( m_pData != nullptr )
			{
				m_pData->IncreaseRef();
			}
		}

		void DecreaseRef()
		{
			if ( m_pData != nullptr )
			{
				if ( m_pData->DecreaseRef() == 0 )
				{
					EE::Delete( m_pData );
					m_pData = nullptr;
				}
			}
		}

	private:

		T*								m_pData = nullptr;
	};

	//class TestObject : public ThreadSafeRcObject
	//{
	//public:
	//	TestObject( long data )
	//		: m_data( data )
	//	{}

	//private:

	//	long				m_data;
	//};

	//-------------------------------------------------------------------------

	template <typename T>
	template <typename... Args>
	inline RefCountPtr<T> RefCountPtr<T>::New( Args&&... args )
	{
		return RefCountPtr<T>( EE::New<T, Args...>( std::forward<Args>( args )... ) );
	}
}