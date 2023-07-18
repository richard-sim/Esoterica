#if defined(EE_VULKAN) && defined(_WIN32)
#include "../VulkanUtils.h"

#include <windows.h>

namespace EE::Render
{
	namespace Backend::Util
	{
		Int2 GetCurrentActiveWindowUserExtent()
		{
			HWND hwnd = GetActiveWindow();

			if ( hwnd == nullptr )
			{
				EE_HALT();
			}

			RECT rect = {};
			GetClientRect( hwnd, &rect );

			return Int2 { rect.right - rect.left, rect.bottom - rect.top };
		}
	}
}

#endif