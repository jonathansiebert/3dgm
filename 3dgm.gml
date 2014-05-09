
#define D3D_DLLInitialize
//////////////////////////////////////////////////////////////
// Name: D3D_DLLInitialize
// Desc:
//   Stores ids for all of "D3D_" functions in globals
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = 
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = none
//////////////////////////////////////////////////////////////
{ // begin D3D_DLLInitialize
  // Render flags
  global.R_DisableZBuf   = 1;
  global.R_UseColorAlpha = 2;
  global.R_BlackRender  = 4;
  global.R_RenderFirst  = 32;
  global.R_Shadow        = 1 | 4;
  global.R_Alpha         = 1 | 2;
  
  DLL = "3DGM.dll";
  
  // Get IDs for all of the functions
  // initialization/Shutdown
  global.g_D3D_Initialize = external_define(DLL, "?D3D_Initialize@@YANNN@Z", dll_cdecl, ty_real, 2, ty_real, ty_real);
  global.g_D3D_Shutdown = external_define(DLL, "?D3D_Shutdown@@YANXZ", dll_cdecl, ty_real, 0);
  
  // Scene control
  global.g_D3D_BeginScene = external_define(DLL, "?D3D_BeginScene@@YANNNNNNN@Z", dll_cdecl, ty_real, 6,  ty_real, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_SetCamera = external_define(DLL, "?D3D_SetCamera@@YANNNNNNN@Z", dll_cdecl, ty_real, 6, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_EndScene = external_define(DLL, "?D3D_EndScene@@YANXZ", dll_cdecl, ty_real, 0);
  
  // Lighting
  global.g_D3D_SetAmbient = external_define(DLL, "?D3D_SetAmbient@@YANNNN@Z", dll_cdecl, ty_real, 3,  ty_real, ty_real, ty_real);
  global.g_D3D_SetLight = external_define(DLL, "?D3D_SetLight@@YANNNNNNNNNNN@Z", dll_cdecl, ty_real, 10, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_SetLightCones = external_define(DLL, "?D3D_SetLightCones@@YANNNN@Z", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_real);
  global.g_D3D_SetLightEnable = external_define(DLL, "?D3D_SetLightEnable@@YANNN@Z", dll_cdecl, ty_real, 2, ty_real, ty_real);
  global.g_D3D_GetLightEnable = external_define(DLL, "?D3D_GetLightEnable@@YANN@Z", dll_cdecl, ty_real, 1, ty_real);
  
  // DXFiles
  global.g_D3D_LoadDXFile = external_define(DLL, "?D3D_LoadDXFile@@YANPAD@Z", dll_cdecl, ty_real, 1,  ty_string);
  global.g_D3D_RenderDXFile = external_define(DLL, "?D3D_RenderDXFile@@YANNNNNNNNNNNN@Z", dll_cdecl, ty_real, 11, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_FindPointAboveTerrain = external_define(DLL, "?D3D_FindPointAboveTerrain@@YANNNN@Z", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_real);
  global.g_D3D_RandomizeTerrain = external_define(DLL, "?D3D_RandomizeTerrain@@YANNNNNN@Z", dll_cdecl, ty_real, 5, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_UnloadDXFile = external_define(DLL, "?D3D_UnloadDXFile@@YANN@Z", dll_cdecl, ty_real, 1,  ty_real);
  global.g_D3D_UnloadAllDXFiles = external_define(DLL, "?D3D_UnloadAllDXFiles@@YANXZ", dll_cdecl, ty_real, 0);
  
  // Billboards
  global.g_D3D_LoadBillboard = external_define(DLL, "?D3D_LoadBillboard@@YANPAD@Z", dll_cdecl, ty_real, 1, ty_string);
  global.g_D3D_BeginBillboard = external_define(DLL, "?D3D_BeginBillboard@@YANPAD@Z", dll_cdecl, ty_real, 1, ty_string);
  global.g_D3D_DrawBillboard = external_define(DLL, "?D3D_DrawBillboard@@YANNNNNNNNNN@Z", dll_cdecl, ty_real, 9, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real);
  global.g_D3D_UnloadBillboard = external_define(DLL, "?D3D_UnloadBillboard@@YANPAD@Z", dll_cdecl, ty_real, 1, ty_string);
  
  // 2D Text
  global.g_D3D_SetFont = external_define(DLL, "?D3D_SetFont@@YANPADNNN@Z", dll_cdecl, ty_real, 4,  ty_string, ty_real, ty_real, ty_real);
  global.g_D3D_DrawText = external_define(DLL, "?D3D_DrawText@@YANNNNPAD@Z", dll_cdecl, ty_real, 4,  ty_real, ty_real, ty_real, ty_string);
  global.g_D3D_SetTextColor = external_define(DLL, "?D3D_SetTextColor@@YANNNN@Z", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_real);
  
  // Render states
  global.g_D3D_SetCullMode = external_define(DLL, "?D3D_SetCullMode@@YANN@Z", dll_cdecl, ty_real, 1,  ty_real);
  global.g_D3D_SetFog = external_define(DLL, "?D3D_SetFog@@YANNNNNN@Z", dll_cdecl, ty_real, 5,  ty_real, ty_real, ty_real, ty_real, ty_real);
} // end D3D_DLLInitialize


#define D3D_Initialize
//////////////////////////////////////////////////////////////
// Name: D3D_Initialize
// Desc:
//   Initializes Direct3D in a child window under GM's window.
//   Does not change screen resolution, so you set it with GM.
//   
//////////////////////////////////////////////////////////////
// argument0  = Whether to initialize normal or freeform mode, 0 = no, 1 = yes
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_Initialize
  return external_call(global.g_D3D_Initialize,
                       window_handle(),
                       argument0);
} // end D3D_Initialize


#define D3D_Shutdown
//////////////////////////////////////////////////////////////
// Name: D3D_Shutdown
// Desc:
//   Unloads Direct3D.
//   Unloads all DirectX files.
//   
//////////////////////////////////////////////////////////////
// argument0  = 
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_Shutdown
  // Unload the DirectX files.
  external_call(global.g_D3D_UnloadAllDXFiles);
    
  // Unload Direct3D
  return external_call(global.g_D3D_Shutdown);
} // end D3D_Shutdown


#define D3D_BeginScene
//////////////////////////////////////////////////////////////
// Name: D3D_BeginScene
// Desc:
//   Begins rendering.
//   All rendering each frame must take place between this and
//   D3D_EndScene.
//////////////////////////////////////////////////////////////
// argument0  = Viewpoint X
// argument1  = Viewpoint Y
// argument2  = Viewpoint Z
// argument3  = X of point to look at
// argument4  = Y of point to look at
// argument5  = Z of point to look at
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_BeginScene
  return external_call(global.g_D3D_BeginScene,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4,
                       argument5);
} // end D3D_BeginScene


#define D3D_EndScene
//////////////////////////////////////////////////////////////
// Name: D3D_EndScene
// Desc:
//   Ends rendering.
//   Draws everything rendered since the last call to
//   D3D_BeginScene.
//////////////////////////////////////////////////////////////
// argument0  = 
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_EndScene
  return external_call(global.g_D3D_EndScene);
} // end D3D_EndScene


#define D3D_SetAmbient
//////////////////////////////////////////////////////////////
// Name: D3D_SetAmbient
// Desc:
//   Sets the ambient light level.
//   Range for colors is 0 to 1.
//   
//////////////////////////////////////////////////////////////
// argument0  = Red
// argument1  = Green
// argument2  = Blue
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetAmbient
  return external_call(global.g_D3D_SetAmbient,
                       argument0,
                       argument1,
                       argument2);
} // end D3D_SetAmbient


