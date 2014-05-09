////////////////////////////////////////////////////////////////////////////////
// 3DGM.cpp                                                                   //
// Implementation of the Direct3D class in a DLL                              //
////////////////////////////////////////////////////////////////////////////////

// Includes ////////////////////////////////////////////////////////////////////
#include "3DGM.h"

// Definitions /////////////////////////////////////////////////////////////////
GM_EXPORT D3D_Initialize(GM_VAL WindowParent,	// Parent window
						 GM_VAL FreeForm)		// Whether to initialize in freeform mode, 0 = no, 1 = yes
{
	// Initialize Direct3D if it is not already initialized
	if(!D3D)
	{
		// Clear all mesh pointers
		for(DWORD i = 0; i < MAX_DXFILES; i++)
			DXFiles[i] = 0;

		D3D = new Direct3D();
		
		// Could we not get the pointer?
		if(!D3D)
			return(0); // Return failure

		// Create our window under the GM window
		WNDCLASS W;
		memset(&W, 0, sizeof(W));
		W.lpszClassName = "W";
		W.lpfnWndProc = DefWindowProc;
		W.hInstance = GetModuleHandle(0);
		RegisterClass(&W);
		Window = CreateWindow("W",
							  "3DGM",
							  WS_POPUP,
							  0,
							  0,
							  GetSystemMetrics(SM_CXSCREEN),
							  GetSystemMetrics(SM_CYSCREEN),
							  (HWND)((INT)WindowParent),
							  0,
							  GetModuleHandle(0),
							  0);

		if(0 != FreeForm)
		{
			// minimize the parent window
			CloseWindow((HWND)(INT)WindowParent);
			CloseWindow(Window);

			// make GDI repaint the screen
			InvalidateRect(0, 0, FALSE);

			// Make time for minimizing and redraw
			Sleep(500);
		} // end if

		// Initialize Direct3D
		if(FAILED(D3D->Init(Window,
							GetSystemMetrics(SM_CXSCREEN),
							GetSystemMetrics(SM_CYSCREEN),
							(FreeForm == 0) ? FALSE : TRUE)))
			return(0); // Return failure

		// Give input back to the GM window
		SetActiveWindow((HWND)(INT)WindowParent);

		// Init the render queue
		for(i = 0; i < MAX_RENDERS; i++)
			RenderQueue[i] = new _RenderQueue;

		ShowCursor(0);
	} // end if

	// Return success
	return(1);
} // end D3D_Initialize

GM_EXPORT D3D_Shutdown()
{
	#define SafeDelete(P) {if(P) {delete P; P = 0;}}

	// Destroy the render queue
	for(DWORD i = 0; i < MAX_RENDERS; i++)
		delete RenderQueue[i];

	// Free all of the textures
	D3DTextr_InvalidateAllTextures();

	// Get rid of all mesh pointers
	D3D_UnloadAllDXFiles();

	// Dump Direct3D
	SafeDelete(D3D);

	// Destroy our window
	DestroyWindow(Window);

	// Return success
	return(1);
} // end D3D_Shutdown

GM_EXPORT D3D_BeginScene(GM_VAL EyeX,	// Eye X
						 GM_VAL EyeY,	// Eye Y
						 GM_VAL EyeZ,	// Eye Z
						 GM_VAL LookX,	// Look at X
						 GM_VAL LookY,	// Look at Y
						 GM_VAL LookZ)	// Look at Z
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	// Load the coords into D3DVECTORs to pass to Direct3D::BeginScene
	static D3DVECTOR	Eye;
	Eye.x = EyeX;
	Eye.y = EyeY;
	Eye.z = EyeZ;
	static D3DVECTOR	Look;
	Look.x = LookX;
	Look.y = LookY;
	Look.z = LookZ;

	ViewPos = Eye;

	// We are in point mode right now
	DirectionalCamera = FALSE;

	// Now begin the rendering
	if(FAILED(D3D->BeginScene(Eye,
							  Look)))
		return(0); // Return failure

	// Return success
	return(1);
} // end D3D_BeginScene

