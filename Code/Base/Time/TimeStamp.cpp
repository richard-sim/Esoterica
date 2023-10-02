#include "TimeStamp.h"
#include "Base/Math/Math.h"
#include <cmath>
#include <time.h>

//-------------------------------------------------------------------------

namespace EE
{
    TimeStamp::TimeStamp()
    {
        Update();
    }

    void TimeStamp::Update()
    {
        // EASTL doesn't have to_time_t
        const eastl::chrono::system_clock::time_point tp = eastl::chrono::system_clock::now();
        m_time = eastl::chrono::duration_cast<eastl::chrono::seconds>( tp.time_since_epoch() ).count();
        // m_time = std::chrono::system_clock::to_time_t( stl::chrono::system_clock::now() );
    }

    String TimeStamp::GetTime() const
    {
        tm* pTimeinfo = localtime( &m_time );

        String s;
        s.resize( 9 );
        strftime( s.data(), 9, "%H:%M:%S", pTimeinfo );
        return s;
    }

    EE::String TimeStamp::GetTimeDetailed() const
    {
        tm* pTimeinfo = localtime( &m_time );

        // TODO: get millisecond precision
        String s;
        s.resize( 12 );
        strftime( s.data(), 12, "%H:%M:%S", pTimeinfo );
        return s;
    }

    String TimeStamp::GetDayMonthYear() const
    {
        tm* pTimeinfo = localtime( &m_time );

        String s;
        s.resize( 11 );
        strftime( s.data(), 11, "%d/%m/%Y", pTimeinfo );
        return s;
    }

    String TimeStamp::GetDayMonth() const
    {
        tm* pTimeinfo = localtime( &m_time );

        String s;
        s.resize( 6 );
        strftime( s.data(), 6, "%d/%m", pTimeinfo );
        return s;
    }

    String TimeStamp::GetDateTime() const
    {
        tm* pTimeinfo = localtime( &m_time );

        String s;
        s.resize( 21 );
        strftime( s.data(), 21, "%Y/%m/%d %H:%M:%S", pTimeinfo );
        return s;
    }
}