#define D3D_SetLight
//////////////////////////////////////////////////////////////
// Name: D3D_SetLight
// Desc:
//   Creates a new light or sets an existing one.
//   Valid light types are as follows:
//   0 - A directional light, it has direction but no position. An example of this would be the sun.
//   1 - A point light, it has a position but no particular direction. An example of this would be a bulb.
//   2 - A spot light, it has position, direction, and two light cones, a inner one at full intensity,
//       and a outer one with only partial intensity.
//   The fastest lights are directional, point lights are slower, and spot lights are slowest.
//   If this is a spot light, you will probably want to change the angles of it's cones.
//   To do so, call the D3D_SetLightCones function. I would have had the cone setting in
//   here, but GM DLL calls have a maximum of twelve arguments. :(
//////////////////////////////////////////////////////////////
// argument0  = Index of light to set (0 means to create a new one and return it's index)
// argument1  = Type of light to create, light types are described above
// argument2  = Color, like make_color(60, 160, 60)
// argument3  = X (not used in directional lights)
// argument4  = Y (not used in directional lights)
// argument5  = Z (not used in directional lights)
// argument6  = X rotation (not used in point lights)
// argument7  = Y rotation (not used in point lights)
// argument8  = Z rotation (not used in point lights)
// argument9  = Range (0 means max) (not used in directional lights)
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (0 if not successful) (the index of the light otherwise)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetLight
  return external_call(global.g_D3D_SetLight,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4,
                       argument5,
                       argument6,
                       argument7,
                       argument8,
                       argument9);
} // end D3D_SetLight


