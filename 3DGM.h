////////////////////////////////////////////////////////////////////////////////
// 3DGM.h                                                                     //
// Header for the implementation of the Direct3D class in a DLL               //
////////////////////////////////////////////////////////////////////////////////

#ifndef ___3DGM_H___INCLUDED___
#define ___3DGM_H___INCLUDED___

// Defines /////////////////////////////////////////////////////////////////////
#define GM_EXPORT __declspec(dllexport) double __cdecl

// Includes ////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <Float.h>
#include <STDIO.h>
#define D3D_OVERLOADS
#include <D3D.h>
#include "D3DFile.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "Direct3D.h"

// Typedefs ////////////////////////////////////////////////////////////////////
typedef double	GM_VAL;
typedef LPSTR	GM_STR;

// Globals /////////////////////////////////////////////////////////////////////
// The window to be created under the GM parent
HWND		Window;
// Store the location of the viewer
D3DVECTOR	ViewPos;
// Flag set to TRUE when direction is being used and false when pointing is
BOOL		DirectionalCamera = FALSE;
// The font that text is drawn in
HFONT		Font	= 0;
BYTE		D3DTEXT_Red,
			D3DTEXT_Green,	
			D3DTEXT_Blue;
// The master Direct3D object
Direct3D*	D3D		= 0;

// DXFile pointers
CONST DWORD	MAX_DXFILES	= 512 + 1;
// Index zero is reserved for error return
CD3DFile*	DXFiles[MAX_DXFILES];

// Terrain randomizer
FLOAT		TERRAIN_Slope,
			TERRAIN_BumpFactor,
			TERRAIN_BumpHeight,
			TERRAIN_Randomness;

// Render Queueing
CONST DWORD MAX_RENDERS = 2048;
struct _RenderQueue {
	union {
		LPSTR BillboardName;
		DWORD DXFileIndex;
	} ;// end union
	D3DVECTOR Trans, Rot, Scale;
	DWORD Flags;
	enum {
		DISABLEZBUF		= 1,
		USECOLORALPHA	= 2,
		BLACKRENDER		= 4,
		BILLBOARD		= 8,
		DXFILE			= 16,
		RENDERFIRST		= 32,
		FORCE_DWORD	= 0xffffffff
	}; // end enum
}* RenderQueue[MAX_RENDERS];
DWORD NumRenders = 0;
LPSTR CurrentBillboard = 0;

// Prototypes //////////////////////////////////////////////////////////////////
GM_EXPORT D3D_Initialize(GM_VAL,	// Parent window
						 GM_VAL);	// Whether to initialize in freeform mode, 0 = no, 1 = yes
GM_EXPORT D3D_Shutdown();
GM_EXPORT D3D_BeginScene(GM_VAL,	// Eye X
						 GM_VAL,	// Eye Y
						 GM_VAL,	// Eye Z
						 GM_VAL,	// Look at X
						 GM_VAL,	// Look at Y
						 GM_VAL);	// Look at Z
GM_EXPORT D3D_EndScene();
GM_EXPORT D3D_SetCamera(GM_VAL,		// Eye X
						GM_VAL,		// Eye Y
						GM_VAL,		// Eye Z
						GM_VAL,		// X rotation
						GM_VAL,		// Y rotation
						GM_VAL);	// Z rotation
GM_EXPORT D3D_SetFog(GM_VAL,	// Fog start
				     GM_VAL,	// Fog end
				     GM_VAL,	// Red
				     GM_VAL,	// Green
				     GM_VAL);	// Blue
GM_EXPORT D3D_SetAmbient(GM_VAL,	// Red
					     GM_VAL,	// Green
					     GM_VAL);	// Blue
GM_EXPORT D3D_SetLight(GM_VAL,	// Index of the light to be set, 0 mean to make a new one
					   GM_VAL,	// The type of light to create, 0 = directional, 1 = point, 2 = spot
					   GM_VAL,	// RGB color, can be assembled by a GM make_color
					   GM_VAL,	// X
					   GM_VAL,	// Y
					   GM_VAL,	// Z
					   GM_VAL,	// X rotation
					   GM_VAL,	// Y rotation
					   GM_VAL,	// Z rotation
					   GM_VAL);	// Range
GM_EXPORT D3D_SetLightCones(GM_VAL,		// Index of the light to set the cones for
							GM_VAL,		// Angle, if this is a spotlight, of the inner cone
							GM_VAL);	// Angle, if this is a spotlight, of the outer cone
GM_EXPORT D3D_SetLightEnable(GM_VAL,	// Index of light to set (0 means all)
						     GM_VAL);	// New state, (1 = on, 0 = off)
GM_EXPORT D3D_GetLightEnable(GM_VAL);	// Index of light to check
GM_EXPORT D3D_LoadDXFile(GM_STR);	// Name of file to load from
GM_EXPORT D3D_RenderDXFile(GM_VAL,	// Index of DXFile to render
						   GM_VAL,	// x
						   GM_VAL,	// y
						   GM_VAL,	// z
						   GM_VAL,	// x axis rotation
						   GM_VAL,	// y axis rotation
						   GM_VAL,	// z axis rotation
						   GM_VAL,	// x axis scaling
						   GM_VAL,	// y axis scaling
						   GM_VAL,	// z axis scaling
						   GM_VAL);	// Flags
GM_EXPORT D3D_FindPointAboveTerrain(GM_VAL,		// Terrain DXFile to find point above
									GM_VAL,		// X coordinate to find the point above
									GM_VAL);	// Y coordinate to find the point above
GM_EXPORT D3D_RandomizeTerrain(GM_VAL,	// Index of the terrain DXFile to randomize
							   GM_VAL,	// Slope
							   GM_VAL,	// Bump factor
							   GM_VAL,	// Bump height
							   GM_VAL);	// Randomness
GM_EXPORT D3D_UnloadDXFile(GM_VAL);	// Index of DXFile to unload
GM_EXPORT D3D_UnloadAllDXFiles();
GM_EXPORT D3D_LoadBillboard(GM_STR);	// Filename of the texture
GM_EXPORT D3D_DrawBillboard(GM_VAL,		// x
							GM_VAL,		// y
							GM_VAL,		// z
							GM_VAL,		// x axis rotation
						    GM_VAL,		// y axis rotation
						    GM_VAL,		// z axis rotation
						    GM_VAL,		// x size
						    GM_VAL,		// y size
							GM_VAL);	// Flags
GM_EXPORT D3D_UnloadBillboard(GM_STR); // Filename of the texture
GM_EXPORT D3D_SetFont(GM_STR,	// Font name
					  GM_VAL,	// Width
					  GM_VAL,	// Height
					  GM_VAL);	// Angle  (degrees)
GM_EXPORT D3D_DrawText(GM_VAL,	// X
					   GM_VAL,	// Y
					   GM_VAL,	// Is centered boolean
					   GM_STR);	// Text
GM_EXPORT D3D_SetTextColor(GM_VAL,	// Red
						   GM_VAL,	// Green
						   GM_VAL);	// Blue
GM_EXPORT D3D_SetCullMode(GM_VAL); // on\off boolean

// Callbacks ///////////////////////////////////////////////////////////////////
FLOAT FPAT_X, FPAT_Z;
FindPointAboveTerrainCB(CD3DFileObject*,
						D3DMATRIX*,
						VOID*);
RandomizeTerrainCB(CD3DFileObject*,
				   D3DMATRIX*,
				   VOID*);

#endif