// For "QSorting" the render list by distance
INT RenderCompareCB(CONST VOID* Arg1, CONST VOID* Arg2)
{
	if((*(_RenderQueue**)Arg1)->Flags & (DWORD)_RenderQueue::RENDERFIRST)
		return(-1);

	static D3DVECTOR Dif;
	static FLOAT Dist1, Dist2;
	Dif		= ViewPos - (*(_RenderQueue**)Arg1)->Trans;
	Dist1	= Magnitude(Dif);
	Dif		= ViewPos - (*(_RenderQueue**)Arg2)->Trans;
	Dist2	= Magnitude(Dif);

	if(Dist1 > Dist2)
		return(-1);
	if(Dist2 > Dist1)
		return(1);
	else
		return(0);
} // end RenderCompareCB

// Just a producure to be placed in D3D_EndScene
VOID ___Render()
{
	// Add dummy boards to the queue, to get rid of sorting and render problems,
	// it's a stupid way to do it, but oh well...
	for(DWORD i = 0; i < 0; i++)
	{
		RenderQueue[NumRenders]->BillboardName = 0;
		RenderQueue[NumRenders]->Trans = ViewPos;
		RenderQueue[NumRenders]->Rot = D3DVECTOR(0.0f, 0.0f, 0.0f);
		RenderQueue[NumRenders]->Scale = D3DVECTOR(0.0f, 0.0f, 0.0f);
		RenderQueue[NumRenders]->Flags = _RenderQueue::BILLBOARD;
		NumRenders++;
	} // end for adding dummy boards to the queue
	// This is a one by one billboard mesh with all vertices prelit to the max cuz it doesn't
	// really make sense to have them be effected by world lighting
	static D3DLVERTEX Billboard[4] =
	{D3DLVERTEX(D3DVECTOR(-0.5f, -0.5f, 0), 0xffffffff, 0, 0.0f, 1.0f),
	 D3DLVERTEX(D3DVECTOR(-0.5f,  0.5f, 0), 0xffffffff, 0, 0.0f, 0.0f),
	 D3DLVERTEX(D3DVECTOR( 0.5f, -0.5f, 0), 0xffffffff, 0, 1.0f, 1.0f),
	 D3DLVERTEX(D3DVECTOR( 0.5f,  0.5f, 0), 0xffffffff, 0, 1.0f, 0.0f)};
	// Sort the renders by distance
	qsort(RenderQueue, NumRenders, sizeof(_RenderQueue*), RenderCompareCB);
	// Now render them all
	for(i = 0; i < NumRenders; i++)
	{
		// Set up the matrices for the render
		static D3DMATRIX World, Trans, r, Rot;
		SetScaleMatrix(World, RenderQueue[i]->Scale);
		D3DUtil_SetIdentityMatrix(Rot);
		if(0.0 != RenderQueue[i]->Rot.x) {
			D3DUtil_SetRotateXMatrix(r, RenderQueue[i]->Rot.x);
			D3DMath_MatrixMultiply(Rot, Rot, r);
		} // end if
		if(0.0 != RenderQueue[i]->Rot.y) {
			D3DUtil_SetRotateYMatrix(r, RenderQueue[i]->Rot.y);
			D3DMath_MatrixMultiply(Rot, Rot, r);
		} // end if
		if(0.0 != RenderQueue[i]->Rot.z) {
			D3DUtil_SetRotateZMatrix(r, RenderQueue[i]->Rot.z);
			D3DMath_MatrixMultiply(Rot, Rot, r);
		} // end if
		D3DMath_MatrixMultiply(World, World, Rot);
		D3DUtil_SetTranslateMatrix(Trans, RenderQueue[i]->Trans);
		D3DMath_MatrixMultiply(World, World, Trans);
		// Set the render matrix to the world
		D3D->GetD3DDevicePtr()->SetTransform(D3DTRANSFORMSTATE_WORLD, &World);
		// Now setup for this particular render
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DISABLEZBUF)
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BILLBOARD)
		{
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			D3D->GetD3DDevicePtr()->SetTexture(0, D3DTextr_GetSurface(RenderQueue[i]->BillboardName));
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
		} // end if
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::USECOLORALPHA)
		{
			if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DXFILE)
				D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		} // end if
		else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BLACKRENDER)
		{
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
			if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DXFILE)
				D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
			else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BILLBOARD)
				D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		} // end if
		else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BILLBOARD)
		{
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		} // end if
		else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DXFILE)
		{
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		} // end if
	
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DXFILE)
			DXFiles[RenderQueue[i]->DXFileIndex]->Render(D3D->GetD3DDevicePtr());
		else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BILLBOARD)
			D3D->GetD3DDevicePtr()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
												  D3DFVF_LVERTEX,
												  Billboard,
												  4,
												  0);

		// Now de-setup for this particular render
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::DISABLEZBUF)
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::BILLBOARD)
		{
			D3D->GetD3DDevicePtr()->SetTexture(0, 0);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
		} // end if
		else if(RenderQueue[i]->Flags & (DWORD)_RenderQueue::USECOLORALPHA)
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	} // end for
	NumRenders = 0;
} // end ___Render

