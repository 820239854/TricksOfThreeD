// DEMOII8_2b.CPP - texture color modulation with base texture demo 
// to compile make sure to include DDRAW.LIB, DSOUND.LIB,
// DINPUT.LIB, WINMM.LIB, and of course 
// T3DLIB1.CPP,T3DLIB2.CPP,T3DLIB3.CPP..T3DLIB6.CPP
// and only run app if desktop is in 16-bit color mode

// INCLUDES ///////////////////////////////////////////////

#define INITGUID

#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h>  // directX includes
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <dinput.h>
#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"
#include "T3DLIB4.h"
#include "T3DLIB5.h"
#include "T3DLIB6.h"


// defines for windows 
#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name

// setup a 640x480 16-bit windowed mode example
#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      640   // size of window
#define WINDOW_HEIGHT     480

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
								// note: if windowed and not
								// fullscreen then bitdepth must
								// be same as system bitdepth
								// also if 8-bit the a pallete
								// is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed

#define TEXTSIZE           128 // size of texture mxm
#define NUM_TEXT           12  // number of textures

#define NUM_LMAP           4   // number of light map textures

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void* parms = NULL);
int Game_Shutdown(void* parms = NULL);
int Game_Main(void* parms = NULL);

// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[80];                          // used to print text

BITMAP_IMAGE textures[NUM_TEXT],   // holds our texture library 
lightmaps[NUM_LMAP],  // holds our light map textures
temp_text;            // temporary working texture

// FUNCTIONS //////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   // handle to a device context

	// what is the message 
	switch (msg)
	{
	case WM_CREATE:
	{
		// do initialization stuff here
		return(0);
	} break;

	case WM_PAINT:
	{
		// start painting
		hdc = BeginPaint(hwnd, &ps);

		// end painting
		EndPaint(hwnd, &ps);
		return(0);
	} break;

	case WM_DESTROY:
	{
		// kill the application			
		PostQuitMessage(0);
		return(0);
	} break;

	default:break;

	} // end switch

// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	// this is the winmain function

	WNDCLASSEX winclass; // this will hold the class we create
	HWND	   hwnd;	 // generic window handle
	MSG		   msg;		 // generic message
	HDC        hdc;      // graphics device context

	// first fill in the window class stucture
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// save hinstance in global
	main_instance = hinstance;

	// register the window class
	if (!RegisterClassEx(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindowEx(NULL,                  // extended style
		WINDOW_CLASS_NAME,     // class
		WINDOW_TITLE, // title
		(WINDOWED_APP ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
		0, 0,	  // initial x,y
		WINDOW_WIDTH, WINDOW_HEIGHT,  // initial width, height
		NULL,	  // handle to parent 
		NULL,	  // handle to menu
		hinstance,// instance of this application
		NULL)))	// extra creation parms
		return(0);

	// save main window handle
	main_window_handle = hwnd;

	if (WINDOWED_APP)
	{
		// now resize the window, so the client area is the actual size requested
		// since there may be borders and controls if this is going to be a windowed app
		// if the app is not windowed then it won't matter
		RECT window_rect = { 0,0,WINDOW_WIDTH - 1,WINDOW_HEIGHT - 1 };

		// make the call to adjust window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));

		// save the global client offsets, they are needed in DDraw_Flip()
		window_client_x0 = -window_rect.left;
		window_client_y0 = -window_rect.top;

		// now resize the window with a call to MoveWindow()
		MoveWindow(main_window_handle,
			0, // x position
			0, // y position
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		// show the window, so there's no garbage on first render
		ShowWindow(main_window_handle, SW_SHOW);
	} // end if windowed


	// perform all game console specific initialization
	Game_Init();

	// enter main event loop
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

	// main game processing goes here
		Game_Main();

	} // end while

// shutdown game and release all resources
	Game_Shutdown();

	// return to Windows like this
	return(msg.wParam);

} // end WinMain

