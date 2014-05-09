//////////////////////////////////////////////////////////////////////////
// Direct3D.cpp                                                         //
// Implementation of the Direct3D class                                 //
//////////////////////////////////////////////////////////////////////////

// Includes //////////////////////////////////////////////////////////////
#include "Direct3D.h"

// Methods for class Direct3D ////////////////////////////////////////////
// In place of constructor, mainly so that there can be a return value
HRESULT Direct3D::Init(HWND Window,			// Window to initialize in
					   INT ScreenWidth,		// ScreenWidth
					   INT ScreenHeight,	// ScreenHeight
					   BOOL FreeForm)		// Whether to initialize in freeform mode
{
	// Set the freeform member to the value passed
	IsFreeForm = FreeForm;

	static DDSURFACEDESC2 SD;

	static HRESULT Return;

	// First, if this is a freeform initialization, we need to copy the screen to a
	// DC owned by this	module
	HDC Screen;
	HBITMAP	ScreenData;
	if(IsFreeForm)
	{
		// Force GDI to repaint the screen
		InvalidateRect(0, 0, FALSE);

		// wait for repaint to finish
		Sleep(500);

		HDC Desktop = GetDC(0);

		Screen = CreateCompatibleDC(Desktop);
		ScreenData = CreateCompatibleBitmap(Desktop,
											ScreenWidth,
											ScreenHeight);
		SelectObject(Screen, ScreenData);

		StretchBlt(Screen,
				   0,
				   0,
				   ScreenWidth,
				   ScreenHeight,
				   Desktop,
				   0,
				   0,
				   ScreenWidth,
				   ScreenHeight,
				   SRCCOPY);   

		ReleaseDC(0, Desktop);
	} // end if
	else 
		ShowCursor(0);

	// Create the master DirectDraw interface
	Return = DirectDrawCreateEx(0,
								(LPVOID*)&DirectDraw,
								IID_IDirectDraw7,
								0);
	if(FAILED(Return))
		return Return;

	// Set the cooperative level with other apps, allow ctrl-alt-delete and that's about it
	Return = DirectDraw->SetCooperativeLevel(Window,
											 DDSCL_ALLOWREBOOT |
											 DDSCL_FULLSCREEN  |
											 DDSCL_EXCLUSIVE);
	if(FAILED(Return))
		return Return;

	// Set the display mode
	Return = DirectDraw->SetDisplayMode(ScreenWidth,
									    ScreenHeight,
									    16, // Force 16 bit color
									    0,
									    0);
	if(FAILED(Return))
		return Return;

	// Create the backbuffer's clipper
	Return = DirectDraw->CreateClipper(0,
									   &Clipper,
									   0);
	if(FAILED(Return))
		return Return;

	// Set the clipper's clip list -- do it the easy way
	Return = Clipper->SetHWnd(0,
							  Window);
	if(FAILED(Return))
		return Return;

	// Create the primary surface
	ZeroMemory(&SD, sizeof(SD));
	SD.dwSize = sizeof(SD);
	SD.dwFlags = DDSD_CAPS |
				 DDSD_BACKBUFFERCOUNT;
	SD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
						DDSCAPS_FLIP           |
						DDSCAPS_3DDEVICE       |
						DDSCAPS_COMPLEX;
	SD.dwBackBufferCount = 1;
	Return = DirectDraw->CreateSurface(&SD,
									   &PrimarySurface,
									   0);
	if(FAILED(Return))
		return Return;

	// Get the attached surface (backbuffer)
	static DDSCAPS2 C;
	ZeroMemory(&C, sizeof(C));
	C.dwCaps = DDSCAPS_BACKBUFFER;
	Return = PrimarySurface->GetAttachedSurface(&C,
												&BackBuffer);
	if(FAILED(Return))
		return Return;

	// Attach the clipper to the backbuffer
	Return = BackBuffer->SetClipper(Clipper);
	if(FAILED(Return))
		return Return;

	// Get master 3D interface
	Return = DirectDraw->QueryInterface(IID_IDirect3D7, (LPVOID*)&D3D);
	if(FAILED(Return))
		return Return;
	
	// Create the zbuffer, not a fun thing to do
	// Stores the 
	INT ZBufFormat = 0;
	// Enumerate the Z-buffer formats, making sure they fit the device.
	// First try hardware with accelerated transform and lighting.
	// If you cannot get that, just keep trying with worse and worse devices :)
	static DDPIXELFORMAT ZBufferPF;
	ZeroMemory(&ZBufferPF, sizeof(ZBufferPF));
	ZBufferPF.dwSize = sizeof(ZBufferPF);
	ZBufferPF.dwFlags = DDPF_ZBUFFER ;
	D3D->EnumZBufferFormats(	IID_IDirect3DTnLHalDevice, 
								EnumZBufferFormatsCallback,
								(VOID*)&ZBufferPF	);
	if(ZBufFound)
		ZBufFormat = 1;
	else
	{
		D3D->EnumZBufferFormats(	IID_IDirect3DHALDevice,
									EnumZBufferFormatsCallback,
									(VOID*)&ZBufferPF	);
		if(ZBufFound)
			ZBufFormat=2;
		else
		{
			D3D->EnumZBufferFormats(	IID_IDirect3DMMXDevice,
										EnumZBufferFormatsCallback,
										(VOID*)&ZBufferPF	);
			if(ZBufFound)
				ZBufFormat=3;
			else
			{
				D3D->EnumZBufferFormats(	IID_IDirect3DRGBDevice,
											EnumZBufferFormatsCallback,
											(VOID*)&ZBufferPF	);
				if(ZBufFound) 
					ZBufFormat=4;
			} // end else
		} // end else
	} // end else

	// Create the zbuffer
	ZeroMemory(&SD, sizeof(SD));
    SD.dwSize = sizeof(SD);
    SD.dwFlags = DDSD_CAPS   |
				 DDSD_WIDTH  |
				 DDSD_HEIGHT |
				 DDSD_PIXELFORMAT;
	
	// Use counter to check if we should create z-buffer in video or system memory
	// Note flag that specifies that this is a z-buffer surface.
	if(ZBufFormat<3)
		SD.ddsCaps.dwCaps = DDSCAPS_ZBUFFER|DDSCAPS_VIDEOMEMORY;
	else
		SD.ddsCaps.dwCaps = DDSCAPS_ZBUFFER|DDSCAPS_SYSTEMMEMORY;
	
	// Set it the to size of screen
	SD.dwWidth = ScreenWidth;
	SD.dwHeight = ScreenHeight;
	SD.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	SD.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
	memcpy(&SD.ddpfPixelFormat, &ZBufferPF, sizeof(DDPIXELFORMAT));
    Return = DirectDraw->CreateSurface(&SD, &ZBuffer, 0);
	if(FAILED(Return))
		return Return;

	// Attach z-buffer to the backbuffer...
	Return = BackBuffer->AddAttachedSurface(ZBuffer);
	if(FAILED(Return))
		return Return;

	// Set up the best hardware interface, really dumb, brute-forcish way to do it
	if(FAILED(D3D->CreateDevice(IID_IDirect3DTnLHalDevice, BackBuffer, &D3DDevice)))
		if(FAILED(D3D->CreateDevice(IID_IDirect3DHALDevice, BackBuffer, &D3DDevice)))
			if(FAILED(D3D->CreateDevice(IID_IDirect3DMMXDevice, BackBuffer, &D3DDevice)))
				if(FAILED(Return = D3D->CreateDevice(IID_IDirect3DRGBDevice, BackBuffer, &D3DDevice)))
					return Return; // Report the error

	// Set up the viewport for the new screen size
	D3DVIEWPORT7 Viewport;
	Viewport.dwX = 0;
	Viewport.dwY = 0;
	Viewport.dwWidth = ScreenWidth;
	Viewport.dwHeight = ScreenHeight;
	Viewport.dvMinZ = 0.0f;
	Viewport.dvMaxZ = 1.0f;
	Return = D3DDevice->SetViewport(&Viewport);
	if(FAILED(Return))
		return Return;

	// Set up a nice projection matrix
	D3DMATRIX PM(3,	0,  0, 0,
				 0,	4,  0, 0,
				 0,	0,  1, 1,
				 0, 0, -5, 0);

	// Apply the projection matrix
	Return = D3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &PM);
	if(FAILED(Return))
		return Return;

	// Turn on some nice texture filtering
	Return = D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	if(FAILED(Return))
		return Return;

	// Enable z-buffering.
    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_USEW);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
	if(FAILED(Return))
		return Return;

	// Enable Direct3D lighting
	Return = D3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
	if(FAILED(Return))
		return Return;
	Return = D3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
	if(FAILED(Return))
		return Return;

	// Just to be safe, disable backface culling
	Return = D3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	if(FAILED(Return))
		return Return;

	// Turn on dithering
	Return = D3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
	if(FAILED(Return))
		return Return;

	// Do any freeform initialization
	if(IsFreeForm)
	{
		// Get a copy of the GDI surface
		ZeroMemory(&SD, sizeof(SD));
		SD.dwSize = sizeof(SD);
		SD.dwFlags = DDSD_WIDTH		|
					 DDSD_HEIGHT	|
					 DDSD_CAPS;
		SD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		SD.dwWidth = ScreenWidth;
		SD.dwHeight = ScreenHeight;
		Return = DirectDraw->CreateSurface(&SD, &GDIScreenShot, 0);
		if(FAILED(Return))
			return(Return);

		HDC xdc;
		GDIScreenShot->GetDC(&xdc);
		StretchBlt(xdc,
				   0,
				   0,
				   ScreenWidth,
				   ScreenHeight,
				   Screen,
				   0,
				   0,
				   ScreenWidth,
				   ScreenHeight,
				   SRCCOPY);
		GDIScreenShot->ReleaseDC(xdc);

		// Clean up the GDI memory
		DeleteDC(Screen);
		DeleteObject(ScreenData);
	} // end if

	// Return success
	return(D3D_OK);
} // end Direct3D::Init

