#ifndef SLIDERLFO_H
#define SLIDERLFO_H

#include "VdjDsp8.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>


//---------------------------------------------------------------------------
// Class definition
//---------------------------------------------------------------------------
class CSliderLFO : public IVdjPluginDsp8
{
public:
	CSliderLFO();
	~CSliderLFO();
	HRESULT VDJ_API OnLoad();
	HRESULT VDJ_API OnGetPluginInfo(TVdjPluginInfo8 *infos);
	ULONG   VDJ_API Release();
	HRESULT VDJ_API OnParameter(int id);
	HRESULT VDJ_API OnGetParameterString(int id, char *outParam, int outParamSize);
	HRESULT VDJ_API OnGetUserInterface(TVdjPluginInterface8 *pluginInterface);
	HRESULT VDJ_API OnStart();
	HRESULT VDJ_API OnStop();
	HRESULT VDJ_API OnProcessSamples(float *buffer, int nb);

private:
	typedef enum _ID_Interface
	{
		ID_INIT,
		ID_SLIDER_1,
		ID_SLIDER_2,
		ID_SLIDER_3,
		ID_SLIDER_4,
		ID_SLIDER_5,
		ID_SWITCH_1,
		ID_SWITCH_2,
	} ID_Interface;

	typedef enum _LFOCURVE
	{
		SINE,
		TRIANGLE,
		SAWTOOTH,
		SQUARE
	} LFOCURVE;

	const int MAX_LFOCURVE = 4;
	const int MAX_SLIDERTYPE = 13;

	// Plugin Interface
	HRESULT OnSlider(int id);
	float m_SliderValue[5];
	double m_Delay;
	LFOCURVE m_LFOcurve;
	int m_SliderType;
	char m_SliderTypeName[50];
	float m_SliderTypeValueStart;
	int m_Inverted;
	int m_Reset;
	float m_SliderMin;
	float m_SliderMax;


	// LFO
    float LFO(LFOCURVE type, double x);
	float ScalingLFO(float x);
};

#endif /* SLIDERLFO_H */ 

