/* Onde Euro Filter float node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "filters/OneEuroFilter.h"

class CFlowOneEuroFilter : public CFlowBaseNode<eNCT_Instanced>
{
	OneEuroFilter *oef;

	enum EInputPorts
	{
		ACTIVE,
		IN_Value,
	};

	enum EOutputPorts
	{
		OUT_Value,
	};

public:
	
	bool active;

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
			InputPortConfig<bool> ("Active",true, _HELP("Activation")),
			
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
				oef = new OneEuroFilter ();
			
			}
			break;
		case eFE_Activate:
			{
				if ( IsPortActive( pActInfo, ACTIVE ) ){
					active = GetPortBool( pActInfo, ACTIVE);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				}
			}
			break;
		
		case eFE_Update:
			{
				if(!active) return;

				oef->increaseTimeStamp(1.0/oef->getFrequence());
				float valeur =  GetPortFloat(pActInfo, IN_Value);
				float filtered = (float)oef->filter(valeur) ;
				ActivateOutput(pActInfo, OUT_Value,filtered);
			}
		}
	}

	////////////////////////////////////////////////////
	virtual void GetMemoryStatistics(ICrySizer *s){
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
