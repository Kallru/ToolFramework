//------------------------------------------------------------------------------
// File. StdAfx.h
//------------------------------------------------------------------------------

#ifndef _STDAFX_
#define _STDAFX_

// CORE_API 전처리기
#ifdef CLIENT_EXPORT
#define CORE_API __declspec(dllexport)
#else
#ifdef CLIENT_IMPORT
#define CORE_API __declspec(dllimport)
#else
#define CORE_API
#endif
#endif

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <ASSERT.H>
#include <tchar.h>

#include <D3DX9.h>
#include <D3D9.h>
//#include <dxerr9.h>
#include <d3d9types.h>
#include <d3dx9shader.h>

#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>
#include <map>
#include <list>
#include <stack>
#include <queue>

using namespace std;

#include "DXUtil.h"
#include "Utility.h"

#include "BmpFile.h"
#include "DrawEx.h"

#include "Timer.h"
#include "D3DApp.h"
#include "GameMain.h"
#include "XFile.h"
#include "Camera.h"
#include "Player.h"
#include "Model.h"
#include "Mesh.h"
#include "Picking.h"

#include "Effect.h"
#include "Effect_Fire.h"
#include "Effect_FireBroken.h"

#include "Render.h"
#include "Render_Cartoon.h"

#include "ZFrustum.h"
#include "ZQuadTree.h"
#include "ZTerrain.h"

#endif