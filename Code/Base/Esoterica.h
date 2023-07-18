#pragma once

#include "_Module/API.h"
#include <stdint.h>

//-------------------------------------------------------------------------

using nullptr_t = decltype( nullptr );

//-------------------------------------------------------------------------
// Common
//-------------------------------------------------------------------------

#define EE_STRINGIZING(x) #x
#define EE_MAKE_STRING(x) EE_STRINGIZING(x)
#define EE_FILE_LINE __FILE__ ":" EE_MAKE_STRING(__LINE__)

//-------------------------------------------------------------------------
// Configurations
//-------------------------------------------------------------------------

// EE_DEBUG = unoptimized build with debug info (debug drawing, string debug names, etc...)
// EE_RELEASE = optimized build with debug info (debug drawing, string debug names, etc...)
// EE_SHIPPING = optimized build with no debug info

//-------------------------------------------------------------------------
// Development Tools
//-------------------------------------------------------------------------
// EE_DEVELOPMENT_TOOLS_LINE_IN_MACRO exists for use in macros.

#if !EE_SHIPPING
#define EE_DEVELOPMENT_TOOLS 1
#endif

#if EE_DEVELOPMENT_TOOLS
#define EE_DEVELOPMENT_TOOLS_LINE_IN_MACRO( x ) x
#define EE_DEVELOPMENT_TOOLS_ONLY( x ) x
#else
#define EE_DEVELOPMENT_TOOLS_LINE_IN_MACRO( x )
#define EE_DEVELOPMENT_TOOLS_ONLY( x )
#endif

//-------------------------------------------------------------------------
// Platform
//-------------------------------------------------------------------------

namespace EE::Platform
{
    enum class Target
    {
        PC = 0,
    };
}

#if _WIN32
#include "Platform/Platform_Win32.h"
#endif

//-------------------------------------------------------------------------
// Logging
//-------------------------------------------------------------------------

#include "Logging/Log.h"

//-------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------

#define InvalidIndex -1

// Platform agnostic asserts
//-------------------------------------------------------------------------

// TODO: macr with comma in C++ with result in error macro expand
// (e.g. for some type contains std::is_same_v<A, B> which contains a comma,
//  and macro will expand that to cond = "is_same_v<A" and error = " B>")
// To correct this, we have several solutions:
// 
// 1. Use macro to prevent macro expand.
// #define COMMA ,
// EE_STATIC_ASSERT( std::is_same_v<A COMMA B>, error_msg )
// 
// this way macro expand will separate comma correctly.
//
// Or something like:
// #define SINGLE_ARG(...) __VA_ARGS__
// EE_STATIC_ASSERT( SINGLE_ARG( std::is_same_v<A, B> ), error_msg )
// 
// 2. User takes care.
// User just wrap cond with a parentheses.
// 
// EE_STATIC_ASSERT( ( std::is_same_v<A, B> ), error_msg )
// 
#define EE_STATIC_ASSERT( cond, error ) static_assert( cond, error )

#if EE_DEVELOPMENT_TOOLS

    #define EE_ASSERT( cond ) do { if( !(cond) ) { EE::Log::LogAssert( __FILE__, __LINE__, #cond ); EE_DEBUG_BREAK(); } } while( 0 )
    #define EE_HALT() { EE::Log::LogAssert( __FILE__, __LINE__, "HALT" ); EE_DEBUG_BREAK(); }
    #define EE_TRACE_MSG( msgFormat, ... ) EE::Log::TraceMessage( msgFormat, __VA_ARGS__ )
    #define EE_TRACE_HALT( msgFormat, ... ) { EE::Log::LogAssert( __FILE__, __LINE__, msgFormat, __VA_ARGS__ ); EE_DEBUG_BREAK(); }
    #define EE_UNIMPLEMENTED_FUNCTION() EE_TRACE_HALT( "Function not implemented!" )
    #define EE_UNREACHABLE_CODE() EE_TRACE_HALT( "Unreachable code encountered!" )

#else

    // Platform specific, need to be defined in Platform/Defines_XXX.h
    #define EE_ASSERT( cond ) do { (void)sizeof( cond );} while (0)
    #define EE_HALT()
    #define EE_TRACE_MSG( msgFormat, ... )
    #define EE_TRACE_HALT( msgFormat, ... )
    #define EE_UNIMPLEMENTED_FUNCTION()
    #define EE_UNREACHABLE_CODE()

#endif