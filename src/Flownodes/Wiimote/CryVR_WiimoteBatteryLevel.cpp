/* Wiimote Battery Level node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "Wiiuse/wiiuse.h" 
#include "Wiimote/CryVR_WiimoteManager.h"


class CryVR_WiimoteBatteryLevel : public CFlowBaseNode<eNCT_Instanced>
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
	CryVR_WiimoteBatteryLevel(SActivationInfo *pActInfo){}
	virtual ~ CryVR_WiimoteBatteryLevel(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Get", _HELP("Activate")),
			InputPortConfig<int>("WiimoteID", _HELP("Wiimote id")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<float>("BatteryLevel", _HELP("")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node to get battery level");
		config.SetCategory(EFLN_APPROVED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		if(event==eFE_Activate  && IsPortActive(pActInfo,0)) {
			float retour = CryVR_WiimoteManager::GetBatteryLevel(GetPortInt(pActInfo, 1));
			ActivateOutput(pActInfo, 0, retour);
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteBatteryLevel(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


	
REGISTER_FLOW_NODE("CryVR:Controlers:WiimoteBatteryLevel",  CryVR_WiimoteBatteryLevel);