// Does a lot more that just a standard begin scene, this restores the surfaces,
// erases the backbuffer, points the camara etc.
HRESULT Direct3D::BeginScene(D3DVECTOR At, // Viewer position
							 D3DVECTOR To) // Look to position
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	// Restore any lost surfaces
	if(DDERR_SURFACELOST == PrimarySurface->IsLost())
		PrimarySurface->Restore();
	if(DDERR_SURFACELOST == BackBuffer->IsLost())
		BackBuffer->Restore();
	if(DDERR_SURFACELOST == ZBuffer->IsLost())
		ZBuffer->Restore();
	D3DTextr_RestoreAllTextures(D3DDevice);

	static HRESULT Return;

	// Set up a view matrix
	static D3DMATRIX VM;
	D3DUtil_SetViewMatrix(VM, At, To, D3DVECTOR(0, 1, 0));
	// Apply the view matrix
	Return = D3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &VM);
	if(FAILED(Return))
		return Return;

	// Clear the viewport if freeform is off, or just clear the ZBuffer
	// and draw a copy of the GDI surface if it is on
	if(IsFreeForm)
	{
		// clear the z-buffer
		Return = D3DDevice->Clear(0,
							   	  0,
								  D3DCLEAR_ZBUFFER,
								  0,
								  1.0f,
								  0);
		if(FAILED(Return))
			return Return;

		// Copy the screenshot onto the backbuffer
		HDC hdc, xdc;
		BackBuffer->GetDC(&hdc);
		GDIScreenShot->GetDC(&xdc);

		StretchBlt(hdc,
				   0,
				   0,
				   GetSystemMetrics(SM_CXSCREEN),
				   GetSystemMetrics(SM_CYSCREEN),
				   xdc,
				   0,
				   0,
				   GetSystemMetrics(SM_CXSCREEN),
				   GetSystemMetrics(SM_CYSCREEN),
				   SRCCOPY);

		BackBuffer->ReleaseDC(hdc);
		GDIScreenShot->ReleaseDC(xdc);
	} // end if
	else
	{
		// Just clear the viewport
		Return = D3DDevice->Clear(0,
							   	  0,
								  D3DCLEAR_ZBUFFER |
								  D3DCLEAR_TARGET,
								  FogColor,
								  1.0f,
								  0);
		if(FAILED(Return))
			return Return;
	} // end else

	// Start the scene render
	Return = D3DDevice->BeginScene();
	if(FAILED(Return))
		return Return;

	// Return success
	return(D3D_OK);
} // end Direct3D::BeginScene

