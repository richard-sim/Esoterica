#pragma once

namespace EE::RHI
{
    class RHISwapchain
    {
    public:

        RHISwapchain() = default;
        virtual ~RHISwapchain() = default;

        RHISwapchain( RHISwapchain const& ) = delete;
        RHISwapchain& operator=( RHISwapchain const& ) = delete;

        RHISwapchain( RHISwapchain&& ) = default;
        RHISwapchain& operator=( RHISwapchain&& ) = default;

    private:
    };

}


