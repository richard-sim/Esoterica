#if defined(EE_VULKAN) && defined(_WIN32)
#include "../VulkanUtils.h"

#include "Base/Application/Platform/Application_Win32.h"

#include <windows.h>

namespace EE::Render
{
	namespace Backend::Util
	{
		Int2 GetCurrentActiveWindowUserExtent( Application* pApplication )
		{
            Win32Application* pWIn32Application = static_cast<Win32Application*>( pApplication );

			HWND hwnd = pWIn32Application->GetWindowHandle();

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