GM_EXPORT D3D_EndScene()
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	___Render();

	// Render the scene
	if(FAILED(D3D->EndScene()))
		return(0); // Return failure

	// Return success
	return(1);
} // end D3D_EndScene

GM_EXPORT D3D_SetCamera(GM_VAL X,		// Eye X
						GM_VAL Y,		// Eye Y
						GM_VAL Z,		// Eye Z
						GM_VAL RotX,	// X rotation
						GM_VAL RotY,	// Y rotation
						GM_VAL RotZ)	// Z rotation
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	ViewPos = D3DVECTOR(X, Y, Z);

	// We are now in directional mode
	DirectionalCamera = TRUE;

	// Calculate the view matrix
	static D3DMATRIX View, Trans, rx, ry, rz;
	D3DUtil_SetRotateXMatrix(rx, RotX);
	D3DUtil_SetRotateYMatrix(ry, RotY);
	D3DUtil_SetRotateZMatrix(rz, RotZ);
	D3DUtil_SetTranslateMatrix(Trans, -X, -Y, -Z);
	View = Trans * ry * rx * rz;
	D3D->GetD3DDevicePtr()->SetTransform(D3DTRANSFORMSTATE_VIEW, &View);

	// Return success
	return(1);
} // end D3D_SetCamera

GM_EXPORT D3D_SetFog(GM_VAL FogStart,	// Fog start
				     GM_VAL FogEnd,		// Fog end
				     GM_VAL Red,		// Red
				     GM_VAL Green,		// Green
				     GM_VAL Blue)		// Blue
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	// Turn on the fog
	if(FAILED(D3D->SetFog(FogStart,
						  FogEnd,
						  Red,
						  Green,
						  Blue)))
		return(0); // Return failure

	// Return success
	return(1);
} // end D3D_SetFog

GM_EXPORT D3D_SetAmbient(GM_VAL Red,	// Red
					     GM_VAL Green,	// Green
					     GM_VAL Blue)	// Blue
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	if(FAILED(D3D->SetAmbient(Red,
							  Green,
							  Blue)))
		return(0); // Return failure

	// Return success
	return(1);
} // end D3D_SetAmbient

GM_EXPORT D3D_SetLight(GM_VAL Index,	// Index of the light to be set, 0 mean to make a new one
					   GM_VAL Type,		// The type of light to create, 0 = directional, 1 = point, 2 = spot
					   GM_VAL Color,	// RGB color, can be assembled by a GM make_color
					   GM_VAL X,		// X
					   GM_VAL Y,		// Y
					   GM_VAL Z,		// Z
					   GM_VAL RotX,		// X rotation
					   GM_VAL RotY,		// Y rotation
					   GM_VAL RotZ,		// Z rotation
					   GM_VAL Range)	// Range
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	// The light index must be passed as a DWORD&
	static DWORD LightIndex;
	LightIndex = Index;

	// Extract the color info from the GM_VAL, normalize
	static DWORD rgb	= 0;
	rgb = Color;
	static FLOAT Red	= 0,
				 Green	= 0,
				 Blue	= 0;
	Red		= (FLOAT)GetRValue(rgb) / 255.0f;
	Green	= (FLOAT)GetGValue(rgb) / 255.0f;
	Blue	= (FLOAT)GetBValue(rgb) / 255.0f;

	// Load the coords and rotations into D3DVECTORs so that they
	// can be passed to Direct3D::SetLight
	D3DVECTOR Pos(X, Y, Z);

	// Create a rotation vector
	static D3DMATRIX r, rx, rz;
	D3DMATRIX ry(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	D3DUtil_SetRotateXMatrix(rx, RotX);
	D3DUtil_SetRotateYMatrix(ry, RotY);
	D3DUtil_SetRotateZMatrix(rz, RotZ);
	r = rx * ry * rz;
	D3DMath_MatrixMultiply(r, rx, ry);
	D3DMath_MatrixMultiply(r, r, rz);
	static D3DVECTOR Rot;
	D3DMath_VectorMatrixMultiply(Rot, D3DVECTOR(1, 1, 1), r);

	// Translate the light type to a D3DLIGHTTYPE
	static D3DLIGHTTYPE LightType;
	if(0.0 == Type)
		LightType = D3DLIGHT_DIRECTIONAL;
	else if(1.0 == Type)
		LightType = D3DLIGHT_POINT;
	else if(2.0 == Type)
		LightType = D3DLIGHT_SPOT;
	else // Unrecognized light type!
		return(0); // Return failure

	// Now we can set the light
	if(FAILED(D3D->SetLight(LightIndex,
							LightType,
							Red,
							Green,
							Blue,
							Pos,
							Rot,
							Range,
							0.74,
							1.54)))
		return(0); // Return failure

	// Return the index of the light
	return(LightIndex);
} // end D3D_SetLight

