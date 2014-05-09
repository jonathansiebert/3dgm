//////////////////////////////////////////////////////////////////////////
// Direct3D.h                                                           //
// Interface for the Direct3D class                                     //
//////////////////////////////////////////////////////////////////////////

#ifndef ___DIRECT3D_H___INCLUDED___
#define ___DIRECT3D_H___INCLUDED___

// Includes //////////////////////////////////////////////////////////////
#include <Windows.h>
#include <Math.h>
#include <DDraw.h>
#define D3D_OVERLOADS
#include <D3D.h>
#include "D3DUtil.h"
#include "D3DTextr.h"

// Direct3D class interface //////////////////////////////////////////////
class Direct3D {
	protected:
	// Master DirectDraw interface
	IDirectDraw7* DirectDraw;

	// The primary surface and it's backbuffer
	IDirectDrawSurface7* PrimarySurface,
					   * BackBuffer,
	// A ZBuffer for rendering order
					   * ZBuffer;

	// A clipper for the backbuffer
	IDirectDrawClipper* Clipper;

	// Master Direct3D interface
	IDirect3D7* D3D;

	// The Direct3D hardware interface
	IDirect3DDevice7* D3DDevice;

	// Lighting
	DWORD NumLights;

	// Fog
	D3DCOLOR FogColor;

	// Freeforming
	BOOL IsFreeForm;
	IDirectDrawSurface7* GDIScreenShot;

	public:
	// Constructor
	Direct3D() {
		// Clear all members
		ZeroMemory(this, sizeof(*this));
	} // end Direct3D::Direct3D

	// In place of constructor, mainly so that their can be a return value
	HRESULT Init(HWND,	// Window to initialize in
				 INT,	// ScreenWidth
				 INT,	// ScreenHeight
				 BOOL);	// Whether to initialize in freeform mode, FALSE = no, TRUE = yes

	// Does a lot more that just a standard begin scene, this restores the surfaces,
	// erases the backbuffer, points the camara etc.
	HRESULT BeginScene(D3DVECTOR,	// Viewer position
					   D3DVECTOR);	// Look to position

	// Ends the scene, flips the surfaces etc.
	HRESULT EndScene();

	// Enables D3D fog
	HRESULT SetFog(D3DVALUE,	// Fog start
				   D3DVALUE,	// Fog end
				   D3DVALUE,	// Red
				   D3DVALUE,	// Green
				   D3DVALUE);	// Blue

	// Sets the ambient light
	HRESULT SetAmbient(D3DVALUE,	// Red
					   D3DVALUE,	// Green
					   D3DVALUE);	// Blue

	// Set a light in the Direct3D pipeline
	// If Index == 0 then it will be assigned the index of a new light created
	// If Index != 0 then the light at index Index will be reset
	HRESULT SetLight(DWORD&,		// The index of the light to be set
					 D3DLIGHTTYPE,	// The type of light to create
					 D3DVALUE,		// Red
					 D3DVALUE,		// Green
					 D3DVALUE,		// Blue
					 D3DVECTOR,		// Position
					 D3DVECTOR,		// Direction
					 D3DVALUE,		// Range
					 D3DVALUE,		// Angle, if this is a spotlight, of the inner cone
					 D3DVALUE);		// Angle, if this is a spotlight, of the outer cone

	// Turns lights on and off
	HRESULT SetLightEnable(DWORD,	// Index of light to set (0 means all)
						   BOOL);	// New state, (TRUE = on, FALSE = off)

	// Checks whether a light is on or off
	HRESULT GetLightEnable(DWORD,	// Index of light to check
						   BOOL*);	// The state is returned in this, (TRUE = on, FALSE = off)

	// Accessor methods
	IDirectDraw7*			GetDirectDrawPtr()		{return(DirectDraw);}
	IDirectDrawSurface7*	GetPrimarySurfacePtr()	{return(PrimarySurface);}
	IDirectDrawSurface7*	GetBackBufferPtr()		{return(BackBuffer);}
	IDirectDrawSurface7*	GetZBufferPtr()			{return(ZBuffer);}
	IDirectDrawClipper*		GetClipperPtr()			{return(Clipper);}
	IDirect3D7*				GetD3DPtr()				{return(D3D);}
	IDirect3DDevice7*		GetD3DDevicePtr()		{return(D3DDevice);}

	// Uninitializer
	HRESULT Shutdown();

	// Destructor
	~Direct3D() {
		// Just call the cleanup function
		Shutdown();
	} // end ~Direct3D
}; // end Direct3D

// Callbacks ////////////////////////////////////////////////////////////
// For picking a zbufffer format
// A global indicating whether the proper buffer was even found
static BOOL ZBufFound = FALSE;
HRESULT PASCAL EnumZBufferFormatsCallback(DDPIXELFORMAT*,
										  VOID*);

#endif