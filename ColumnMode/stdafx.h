// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Can't use #define WIN32_LEAN_AND_MEAN, it'll exclude print controls. Oh well.
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here

#include <commdlg.h> // Needs to be explicitly included because of LEAN_AND_MEAN
#include <Shlobj.h>
#include <assert.h>

#include <d2d1_1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <Commdlg.h>
#include <prntvpt.h>
#include <documenttarget.h>
#include <wincodec.h>

#include <wrl/client.h>
#include <windowsx.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "ColumnModePluginApi.h"
#include "MenuHelper.h"

using namespace ::Microsoft::WRL;
