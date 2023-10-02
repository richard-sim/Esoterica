#include "Time.h"
#include <EASTL/chrono.h>

//-------------------------------------------------------------------------

namespace EE
{
    EE::Nanoseconds EngineClock::CurrentTime = 0;

    //-------------------------------------------------------------------------

    Nanoseconds::operator Microseconds() const
    {
        auto const duration = eastl::chrono::duration<uint64_t, eastl::chrono::steady_clock::period>( m_value );
        uint64_t const numMicroseconds = eastl::chrono::duration_cast<eastl::chrono::microseconds>( duration ).count();
        return float( numMicroseconds );
    }

    //-------------------------------------------------------------------------

    Nanoseconds PlatformClock::GetTime()
    {
        auto const time = eastl::chrono::high_resolution_clock::now();
        uint64_t const numNanosecondsSinceEpoch = time.time_since_epoch().count();
        return Nanoseconds( numNanosecondsSinceEpoch );
    }

    //-------------------------------------------------------------------------

    void EngineClock::Update( Milliseconds deltaTime )
    {
        EE_ASSERT( deltaTime >= 0 );
        CurrentTime += deltaTime.ToNanoseconds();
    }
}