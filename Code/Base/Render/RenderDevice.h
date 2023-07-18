#pragma once

//-------------------------------------------------------------------------

#include "RenderAPI.h"

#if defined(_WIN32) && defined(EE_DX11)
#include "Platform/DX11/RenderDevice_DX11.h"
#else
#error Invalid render backend
#endif