// Ends the scene, flips the surfaces etc.
HRESULT Direct3D::EndScene()
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	static HRESULT Return;

	// Render the scene
	Return = D3DDevice->EndScene();
	if(FAILED(Return))
		return Return;

	// Flip between the primary surface and backbuffer, bringing the rendering to the screen
	Return = PrimarySurface->Flip(0, DDFLIP_WAIT);
	if(FAILED(Return))
		return Return;

	// Return success
	return(D3D_OK);
} // end Direct3D::EndScene

// Enables D3D fog
HRESULT Direct3D::SetFog(D3DVALUE FogStart,	// Fog start
				  	     D3DVALUE FogEnd,	// Fog end
			   	  	     D3DVALUE Red,		// Red
			   	  	     D3DVALUE Green,	// Green
			   	  	     D3DVALUE Blue)		// Blue
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	static HRESULT Return;

	// Enable fog blending
    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
	if(FAILED(Return))
		return Return;
 
    // Set the fog color
	FogColor = D3DRGB(Red, Green, Blue);
    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, FogColor);
	if(FAILED(Return))
		return Return;
    
    // Set fog parameters
    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
	if(FAILED(Return))
		return Return;

    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD*)(&FogStart));
	if(FAILED(Return))
		return Return;

    Return = D3DDevice->SetRenderState(D3DRENDERSTATE_FOGEND,   *(DWORD*)(&FogEnd));
	if(FAILED(Return))
		return Return;	

	// Shouldn't draw past the fog end
	static D3DMATRIX PM;
	PM._11 = 3;	PM._12 = 0;	PM._13 = 0;							PM._14 = 0;
    PM._21 = 0;	PM._22 = 4;	PM._23 = 0;							PM._24 = 0;
    PM._31 = 0;	PM._32 = 0;	PM._33 = FogEnd / (FogEnd - 5.0f);	PM._34 = 1;
    PM._41 = 0;	PM._42 = 0;	PM._43 = -5.0f * PM._33;			PM._44 = 0;

	// Apply the projection matrix
	Return = D3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &PM);
	if(FAILED(Return))
		return Return;	   

	// Return success
	return(D3D_OK);
} // end Direct3D::SetFog