GM_EXPORT D3D_SetLightCones(GM_VAL Index,	// Index of the light to set the cones for
							GM_VAL Theta,	// Angle of the inner cone
							GM_VAL Phi)		// Angle of the outer cone
{
	// Make sure we have a valid Direct3D pointer
	if(!D3D)
		return(0);

	// Get the state of the current light at the index
	D3DLIGHT7 Light;
	if(FAILED(D3D->GetD3DDevicePtr()->GetLight(Index, &Light)))
		return(0);

	// Set the cones
	Light.dvTheta	= Theta;
	Light.dvPhi		= Phi;

	// Now set the light again with the new cone angles
	if(FAILED(D3D->GetD3DDevicePtr()->SetLight(Index, &Light)))
		return(0);

	// Return success
	return(1);
} // end D3D_SetLightCones

GM_EXPORT D3D_SetLightEnable(GM_VAL Index,	// Index of light to set (0 means all)
						     GM_VAL State)	// New state, (1 = on, 0 = off)
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(0); // Return failure

	// Set the light's state
	if(FAILED(D3D->SetLightEnable(Index,
								  (1 == State) ? TRUE : FALSE)))
		return(0); // Return failure

	// Return success
	return(1);
} // end D3D_SetLightEnable

GM_EXPORT D3D_GetLightEnable(GM_VAL Index)	// Index of light to check
{
	// Make sure Direct3D has been initialized
	if(!D3D)
		return(-1); // Return failure

	// Stores whether the light is on
	BOOL IsOn;

	// Get the light's state
	if(FAILED(D3D->GetLightEnable(Index,
								  &IsOn)))
		return(-1); // Return failure

	// Return light state
	return(IsOn ? TRUE : FALSE);
} // end D3D_GetLightEnable

GM_EXPORT D3D_LoadDXFile(GM_STR FileName)	// Name of file to load from
{
	// Find an open spot in the array DXFile pointers and create the mesh in it
	for(DWORD i = 1; i < MAX_DXFILES; i++)
	{
		// Pass filled array indexes
		if(DXFiles[i])
			continue;

		// We have a good array position, so load the D3DFile
		DXFiles[i] = new CD3DFile();
		if(!DXFiles[i])
			return(0);
		if(FAILED(DXFiles[i]->Load(FileName)))
			return(0); // Return failed		  

		// Return the index of the D3DFile created
		return(i);
	} // end for

	// If it got here, the array must be full
	return(0);
} // end D3D_LoadDXFile

GM_EXPORT D3D_RenderDXFile(GM_VAL Index,	// Index of DXFile to render
						   GM_VAL X,		// x
						   GM_VAL Y,		// y
						   GM_VAL Z,		// z
						   GM_VAL RotX,		// x axis rotation
						   GM_VAL RotY,		// y axis rotation
						   GM_VAL RotZ,		// z axis rotation
						   GM_VAL ScaleX,	// x axis scaling
						   GM_VAL ScaleY,	// y axis scaling
						   GM_VAL ScaleZ,	// z axis scaling
						   GM_VAL Flags)	// Flags
{
	// Make sure we have a valid Direct3D object and DXFile
	if(!D3D || !DXFiles[(INT)Index])
		return(0);

	// Queue up the render
	if(NumRenders + 1 > MAX_RENDERS)
		return(0);
	RenderQueue[NumRenders]->DXFileIndex	=	Index;
	RenderQueue[NumRenders]->Trans			=	D3DVECTOR(X, Y, Z);
	RenderQueue[NumRenders]->Rot			=	D3DVECTOR(RotX, RotY, RotZ);
	RenderQueue[NumRenders]->Scale			=	D3DVECTOR(ScaleX, ScaleY, ScaleZ);
	RenderQueue[NumRenders]->Flags			=	(DWORD)Flags | (DWORD)_RenderQueue::DXFILE;
	NumRenders++;

	// Return success
	return(1);
} // end D3D_RenderDXFile

