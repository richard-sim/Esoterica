#pragma once

#include "Base/Types/String.h"

#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <stringapiset.h>

namespace EE
{
    using WString = eastl::basic_string<wchar_t>;
    template<eastl_size_t S> using TInlineWString = eastl::fixed_string<wchar_t, S, true>;
    using InlineWString = eastl::fixed_string<wchar_t, 255, true>;

    //-------------------------------------------------------------------------

    namespace StringUtils
    {
        // TODO: safety
        inline bool StringToWString( String const& string, WString& outWString )
        {
            int byteSize = MultiByteToWideChar( CP_UTF8, 0, string.c_str(), (int)strlen( string.c_str() ), NULL, 0 );
            if ( byteSize < 0 )
            {
                return false;
            }

            outWString.resize( (size_t)byteSize + 1 );
            int res = MultiByteToWideChar( CP_UTF8, 0, string.c_str(), (int)strlen( string.c_str() ), outWString.data(), (int)outWString.size() );
            if ( res < 0 )
            {
                return false;
            }

            return true;
        }
    }
}