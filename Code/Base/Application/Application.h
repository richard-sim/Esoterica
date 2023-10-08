#pragma once

#include "Base/Esoterica.h"

namespace EE
{
    class EE_BASE_API Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

    protected:

        // Initialize/Shutdown
        virtual bool Initialize() = 0;
        virtual bool Shutdown() = 0;

        // The actual application loop
        virtual bool ApplicationLoop() = 0;
    };
}