GM_EXPORT D3D_FindPointAboveTerrain(GM_VAL TerrainIndex,	// Terrain DXFile to find point above
									GM_VAL X,				// X coordinate to find the point above
									GM_VAL Z)				// Z coordinate to find the point above
{
	static INT Index;
	Index = TerrainIndex;

	// Make sure the terrain index is valid
	if(!DXFiles[Index])
		return(0);

	// Set globals for the callback
	FPAT_X = X;
	FPAT_Z = Z;

	static FLOAT y;

	DXFiles[Index]->EnumObjects(&FindPointAboveTerrainCB, 0, (VOID*)&y);

	return y;
} // end D3D_FindPointAboveTerrain

GM_EXPORT D3D_RandomizeTerrain(GM_VAL TerrainIndex,	// Index of the terrain DXFile to randomize
							   GM_VAL Slope,		// Slope
							   GM_VAL BumpFactor,	// Bump factor
							   GM_VAL BumpHeight,	// Bump height
							   GM_VAL Randomness)	// Randomness
{
	static INT Index;
	Index = TerrainIndex;

	// Make sure the terrain index is valid
	if(!DXFiles[Index])
		return(0);

	// Set up the globals
	TERRAIN_Slope		= Slope;
	TERRAIN_BumpFactor	= BumpFactor;
	TERRAIN_BumpHeight	= BumpHeight;
	TERRAIN_Randomness	= Randomness;

	DXFiles[Index]->EnumObjects(&RandomizeTerrainCB, 0, 0);

	return(1);
} // end D3D_RandomizeTerrain

GM_EXPORT D3D_UnloadDXFile(GM_VAL Index)	// Index of DXFile to unload
{
	#define SafeDelete(P) {if(P) {delete P; P = 0;}}

	// Kill the DXFile
	SafeDelete(DXFiles[(INT)Index]);

	// Return success
	return(1);
} // end D3D_UnloadDXFile

GM_EXPORT D3D_UnloadAllDXFiles()
{
	#define SafeDelete(P) {if(P) {delete P; P = 0;}}

	// Kill all of the DXFiles
	for(DWORD i = 0; i < MAX_DXFILES; i++)
		SafeDelete(DXFiles[i]);

	// Return success
	return(1);
} // end D3D_UnloadAllDXFiles

GM_EXPORT D3D_LoadBillboard(GM_STR FileName)	// Filename of the texture
{
	// Load the texture
	if(FAILED(D3DTextr_CreateTextureFromFile(FileName, 0, D3DTEXTR_TRANSPARENTBLACK)))
		return(0);

	// Return success
	return(1);
} // end D3D_LoadBillboard

GM_EXPORT D3D_BeginBillboard(GM_STR TextureName) // Texture file name
{
	CurrentBillboard = TextureName;

	return(1);
} // end D3D_BeginBillboard

GM_EXPORT D3D_DrawBillboard(GM_VAL X,			// x
							GM_VAL Y,			// y
							GM_VAL Z,			// z
							GM_VAL RotX,		// x axis rotation
						    GM_VAL RotY,		// y axis rotation
						    GM_VAL RotZ,		// z axis rotation
						    GM_VAL SizeX,		// x size
						    GM_VAL SizeY,		// y size
							GM_VAL Flags)		// Flags
{
	// Make sure we have a valid Direct3D object and DXFile
	if(!D3D)
		return(0);

	// Queue up the render
	if(NumRenders + 1 > MAX_RENDERS)
		return(0);
	RenderQueue[NumRenders]->BillboardName	=	CurrentBillboard;
	RenderQueue[NumRenders]->Trans			=	D3DVECTOR(X, Y, Z);
	RenderQueue[NumRenders]->Rot			=	D3DVECTOR(RotX, RotY, RotZ);
	RenderQueue[NumRenders]->Scale			=	D3DVECTOR(SizeX, SizeY, 0);
	RenderQueue[NumRenders]->Flags			=	(DWORD)Flags | (DWORD)_RenderQueue::BILLBOARD;
	NumRenders++;

	// return success
	return(1);
} // end D3D_DrawBillboard