// T3D GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void* parms)
{
	// this function is where you do all the initialization 
	// for your game

	int index; // looping variable

	// initialize directdraw, very important that in the call
	// to setcooperativelevel that the flag DDSCL_MULTITHREADED is used
	// which increases the response of directX graphics to
	// take the global critical section more frequently
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// load in the textures
	Load_Bitmap_File(&bitmap16bit, "OMPTXT128_24.BMP");

	// now extract each 128x128x16 texture from the image
	for (int itext = 0; itext < NUM_TEXT; itext++)
	{
		// create the bitmap
		Create_Bitmap(&textures[itext], (WINDOW_WIDTH / 2) - 4 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2), TEXTSIZE, TEXTSIZE, 16);
		Load_Image_Bitmap16(&textures[itext], &bitmap16bit, itext % 4, itext / 4, BITMAP_EXTRACT_MODE_CELL);
	} // end for

// create temporary working texture (load with first texture to set loaded flags)
	Create_Bitmap(&temp_text, (WINDOW_WIDTH / 2) - (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) + (TEXTSIZE / 2), TEXTSIZE, TEXTSIZE, 16);
	Load_Image_Bitmap16(&temp_text, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_CELL);

	// done, so unload the bitmap
	Unload_Bitmap_File(&bitmap16bit);

	// now load the lightmaps

	// load in the textures
	Load_Bitmap_File(&bitmap16bit, "LIGHTMAPS128_24.BMP");

	// now extract each 128x128x16 texture from the image
	int itext;
	for (itext = 0; itext < NUM_LMAP; itext++)
	{
		// create the bitmap
		Create_Bitmap(&lightmaps[itext], (WINDOW_WIDTH / 2) + 2 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2), TEXTSIZE, TEXTSIZE, 16);
		Load_Image_Bitmap16(&lightmaps[itext], &bitmap16bit, itext % 4, itext / 4, BITMAP_EXTRACT_MODE_CELL);
	} // end for

// done, so unload the bitmap
	Unload_Bitmap_File(&bitmap16bit);


	// initialize directinput
	DInput_Init();

	// acquire the keyboard only
	DInput_Init_Keyboard();

	// hide the mouse
	if (!WINDOWED_APP)
		ShowCursor(FALSE);

	// seed random number generate
	srand(Start_Clock());

	// return success
	return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void* parms)
{
	// this function is where you shutdown your game and
	// release all resources that you allocated

	// shut everything down

	// shutdown directdraw last
	DDraw_Shutdown();

	// now directsound
	DSound_Stop_All_Sounds();
	DSound_Shutdown();

	// shut down directinput
	DInput_Shutdown();

	// return success
	return(1);
} // end Game_Shutdown

//////////////////////////////////////////////////////////

int Game_Main(void* parms)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	int          index;               // looping var
	static int   curr_texture = 7;   // currently active texture
	static int   curr_lightmap = 1;   // current light map
	static float scalef = .25; // texture ambient scale factor

	// start the timing clock
	Start_Clock();

	// clear the drawing surface
	DDraw_Fill_Surface(lpddsback, 0);

	// lock back buffer and copy background into it
	DDraw_Lock_Back_Surface();

	///////////////////////////////////////////
	// our little image processing algorithm :)

	// Pixel_dest[x,y]rgb = pixel_source[x,y]rgb * ambient + 
	//                      pixel_source[x,y]rgb * light_map[x,y]rgb

	USHORT* sbuffer = (USHORT*)textures[curr_texture].buffer;
	USHORT* lbuffer = (USHORT*)lightmaps[curr_lightmap].buffer;
	USHORT* tbuffer = (USHORT*)temp_text.buffer;

	// perform RGB transformation on bitmap
	for (int iy = 0; iy < temp_text.height; iy++)
		for (int ix = 0; ix < temp_text.width; ix++)
		{
			int rs, gs, bs;   // used to extract the source rgb values
			int rl, gl, bl; // light map rgb values
			int rf, gf, bf;   // the final rgb terms

			// extract pixel from source bitmap
			USHORT spixel = sbuffer[iy * temp_text.width + ix];

			// extract RGB values
			_RGB565FROM16BIT(spixel, &rs, &gs, &bs);

			// extract pixel from lightmap bitmap
			USHORT lpixel = lbuffer[iy * temp_text.width + ix];

			// extract RGB values
			_RGB565FROM16BIT(lpixel, &rl, &gl, &bl);

			// simple formula base + scale * lightmap
			rf = (scalef * (float)rl) + ((float)rs);
			gf = (scalef * (float)gl) + ((float)gs);
			bf = (scalef * (float)bl) + ((float)bs);

			// test for overflow
			if (rf > 255) rf = 255;
			if (gf > 255) gf = 255;
			if (bf > 255) bf = 255;

			// rebuild RGB and test for overflow
			// and write back to buffer
			tbuffer[iy * temp_text.width + ix] = _RGB16BIT565(rf, gf, bf);

		} // end for ix     

