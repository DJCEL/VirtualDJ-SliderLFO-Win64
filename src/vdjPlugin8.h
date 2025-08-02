//////////////////////////////////////////////////////////////////////////
//
// VirtualDJ
// Plugin SDK
// (c)Atomix Productions 2011-2013
//
//////////////////////////////////////////////////////////////////////////
//
// This file defines the basic functions that are used in all plugins.
// It defines the functions and variables needed to:
// - load and unload a plugin
// - give the infos about the plugin (name, picture, etc)
// - get the parameters automatically saved and restored between loads
// - interact with VirtualDJ (ask queries or send commands)
// - implement a custom interface
//
// Other functions specific to particular types of plugin can be found
// in their respective header file
//
//////////////////////////////////////////////////////////////////////////


#ifndef VdjPlugin8H
#define VdjPlugin8H

//////////////////////////////////////////////////////////////////////////
// Platform specific defines for compatibility Mac/Windows
#ifdef VDJ_NOEXPORT
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32_))
	#define WIN32_LEAN_AND_MEAN //For preventing linking errors with DllGetClassObject
	#include <windows.h>
	#define VDJ_WIN
	#if (defined(_M_X64) || defined(__amd64__)) 
		#define VDJ_IS_WIN64  // if you need special code for the 64bit version of VirtualDJ, use this
	#endif
	#define VDJ_EXPORT		__declspec( dllexport )
	#define VDJ_API			__stdcall
	#define VDJ_BITMAP		HBITMAP
	#define VDJ_HINSTANCE	HINSTANCE
	#define VDJ_WINDOW		HWND
#elif (defined(__APPLE__) || defined(MACOSX) || defined(__MACOSX__))
	#include <CoreFoundation/CoreFoundation.h>
	#include <MacTypes.h>
	#define VDJ_MAC
	#define VDJ_EXPORT		__attribute__ ((visibility ("default")))
	#define VDJ_API
	#define VDJ_BITMAP		char *
	#define VDJ_HINSTANCE	CFBundleRef
	#define VDJ_WINDOW		WindowRef
	#define S_OK            ((HRESULT)0x00000000L)
	#define S_FALSE         ((HRESULT)0x00000001L)
	#define E_NOTIMPL       ((HRESULT)0x80004001L)
	#define CLASS_E_CLASSNOTAVAILABLE -1
	#define NO_ERROR		0
	typedef long HRESULT;
	typedef unsigned long ULONG;
	typedef unsigned long DWORD;
	#ifndef GUID_DEFINED
		#define GUID_DEFINED
		typedef struct _GUID {
		unsigned long Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char Data4[ 8 ];
		} GUID;
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
// Internal structures
struct IVdjCallbacks8
{
	virtual HRESULT SendCommand(char *command)=0;
	virtual HRESULT GetInfo(char *command,double *result)=0;
	virtual HRESULT GetStringInfo(char *command,void *result,int size)=0;
	virtual HRESULT DeclareParameter(void *parameter,int type,int id,char *name,char *shortName,float defaultvalue)=0;
	virtual HRESULT GetSongBuffer(int pos, int nb, short **buffer)=0;
};


//////////////////////////////////////////////////////////////////////////
// Standard structures and defines

// structure used in plugin identification
struct TVdjPluginInfo8
{
	const char *PluginName;
	const char *Author;
	const char *Description;
	const char *Version;
	VDJ_BITMAP Bitmap;
	DWORD Flags;
};

// flags used for plugin's parameters
#define VDJPARAM_BUTTON	0
#define VDJPARAM_SLIDER	1
#define VDJPARAM_SWITCH	2
#define VDJPARAM_STRING	3
#define VDJPARAM_CUSTOM	4
#define VDJPARAM_RADIO	5

#define VDJFLAG_NODOCK 0x1
#define VDJFLAG_PROCESSAFTERSTOP 0x2