#define D3D_SetLightEnable
//////////////////////////////////////////////////////////////
// Name: D3D_SetLightEnable
// Desc:
//   Turns lights on or off.
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = Index of light to set (0 means all)
// argument1  = State to put light at (0 means off, 1 means on)
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetLightEnable
  return external_call(global.g_D3D_SetLightEnable,
                       argument0,
                       argument1);
} // end D3D_SetLightEnable


#define D3D_GetLightEnable
//////////////////////////////////////////////////////////////
// Name: D3D_GetLightEnable
// Desc:
//   Check whether a light is on or off.
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = The index of the light to set
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (-1 if not successful) (0 is light is off) (1 if light is on)
//////////////////////////////////////////////////////////////
{ // begin D3D_GetLightEnable
  return external_call(global.g_D3D_GetLightEnable);
} // end D3D_GetLightEnable


#define D3D_LoadDXFile
//////////////////////////////////////////////////////////////
// Name: D3D_LoadDXFile
// Desc:
//   Loads a DirectX (.x) file and returns a index
//   to pass to D3D_RenderDXFile.
//   
//////////////////////////////////////////////////////////////
// argument0  = The name of the file containing the mesh to load
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (0 if not successful) (otherwise the index of the new DirectX file)
//////////////////////////////////////////////////////////////
{ // begin D3D_LoadDXFile
  return external_call(global.g_D3D_LoadDXFile,
                       argument0);
} // end D3D_LoadDXFile


#define D3D_RenderDXFile
//////////////////////////////////////////////////////////////
// Name: D3D_RenderDXFile
// Desc:
//   Renders a previously loaded DirectX file.
//   Must be called between D3D_BeginScene and D3D_EndScene.
//   Scaling is multiplied against size, so 0 is infinitly small,
//   1 is normal, 2 is twice as long, etc.
//////////////////////////////////////////////////////////////
// argument0  = Index of the DirectX file to draw
// argument1  = X
// argument2  = Y
// argument3  = Z
// argument4  = X rotation
// argument5  = Y rotation
// argument6  = Z rotation
// argument7  = X scale
// argument8  = Y scale
// argument9  = Z scale
// argument10 = Render flags, as definded in D3D_DLLInitialize
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_RenderDXFile
  return external_call(global.g_D3D_RenderDXFile,
                       argument0,
                       argument1,  
                       argument2,  
                       argument3,  
                       argument4,  
                       argument5,  
                       argument6,  
                       argument7,  
                       argument8,  
                       argument9,
                       argument10);
} // end D3D_RenderDXFile


#define D3D_UnloadDXFile
//////////////////////////////////////////////////////////////
// Name: D3D_UnloadDXFile
// Desc:
//   Unloads the DirectX file at the specified index.
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = Index of DirectX file to unload
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_UnloadDXFile
  return external_call(global.g_D3D_UnloadDXFile,
                       argument0);
} // end D3D_UnloadDXFile


#define D3D_UnloadAllDXFiles
//////////////////////////////////////////////////////////////
// Name: D3D_UnloadAllDXFiles
// Desc:
//   Unloads all DirectXFiles.
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = 
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 =  
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_UnloadAllDXFiles
  return external_call(global.g_D3D_UnloadAllDXFiles);
  } // end D3D_UnloadAllDXFiles


