/*==============================================================================

	PHC Header For Programe [Project_Header.h]

	Author : Choi HyungJoon

==============================================================================*/
#pragma once

//--------------------- Windows API ---------------------//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <Xinput.h>
#include <xaudio2.h>
#include <winsock2.h>

//--------------------- C/C++ Standard Library ---------------------//
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <list>
#include <random>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <map>
#include <mutex>
#include <chrono>
#include <functional>

//--------------------- DirectX ---------------------//
#include <sdkddkver.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>