// structure used for custom interfaces
#define VDJINTERFACE_DEFAULT	0
#define VDJINTERFACE_SKIN		1
#define VDJINTERFACE_DIALOG		2
struct TVdjPluginInterface8
{
	DWORD Type;
	// xml and image buffers if Type==VDJINTERFACE_SKIN
	char *Xml;
	void *ImageBuffer;
	int ImageSize;
	// HWND returned by CreateDialog or CreateWindow if Type==VDJINTERFACE_DIALOG
	VDJ_WINDOW hWnd;
};

//////////////////////////////////////////////////////////////////////////
// Base class

struct IVdjCallbacks8;

class IVdjPlugin8
{
public:
	// Initialization
	virtual HRESULT VDJ_API OnLoad() {return S_OK;}
	virtual HRESULT VDJ_API OnGetPluginInfo(TVdjPluginInfo8 *info) {return E_NOTIMPL;}
	virtual ULONG VDJ_API Release() {delete this; return S_OK;}
	virtual ~IVdjPlugin8() {}

	// call DeclareParameter() for all your variables during OnLoad()
	// if type=VDJPARAM_CUSTOM or VDJPARAM_STRING, defaultvalue must be set to sizeof(*parameter)
	HRESULT DeclareParameter(void *parameter, int type, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,type,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterButton(int *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_BUTTON,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterSlider(float *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_SLIDER,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterSwitch(int *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_SWITCH,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterString(char *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_STRING,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterCustom(char *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_CUSTOM,id,name,shortName,defaultvalue);}
	HRESULT DeclareParameterRadio(int *parameter, int id, char *name, char *shortName, float defaultvalue) {return cb->DeclareParameter(parameter,VDJPARAM_RADIO,id,name,shortName,defaultvalue);}

	// OnParameter will be called each time a parameter is changed from within VirtualDJ
	virtual HRESULT VDJ_API OnParameter(int id) {return S_OK;}
	// OnGetParameterString will be called each time the string label of a parameter is requested by VirtualDJ
	virtual HRESULT VDJ_API OnGetParameterString(int id, char *outParam, int outParamSize) {return E_NOTIMPL;} ;

	// Custom user-interface
	// Fill the HWND or xml/bitmap info in the passed pluginInterface structure, to define your own plugin window
	virtual HRESULT VDJ_API OnGetUserInterface(TVdjPluginInterface8 *pluginInterface) {return E_NOTIMPL;}
	VDJ_HINSTANCE hInstance;


	// send a VDJScript command to VirtualDJ
	HRESULT SendCommand(char *command) {return cb->SendCommand(command);}
	// get info from VirtualDJ (as a value, or as a utf-8 string)
	HRESULT GetInfo(char *command, double *result) {return cb->GetInfo(command,result);}
	HRESULT GetStringInfo(char *command, char *result, int size) {return cb->GetStringInfo(command,result,size);}



	IVdjCallbacks8 *cb;
};

//////////////////////////////////////////////////////////////////////////
// GUID definitions

#ifndef VDJCLASS8GUID_DEFINED
#define VDJCLASS8GUID_DEFINED
static const GUID CLSID_VdjPlugin8 = { 0xED8A8D87, 0xF4F9, 0x4DCD, { 0xBD, 0x24, 0x29, 0x14, 0x12, 0xE9, 0x3B, 0x60 } };
static const GUID IID_IVdjPluginBasic8 = { 0xa1d90ea1, 0x4d0d, 0x42dd, { 0xa4, 0xd0, 0xb8, 0xf3, 0x37, 0xb3, 0x21, 0xf1 } };
#else
extern static const GUID CLSID_VdjPlugin8;
extern static const GUID IID_IVdjPluginBasic8;
#endif

//////////////////////////////////////////////////////////////////////////
// DLL export function

#ifndef NODLLEXPORT
#ifdef __cplusplus
extern "C" {
#endif
	VDJ_EXPORT HRESULT VDJ_API DllGetClassObject(const GUID &rclsid,const GUID &riid,void** ppObject);
#ifdef __cplusplus
}
#endif
#endif

//////////////////////////////////////////////////////////////////////////

#endif