////////////////////////////////////////

// draw textures
	Draw_Bitmap16(&temp_text, back_buffer, back_lpitch, 0);
	Draw_Bitmap16(&textures[curr_texture], back_buffer, back_lpitch, 0);
	Draw_Bitmap16(&lightmaps[curr_lightmap], back_buffer, back_lpitch, 0);

	// unlock back surface
	DDraw_Unlock_Back_Surface();

	// read keyboard
	DInput_Read_Keyboard();

	// test if user wants to change texture
	if (keyboard_state[DIK_RIGHT])
	{
		if (++curr_texture > (NUM_TEXT - 1))
			curr_texture = (NUM_TEXT - 1);

		Wait_Clock(100);
	} // end if

	if (keyboard_state[DIK_LEFT])
	{
		if (--curr_texture < 0)
			curr_texture = 0;

		Wait_Clock(100);
	} // end if


 // test if user wants to change ligthmap texture
	if (keyboard_state[DIK_UP])
	{
		if (++curr_lightmap > (NUM_LMAP - 1))
			curr_lightmap = (NUM_LMAP - 1);

		Wait_Clock(100);
	} // end if

	if (keyboard_state[DIK_DOWN])
	{
		if (--curr_lightmap < 0)
			curr_lightmap = 0;

		Wait_Clock(100);
	} // end if

 // is user changing scaling factor
	if (keyboard_state[DIK_PGUP])
	{
		scalef += .01;
		if (scalef > 10)
			scalef = 10;
		Wait_Clock(10);
	} // end if

	if (keyboard_state[DIK_PGDN])
	{
		scalef -= .01;
		if (scalef < 0)
			scalef = 0;

		Wait_Clock(10);
	} // end if


 // draw title
	Draw_Text_GDI("Use <RIGHT>/<LEFT> arrows to change texture.", 10, 4, RGB(255, 255, 255), lpddsback);
	Draw_Text_GDI("Use <UP>/<DOWN> arrows to change the light map texture.", 10, 20, RGB(255, 255, 255), lpddsback);
	Draw_Text_GDI("Use <PAGE UP>/<PAGE DOWN> arrows to change ambient scale factor.", 10, 36, RGB(255, 255, 255), lpddsback);
	Draw_Text_GDI("Press <ESC> to Exit. ", 10, 56, RGB(255, 255, 255), lpddsback);

	// print stats
	sprintf(buffer, "Texture #%d, Ligtmap #%d, Ambient Scaling Factor: %f", curr_texture, curr_lightmap, scalef);
	Draw_Text_GDI(buffer, 10, WINDOW_HEIGHT - 20, RGB(255, 255, 255), lpddsback);

	// flip the surfaces
	DDraw_Flip();

	// sync to 30ish fps
	Wait_Clock(30);

	// check of user is trying to exit
	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);

		// stop all sounds
		DSound_Stop_All_Sounds();

	} // end if

// return success
	return(1);

} // end Game_Main

//////////////////////////////////////////////////////////

