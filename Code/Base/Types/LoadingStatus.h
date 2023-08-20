#pragma once

//-------------------------------------------------------------------------

namespace EE
{
    enum class LoadingStatus : uint8_t
    {
        Unloaded = 0,
        Loading,
        Loaded,
        Unloading,
        Failed,
    };
}