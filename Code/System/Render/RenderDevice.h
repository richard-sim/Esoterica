#pragma once

//-------------------------------------------------------------------------

#include "RenderAPI.h"

#ifdef _WIN32
#include "Platform/DX11/RenderDevice_DX11.h"
#else
#error Invalid render backend
#endif