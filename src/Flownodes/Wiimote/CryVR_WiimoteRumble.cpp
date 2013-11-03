/* Wiimote Rumble node - for licensing and copyright see license.txt */

#include "StdAfx.h"


#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "Wiiuse/wiiuse.h" 
#include "Wiimote/CryVR_WiimoteManager.h"


class CryVR_WiimoteRumble : public CFlowBaseNode<eNCT_Instanced>
{
public:

	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_WiimoteMotionSensing = 1
	};

	enum EOutputPorts
	{
		EOP_Status		
	};


	////////////////////////////////////////////////////
	CryVR_WiimoteRumble(SActivationInfo *pActInfo){}
	virtual ~CryVR_WiimoteRumble(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Toogle", _HELP("Activate")),
			InputPortConfig<int>("WiimoteID", _HELP("Wiimote id")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<bool>("Status", _HELP("")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node to set rumble");
		config.SetCategory(EFLN_APPROVED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		if(event==eFE_Activate  && IsPortActive(pActInfo,0)) {
			bool retour = CryVR_WiimoteManager::ToggleRumble(GetPortInt(pActInfo, 1));
			ActivateOutput(pActInfo, 0, retour);
		}

		
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteRumble(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


	
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteRumble",  CryVR_WiimoteRumble);