GM_EXPORT D3D_UnloadBillboard(GM_STR FileName) // Filename of the texture
{
	// Unload the texture
	if(FAILED(D3DTextr_DestroyTexture(FileName)))
		return(0);

	// Return success
	return(1);
} // end D3D_UnloadBillboard

GM_EXPORT D3D_SetFont(GM_STR FontName,	// Font name
					  GM_VAL Width,		// Width
					  GM_VAL Height,	// Height
					  GM_VAL Angle)		// Angle (degrees)
{
	// Get rid of the previous font, if any
	if(Font)
		DeleteObject(Font);

	// Create the font
	Font = CreateFont(	Width,
						Height,
						Angle * 10,
						Angle * 10,
						400,
						FALSE,
						FALSE,
						FALSE,
						DEFAULT_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH | (FF_DONTCARE << 2),
						FontName	);

	// Make sure the font was created
	if(0 == Font)
		return 0;

	// Return success
	return(1);
} // end D3D_SetFont

GM_EXPORT D3D_DrawText(GM_VAL X,			// X
					   GM_VAL Y,			// Y
					   GM_VAL IsCentered,	// Is centered boolean
					   GM_STR Text)			// Text
{
	// Make sure we have a valid Direct3D pointer
	if(!D3D)
		return(0);

	// Get a device context for the DirectDraw surface
	HDC XDC = 0;
	if(FAILED(D3D->GetPrimarySurfacePtr()->GetDC(&XDC)))
		return(0);

	// Set the font to ours
	SelectObject(XDC, Font);

	// Always transparent
	SetBkMode(XDC, TRANSPARENT);

	// Set the color to the global colors set by D3D_SetTextColor
	SetTextColor(XDC, RGB(D3DTEXT_Red, D3DTEXT_Green, D3DTEXT_Blue));

	// Center the text if neccessary
	INT x = X,
		y = Y;
	if(IsCentered) {
		// A SIZE to put the text extents into
		static SIZE TextSize;

		// Get the text size
		GetTextExtentPoint(XDC, Text, strlen(Text), &TextSize);

		// Center the text
		x -= TextSize.cx / 2;
		y -= TextSize.cy / 2;
	} // end if

	// Draw that text!
	TextOut(XDC, x, y, Text, strlen(Text));

	// Dunk the device context
	D3D->GetPrimarySurfacePtr()->ReleaseDC(XDC);

	// Return success
	return(1);
} // end D3D_DrawText

GM_EXPORT D3D_SetTextColor(GM_VAL r,	// Red
						   GM_VAL g,	// Green
						   GM_VAL b)	// Blue
{
	// Set the global font color variables
	D3DTEXT_Red		= r;
	D3DTEXT_Green	= g;
	D3DTEXT_Blue	= b;

	// Return success
	return(1);
} // end D3D_SetTextColor

GM_EXPORT D3D_SetCullMode(GM_VAL Mode) // on\off boolean
{
	// Make sure the Direct3D is initialized
	if(!D3D)
		return(0);

	// Return success or failure at setting the cull mode
	return(SUCCEEDED(D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_CULLMODE, 0 == Mode ? D3DCULL_NONE : D3DCULL_CCW)) ? 1 : 0);
} // end D3D_SetCullMode

GM_EXPORT D3D_SetZBufferState(GM_VAL On) // on\off boolean
{
	// Make sure the Direct3D is initialized
	if(!D3D)
		return(0);

	return
	(
		SUCCEEDED
		(
			D3D->GetD3DDevicePtr()->SetRenderState(D3DRENDERSTATE_ZENABLE, 0.0 == On ? FALSE : TRUE)
		) // end SUCCEEDED
	); // end return
} // end D3D_SetZBufferState

