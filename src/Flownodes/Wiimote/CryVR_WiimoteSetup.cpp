/* Wiimote Setup node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiiuse/wiiuse.h" 
#include "Wiimote/CryVR_WiimoteManager.h"


class CFlowWiimoteSetup : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_WiimoteThreshold = 2,
		EIP_WiimoteAngle = 3,
		EIP_WiimoteTimeOut = 2,
	};

	enum EOutputPorts
	{
		EOP_Status		
	};


public:
	
	
	wiimote** wiimotes;// = wiiuse_init(2); 
	
	////////////////////////////////////////////////////
	CFlowWiimoteSetup(SActivationInfo *pActInfo){}
	virtual ~CFlowWiimoteSetup(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Connexion à un server VRPN")),
			InputPortConfig<int>("Threshold", _HELP("Min threshold needed to create an event")),
			InputPortConfig<float>("Angle", _HELP("Min angle needed to create")),
			InputPortConfig<int>("Timeout", _HELP("Timeout events")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("")),
			{ 0 },

		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that retrieve values from wiimote");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
				
		if(event == eFE_Activate){
			if(IsPortActive(pActInfo,0)){
				bool retour = CryVR_WiimoteManager::SetTimeout(GetPortInt(pActInfo, 3));
				retour = retour && CryVR_WiimoteManager::SetAccelThreshold(GetPortInt(pActInfo, 1),GetPortFloat(pActInfo,2));
				ActivateOutput(pActInfo, 0, retour);
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteSetup(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteSetup", CFlowWiimoteSetup);

