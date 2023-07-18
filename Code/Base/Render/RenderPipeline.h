#pragma once

#include <stdint.h>

namespace EE
{
    namespace Render
    {
        enum class PipelineType : uint8_t
        {
            Raster = 0,
            Compute,
            Transfer,

            RayTracing,
        };
    }
}