// Callbacks ///////////////////////////////////////////////////////////////////
FindPointAboveTerrainCB(CD3DFileObject* DXFileObj,
						D3DMATRIX*,
						VOID* Y)
{
	FLOAT* y = (FLOAT*)Y;

	static D3DVERTEX* Verts;
    static DWORD      NumVerts;

	// For storing the points of the nearest triangle
	FLOAT PointHeight[3];
	FLOAT PointDistance[3] = {FLT_MAX, FLT_MAX, FLT_MAX};

    if(SUCCEEDED(DXFileObj->GetMeshGeometry(&Verts, &NumVerts, 0, 0)))
    {
        for(DWORD i = 0; i < NumVerts; i += 2)
        {
            static FLOAT Dist, DifX, DifZ;
			DifX = Verts[i].x - FPAT_X;
			DifZ = Verts[i].z - FPAT_Z;
			Dist = sqrt(DifX * DifX + DifZ * DifZ);

			if(Dist < PointDistance[0])
			{
				PointDistance[2] = PointDistance[1];
				PointDistance[1] = PointDistance[0];
				PointDistance[0] = Dist;
				PointHeight[2] = PointHeight[1];
				PointHeight[1] = PointHeight[0];
				PointHeight[0] = Verts[i].y;
			} // end if
			else if(Dist < PointDistance[1])
			{
				PointDistance[2] = PointDistance[1];
				PointDistance[1] = Dist;
				PointHeight[2] = PointHeight[1];
				PointHeight[1] = Verts[i].y;
			} // end if
			else if(Dist < PointDistance[2])
			{
				PointDistance[2] = Dist;
				PointHeight[2] = Verts[i].y;
			} // end if
        } // end for

		if(0.01f > PointDistance[0])
		{
			*y = PointHeight[0];
			return TRUE;
		} // end if		 
		
		static FLOAT MinHeight, MaxHeight, DiffHeight;
		MinHeight = min(min(PointHeight[0], PointHeight[1]), PointHeight[2]);
		MaxHeight = max(max(PointHeight[0], PointHeight[1]), PointHeight[2]);
		DiffHeight = MaxHeight - MinHeight;

		/*
		HDC xdc;
		D3D->GetBackBufferPtr()->GetDC(&xdc);
		SetBkColor(xdc, 0);
		SetTextColor(xdc, RGB(0,255,0));
		char s[128];
		sprintf(s, "MaxHeight: %f", MaxHeight);
		TextOut(xdc, 0, 0 * 16, s, strlen(s));
		sprintf(s, "MinHeight: %f", MinHeight);
		TextOut(xdc, 0, 1 * 16, s, strlen(s));
		sprintf(s, "DiffHeight: %f", DiffHeight);
		TextOut(xdc, 0, 2 * 16, s, strlen(s));
		sprintf(s, "PointDistance[0]: %f", PointDistance[0]);
		TextOut(xdc, 0, 3 * 16, s, strlen(s));
		sprintf(s, "PointDistance[1]: %f", PointDistance[1]);
		TextOut(xdc, 0, 4 * 16, s, strlen(s));
		sprintf(s, "PointDistance[2]: %f", PointDistance[2]);
		TextOut(xdc, 0, 5 * 16, s, strlen(s));
		sprintf(s, "PointHeight[0]: %f", PointHeight[0]);
		TextOut(xdc, 0, 6 * 16, s, strlen(s));
		sprintf(s, "PointHeight[1]: %f", PointHeight[1]);
		TextOut(xdc, 0, 7 * 16, s, strlen(s));
		sprintf(s, "PointHeight[2]: %f", PointHeight[2]);
		TextOut(xdc, 0, 8 * 16, s, strlen(s));
		sprintf(s, "NumVerts: %d", NumVerts);
		TextOut(xdc, 0, 9 * 16, s, strlen(s));
		D3D->GetBackBufferPtr()->ReleaseDC(xdc);
		*/

		*y = MinHeight + DiffHeight / 2.0f;
    } // end if

    // Keep enumerating
    return FALSE;
} // end FindPointAboveTerrainCB

RandomizeTerrainCB(CD3DFileObject* DXFileObj,
				   D3DMATRIX*,
				   VOID*)
{
	static D3DVERTEX* Verts;
    static DWORD      NumVerts;

    if(SUCCEEDED(DXFileObj->GetMeshGeometry(&Verts, &NumVerts, 0, 0)))
    {
        for(DWORD i = 1; i < NumVerts; i++)
        {
			Verts[i].y += Verts[i].x * TERRAIN_Slope;
			Verts[i].y += sin(Verts[i].x * TERRAIN_BumpFactor) * TERRAIN_BumpHeight * 2.0 -
						  sin(Verts[i].z * TERRAIN_BumpFactor) * TERRAIN_BumpHeight;
			if(0.0 != TERRAIN_Randomness)
				Verts[i].y += (rand()%(INT)(TERRAIN_Randomness * 1e4f)) / 1e4f; 
        } // end for
    } // end if

	// Re-compute normals
	DXFileObj->ComputeNormals();

    // Keep enumerating
    return FALSE;
} // end RandomizeTerrainCB