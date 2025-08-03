#include "SliderLFO.h"

//-----------------------------------------------------------------------------
CSliderLFO::CSliderLFO()
{
	m_Delay = 0;
	m_LFOcurve = SINE;
	m_SliderType = 0;
	m_SliderTypeValueStart = 0;
	m_Inverted = 0;
	m_Reset = 0;
	ZeroMemory(m_SliderValue, 2 * sizeof(float));
	ZeroMemory(m_SliderTypeName, 50 * sizeof(char));
}
//-----------------------------------------------------------------------------
CSliderLFO::~CSliderLFO()
{ 

}
//-----------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnLoad()
{	
	HRESULT hr = S_FALSE;

	hr = DeclareParameterSlider(&m_SliderValue[0], ID_SLIDER_1, "LFO Rate", "LR",0.5f);
	hr = DeclareParameterSlider(&m_SliderValue[1], ID_SLIDER_2, "LFO Curve", "LC", 0.0f);
	hr = DeclareParameterSlider(&m_SliderValue[2], ID_SLIDER_3, "VDJscript", "VS", 0.0f);
	hr = DeclareParameterSwitch(&m_Inverted, ID_SWITCH_1, "Inverted", "INV", 0.0f);
	hr = DeclareParameterSwitch(&m_Reset, ID_SWITCH_2, "Reset", "R", 1.0f);
	
	OnParameter(ID_INIT);
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnGetPluginInfo(TVdjPluginInfo8 *infos)
{
	infos->Author = "DJ CEL";
	infos->PluginName = "SliderLFO";
	infos->Description = "Move VirtualDJ slider on the beat";
	infos->Version = "1.0";
	infos->Flags = 0x00;

	return S_OK;
}
//---------------------------------------------------------------------------
ULONG VDJ_API CSliderLFO::Release()
{
	delete this;
	return 0;
}
//------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnGetUserInterface(TVdjPluginInterface8 *pluginInterface)
{
	pluginInterface->Type = VDJINTERFACE_DEFAULT;

	return S_OK;
}
//------------------------------------------------------------------------------
// User Interface
//------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnParameter(int id)
{
	if (id == ID_INIT)
	{
		for (int i = ID_SLIDER_1; i <= ID_SLIDER_3; i++) OnSlider(i);
	}
	else OnSlider(id);

	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT CSliderLFO::OnSlider(int id)
{
	HRESULT hr = S_OK;
	int LFOcurve_value = 0;
	double result = 0;

	switch (id)
	{
		case ID_SLIDER_1:
			if (m_SliderValue[0] == 0.0f)										m_Delay = 0.125f;
			else if (m_SliderValue[0] > 0.0f && m_SliderValue[0] < 0.125f)		m_Delay = 0.25f;
			else if (m_SliderValue[0] >= 0.125f && m_SliderValue[0] < 0.25f)    m_Delay = 0.5f;
			else if (m_SliderValue[0] >= 0.25f && m_SliderValue[0] < 0.375f)	m_Delay = 1.0f;
			else if (m_SliderValue[0] >= 0.375f && m_SliderValue[0] < 0.5f)     m_Delay = 2.0f;
			else if (m_SliderValue[0] >= 0.5f && m_SliderValue[0] < 0.625f)		m_Delay = 4.0f;
			else if (m_SliderValue[0] >= 0.625 && m_SliderValue[0] < 0.75f)		m_Delay = 6.0f;
			else if (m_SliderValue[0] >= 0.75f && m_SliderValue[0] < 0.875f)	m_Delay = 8.0f;
			else if (m_SliderValue[0] >= 0.875f && m_SliderValue[0] < 1.0f)     m_Delay = 16.0f;
			else if (m_SliderValue[0] == 1.0f)									m_Delay = 32.0f;
			break;

		case ID_SLIDER_2:
			LFOcurve_value = int(1.0f + m_SliderValue[1] * (MAX_LFOCURVE - 1.0f));
			switch (LFOcurve_value)
			{
				case 1:
					m_LFOcurve = SINE;
					break;
				case 2:
					m_LFOcurve = TRIANGLE;
					break;
				case 3:
					m_LFOcurve = SAWTOOTH;
					break;
				case 4:
					m_LFOcurve = SQUARE;
					break;
			}
			break;

		case ID_SLIDER_3:
			if (m_Reset)
			{
				char vdjscript[100] = "\0";
				sprintf_s(vdjscript, 100 * sizeof(char), "%s %.5f", m_SliderTypeName, m_SliderTypeValueStart);
				SendCommand(vdjscript);
			}
			m_SliderType = int(1 + m_SliderValue[2] * (MAX_SLIDERTYPE - 1.0f));
			switch (m_SliderType)
			{
				case 1:
					wsprintf(m_SliderTypeName, "level");
					break;

				case 2:
					wsprintf(m_SliderTypeName, "crossfader");
					break;

				case 3:
					wsprintf(m_SliderTypeName, "video_crossfader");
					break;

				case 4:
					wsprintf(m_SliderTypeName, "video_fx_slider");
					break;

				case 5:
					wsprintf(m_SliderTypeName, "master_balance");
					break;
			}
			
			hr = GetInfo(m_SliderTypeName, &result);
			if (hr == S_OK)
			{
				m_SliderTypeValueStart = (float)result;
			}
			break;
	}

	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnGetParameterString(int id, char *outParam, int outParamSize) 
{
	switch(id)
	{
		case ID_SLIDER_1:
			if (m_Delay == 0.0f) sprintf_s(outParam, outParamSize,"OFF");
			else if (m_Delay < 1.0f) sprintf_s(outParam, outParamSize,"1/%.0f beat", 1 / m_Delay);
			else sprintf_s(outParam, outParamSize,"%.0f beat(s)", m_Delay);
			break;

		case ID_SLIDER_2:
			switch (m_LFOcurve)
			{
			case SINE:
				sprintf_s(outParam, outParamSize, "Sine");
				break;

			case TRIANGLE:
				sprintf_s(outParam, outParamSize, "Triangle");
				break;

			case SAWTOOTH:
				sprintf_s(outParam, outParamSize, "Sawtooth");
				break;

			case SQUARE:
				sprintf_s(outParam, outParamSize, "Square");
				break;
			}
			break;

		case ID_SLIDER_3:
			sprintf_s(outParam, outParamSize, "%s", m_SliderTypeName);
			break;

	}
	return S_OK;
}
//-------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnStart()
{
	HRESULT hr = S_OK;
	double result = 0;
	hr = GetInfo(m_SliderTypeName, &result);
	if (hr == S_OK)
	{
		m_SliderTypeValueStart = (float)result;
	}
	
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnStop()
{
	if (m_Reset)
	{
		char vdjscript[100] = "\0";
		sprintf_s(vdjscript, 100 * sizeof(char), "%s %.5f", m_SliderTypeName, m_SliderTypeValueStart);
		SendCommand(vdjscript);
	}

	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT VDJ_API CSliderLFO::OnProcessSamples(float *buffer, int nb)
{
	float y = 0;
	double xBeat = 0;	
	
	xBeat = fmod(SongPosBeats, m_Delay) / m_Delay;

	y = LFO(m_LFOcurve, xBeat);  // LFO sur 'Delay' beat(s)

	if (m_Inverted)
	{
		y = 1.0f - y;
	}

	char vdjscript[100] = "\0";
	sprintf_s(vdjscript, 100 * sizeof(char), "%s %.5f", m_SliderTypeName, y);
	SendCommand(vdjscript);
	
	return S_OK;
}
//------------------------------------------------------------------------------
float CSliderLFO::LFO(LFOCURVE type, double x)
{
	// LFO curves for x in [0.0f , 1.0f] and y in [0.0f , 1.0f]

	double y = 0.0f;

	switch (type)
	{
		case SINE:
			y = 0.5f + 0.5f * sin(2.0f * M_PI * (x - 0.25f));
			break;

		case SAWTOOTH:
			y = fmod(x, 1.0);
			//ValLFO = 1.0 - fmod(x, 1.0);
			break;

		case TRIANGLE:
			y = 1.0f - fabs(2.0 * fmod(x, 1.0) - 1.0);
			break;

		case SQUARE:
			y = (fmod(x, 1.0) < 0.5) ? 1.0f : 0.0f;
			break;
	}

	return float(y);
}