// Sets the ambient light
HRESULT Direct3D::SetAmbient(D3DVALUE Red,		// Red
							 D3DVALUE Green,	// Green
							 D3DVALUE Blue)		// Blue
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	// Set up the color
	static D3DCOLOR Color;
	Color = D3DRGB(Red, Green, Blue);

	// Return success or failure
	return(D3DDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, Color));
} // end Direct3D::SetAmbient

// Set a light in the Direct3D pipeline
// If Index == 0 then it will be assigned the index of a new light created
// If Index != 0 then the light at index Index will be reset
HRESULT Direct3D::SetLight(DWORD& Index,		// The index of the light to be set
						   D3DLIGHTTYPE Type,	// The type of light to create
				 		   D3DVALUE Red,		// Red
				 		   D3DVALUE Green,		// Green
				 		   D3DVALUE Blue,		// Blue
				 		   D3DVECTOR Pos,		// Position
				 		   D3DVECTOR Dir,		// Direction
				 		   D3DVALUE Range,		// Range
				 		   D3DVALUE Theta,		// Angle, if this is a spotlight, of the inner cone
				 		   D3DVALUE Phi)		// Angle, if this is a spotlight, of the outer cone
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	// If we are creating a new light, we have to increment NumLights
	if(0 == Index)
	{
		NumLights++;

		// Return the light index via reference
		Index = NumLights;
	} // end if

	// A Direct3D light structure
	static D3DLIGHT7 Light;
	// Clear it out
	ZeroMemory(&Light, sizeof(Light));
	// Now set the fields as they were passed
	Light.dltType			= Type;
	Light.dcvDiffuse.r		= Red;
	Light.dcvDiffuse.g		= Green;
	Light.dcvDiffuse.b		= Blue;
	Light.dvPosition		= Pos;
	Light.dvDirection		= Dir;
	Light.dvFalloff			= 1.0f;
	Light.dvTheta			= Theta;
	Light.dvPhi				= Phi;
	Light.dvAttenuation0	= 1.0f;

	// Put range to max if zero is passed
	if(0 == Range)
		Light.dvRange		= D3DLIGHT_RANGE_MAX;
	else
		Light.dvRange		= Range;

	// Finally, set the light in the render pipeline
	static HRESULT Return;
	Return = D3DDevice->SetLight(Index, &Light);
	if(FAILED(Return))
		return Return; // Report the error
	
	// Return success or failure at turning the light on
	return(D3DDevice->LightEnable(Index, TRUE));
} // end Direct3D::SetLight

