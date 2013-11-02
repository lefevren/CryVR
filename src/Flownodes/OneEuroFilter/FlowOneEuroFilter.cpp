#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "VR/OneEuroFilter.h"

class CFlowOneEuroFilter : public CFlowBaseNode<eNCT_Instanced>
{
	OneEuroFilter *oef;

	enum EInputPorts
	{
		//PARAM_Freq,
		//PARAM_MinCutOff,
		//PARAM_Beta,
		//PARAM_DCutOff,
		IN_Value,
	};

	enum EOutputPorts
	{
		OUT_Value,
	};

public:
	
	////////////////////////////////////////////////////
	CFlowOneEuroFilter(SActivationInfo *pActInfo){}
	////////////////////////////////////////////////////
	virtual ~CFlowOneEuroFilter(void){}
	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			//InputPortConfig<float> ("Frequence",120,_HELP("Ne sert a rien?")),
			//InputPortConfig<float> ("MinCutOff",1.0, _HELP("Min cut Off")),
			//InputPortConfig<float> ("beta",1.0, _HELP("?")),
			//InputPortConfig<float> ("DCutOff",1.0, _HELP("Derivate Cut Off ?")),
			InputPortConfig<float> ("Valeur",1.0, _HELP("Valeur a filtrer")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<float> ("Valeur filtree", _HELP("Fov courrant")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that sets up a CAVE environment");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{	
		switch (event)
		{
		case eFE_Initialize:
			{
				
				//float frequence_ = GetPortFloat(pActInfo,PARAM_Freq);
				//float mincutoff_ = GetPortFloat(pActInfo,PARAM_MinCutOff);
				//float beta__ = 	GetPortFloat(pActInfo,PARAM_Beta);
				//float dcutoff_ = GetPortFloat(pActInfo,PARAM_DCutOff);


				//oef = new OneEuroFilter (frequence_,mincutoff_,beta__,dcutoff_);
				oef = new OneEuroFilter ();
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
		case eFE_Activate:
			{
				
				//CryLogAlways("Activation EuroFilter"); 
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
			break;
		
		case eFE_Update:
			{
				oef->increaseTimeStamp(1.0/oef->getFrequence());
				float valeur =  GetPortFloat(pActInfo, IN_Value);
				float filtered = (float)oef->filter(valeur) ;
				
				ActivateOutput(pActInfo, OUT_Value,filtered);
			}
		}
	}

	////////////////////////////////////////////////////
	virtual void GetMemoryStatistics(ICrySizer *s)
	{
		s->Add(*this);
	}

	////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
	{
		return new CFlowOneEuroFilter(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
		
	}


};


REGISTER_FLOW_NODE("CryVR:Filters:OneEuroFilter", CFlowOneEuroFilter);