#define D3D_SetFont
//////////////////////////////////////////////////////////////
// Name: D3D_SetFont
// Desc:
//   Sets the font used to render text.
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = Name of font, like "Times New Roman"
// argument1  = Average width of characters
// argument2  = Average height of characters
// argument3  = Angle at which to draw text
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetFont
  return external_call(global.g_D3D_SetFont,
                       argument0,
                       argument1,
                       argument2,
                       argument3);
} // end D3D_SetFont


#define D3D_DrawText
//////////////////////////////////////////////////////////////
// Name: D3D_DrawText
// Desc:
//   Draws the specified string.
//   Must be called AFTER rendering the scene.
//   
//////////////////////////////////////////////////////////////
// argument0  = X
// argument1  = Y
// argument2  = Centering boolean, 1 if the text should be centered on (X, Y), 0 if not
// argument3  = String to draw
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_DrawText
  return external_call(global.g_D3D_DrawText,
                       argument0,
                       argument1,
                       argument2,
                       argument3);
} // end D3D_DrawText


#define D3D_SetTextColor
//////////////////////////////////////////////////////////////
// Name: D3D_SetTextColor
// Desc:
//   Set the color that text will be rendered with.
//   Range of colors is 0 to 255
//   
//////////////////////////////////////////////////////////////
// argument0  = Red
// argument1  = Green
// argument2  = Blue
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetTextColor
  return external_call(global.g_D3D_SetTextColor,
                       argument0,
                       argument1,
                       argument2);
} // end D3D_SetTextColor


#define D3D_SetLightCones
//////////////////////////////////////////////////////////////
// Name: D3D_SetLightCones
// Desc:
//   Sets the angles of the inner and outer cones of a spotlight.
//   I would have had the cone setting in the D3D_SetLight function,
//   but GM DLL calls have a maximum of twelve arguments. :(
//////////////////////////////////////////////////////////////
// argument0  = Index of light to set
// argument1  = Angle of inner cone
// argument2  = Angle of outer cone
// argument3  =
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 =
// argument11 =
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetLightCones
  return external_call(global.g_D3D_SetLightCones,
                       argument1,
                       argument2,
                       argument3);
} // end D3D_SetLightCones


#define D3D_SetCullMode
//////////////////////////////////////////////////////////////
// Name: D3D_SetCullMode
// Desc:
//   Turns backface culling on and off.
//   Backface culling is a methods of preventing the drawing
//   of triangles that do not face the camara, unfortunately,
//   this often makes renders not turn out properly, so it defaults
//   to off. (Turn it on if you can)
//////////////////////////////////////////////////////////////
// argument0  = new cull mode (0 means no culling) (nonzero means counterclockwise culling)
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (nonzero if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetCullMode
  return external_call(global.g_D3D_SetCullMode,
                       argument0);
} // end D3D_SetCullMode


#define D3D_SetFog
//////////////////////////////////////////////////////////////
// Name: D3D_SetFog
// Desc:
//   Sets the fog effect.
//////////////////////////////////////////////////////////////
// argument0  = Distance from view that fog starts
// argument1  = Distance from view the everything beyond is obscured
// argument2  = Red (0 to 1)
// argument3  = Green (0 to 1)
// argument4  = Blue (0 to 1)
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetFog
  return external_call(global.g_D3D_SetFog,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4);
} // end D3D_SetFog


#define D3D_SetCamera
//////////////////////////////////////////////////////////////
// Name: D3D_SetCamera
// Desc:
//   Set the camera position and rotation.
//   It would probably be wise to not call this more than once
//   per frame.
//////////////////////////////////////////////////////////////
// argument0  = Viewpoint X
// argument1  = Viewpoint Y
// argument2  = Viewpoint Z
// argument3  = X rotation
// argument4  = Y rotation
// argument5  = Z rotation
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_SetCamera
  return external_call(global.g_D3D_SetCamera,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4,
                       argument5);
} // end D3D_SetCamera