// Turns lights on and off
HRESULT Direct3D::SetLightEnable(DWORD Index,	// Index of light to set (0 means all)
								 BOOL State)	// New state, (TRUE = on, FALSE = off)
{
	// Make sure the device pointer is valid
	if(0 == D3DDevice)
		return(E_FAIL);

	// See if we are just setting one light
	if(Index)
	{
		// Return success or failure
		return(D3DDevice->LightEnable(Index, State));
	} // end if
	else // Nope, set the state of them all!
	{
		static HRESULT Return;

		// Go through all of the lights
		// Remember that index zero is reserved
		for(int i = 1; i <= NumLights; i++)
			if(FAILED(Return = D3DDevice->LightEnable(i + 1, State)))
				return(Return); // Return the error

		// Return success
		return(D3D_OK);
	} // end else

	// It could not have gotten here, so return failure
	return(E_FAIL);
} // end Direct3D::SetLightEnable

// Checks whether a light is on or off
HRESULT Direct3D::GetLightEnable(DWORD Index,	// Index of light to check
								 BOOL* State)	// The state is returned in this, (TRUE = on, FALSE = off)
{
	// Return success or failure
	return(D3DDevice->GetLightEnable(Index, State));
} // end Direct3D::SetLightEnable

// Uninitializer
HRESULT Direct3D::Shutdown()
{
	#define SafeRelease(I) {if(I) {I->Release(); I = 0;}}

	// Dump all of the COM interfaces
	SafeRelease(D3DDevice);
	SafeRelease(D3D);
	SafeRelease(PrimarySurface);
	SafeRelease(DirectDraw);

	// Clear out all of the members
	ZeroMemory(this, sizeof(*this));

	// Return success
	return(D3D_OK);
} // end Direct3D::End

// Callbacks //////////////////////////////////////////////////////////
HRESULT PASCAL EnumZBufferFormatsCallback(DDPIXELFORMAT* PixelFormat,
										  VOID* DesiredFormat)
{
	// What good are null pointers?
	if(0 == PixelFormat || 0 == DesiredFormat)
        return D3DENUMRET_CANCEL;

    // If the current pixel format's match the desired ones (DDPF_ZBUFFER and
    // possibly DDPF_STENCILBUFFER), lets copy it and return. This function is
    // not choosy...it accepts the first valid format that comes along.
    if(PixelFormat->dwFlags == ((DDPIXELFORMAT*)DesiredFormat)->dwFlags)
    {
        memcpy(DesiredFormat, PixelFormat, sizeof(DDPIXELFORMAT));
		
		// Set flag to TRUE, since we got a valid z-buffer format.
		ZBufFound=TRUE;

		// We are done
        return D3DENUMRET_CANCEL;
    } // end if

	// OK, we didn't get anything, but that just makes us need more devices :)
    return D3DENUMRET_OK;
} // end EnumZBufferFormatsCallback