#define D3D_FindPointAboveTerrain
//////////////////////////////////////////////////////////////
// Name: D3D_FindPointAboveTerrain
// Desc:
//   Finds the Y coordinate of the point that touches the terrain
//   at (argument1, argument2). Terrain must not 
//   have vertical or overhanging triangles.
//   NOTE: Currently not implemented properly, only works with
//         high resolution terrain because it return the average
//         height of the three nearest points.
//////////////////////////////////////////////////////////////
// argument0  = Index of DirectX file to find point above
// argument1  = X coordinate, relative to the terrain, of the point to find
// argument2  = Z coordinate, relative to the terrain, of the point to find
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_FindPointAboveTerrain
  return external_call(global.g_D3D_FindPointAboveTerrain,
                       argument0,
                       argument1,
                       argument2);
} // end D3D_FindPointAboveTerrain


#define D3D_RandomizeTerrain
//////////////////////////////////////////////////////////////
// Name: D3D_RandomizeTerrain
// Desc:
//   Randomizes the y coordanite of the a DXFile' points
//////////////////////////////////////////////////////////////
// argument0  = Index of DirectX file to randomize the y coordanites of
// argument1  = Slope, 0 would be horizontal, 0.5 would be 22.5º, 1 would be 45º, after that
//           the effect decreases by half for each doubling, like this:
//           2 would be 77.5º, 4 would be 88.75º, and so on. Negetives slope
//           in the opposite direction. Sloping is always on the X axis. 
// argument2  = Bump factor, or wavyness. 
// argument3  = Bump amount, the maximum height of argument2's effect.
// argument4  = Randomness, or the maximun amount of height that can be randomly applied to a point.
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_RandomizeTerrain
  return external_call(global.g_D3D_RandomizeTerrain,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4);
} // end D3D_RandomizeTerrain


#define D3D_LoadBillboard
//////////////////////////////////////////////////////////////
// Name: D3D_LoadBillboard
// Desc:
//   Loads a picture to draw onto billboards
//   BLACK is transparent
//   YOUR PICTURES MUST BE 8, 4 or possibly 16 BIT .BMP FILES IF YOU
//   WANT BlACK TO BE TRANSPARENT!!! If transparency is not
//   and issue, 24 bit bitmaps, and 24 or 32 bit targas are fine.
//   You may use any bitmap or targa for alpha renders.
//////////////////////////////////////////////////////////////
// argument0  = FileName of the billboard picture to load
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_LoadBillboard
  return external_call(global.g_D3D_LoadBillboard,
                       argument0);
} // end D3D_LoadBillboard


#define D3D_BeginBillboard
//////////////////////////////////////////////////////////////
// Name: D3D_BeginBillboard
// Desc:
//   Sets the texture drawn onto billboards
/////////////////////////////////
// argument0  = FileName of the pre-loaded billboad picture to use
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_BeginBillboard
  return external_call(global.g_D3D_BeginBillboard,
                       argument0);
} // end D3D_BeginBillboard


#define D3D_DrawBillboard
//////////////////////////////////////////////////////////////
// Name: D3D_DrawBillboard
// Desc:
//   Draw a billboard with the currently set texture.
//   
//////////////////////////////////////////////////////////////
// argument0  = X
// argument1  = Y
// argument2  = Z
// argument3  = X rotation
// argument4  = Y rotation
// argument5  = Z rotation
// argument6  = X size
// argument7  = Y size
// argument8  = Render flags, as defined in D3D_DLLInitialize
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_DrawBillboard
  return external_call(global.g_D3D_DrawBillboard,
                       argument0,
                       argument1,
                       argument2,
                       argument3,
                       argument4,
                       argument5,
                       argument6,
                       argument7,
                       argument8);
} // end D3D_DrawBillboard


#define D3D_UnloadBillboard
//////////////////////////////////////////////////////////////
// Name: D3D_UnloadBillboard
// Desc:
//   Unloads a billboard texture
//   
//   
//////////////////////////////////////////////////////////////
// argument0  = FileName of the billboard picture to unload
// argument1  = 
// argument2  = 
// argument3  = 
// argument4  = 
// argument5  = 
// argument6  = 
// argument7  = 
// argument8  = 
// argument9  = 
// argument10 = 
// argument11 = 
//////////////////////////////////////////////////////////////
// return = (1 if successful) (0 if not)
//////////////////////////////////////////////////////////////
{ // begin D3D_UnloadBillboard
  return external_call(global.g_D3D_UnloadBillboard,
                       argument0);
} // end D3D_UnloadBillboard

