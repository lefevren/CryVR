/* Test d'optimisation et de rapidité */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "wiiuse.h" 
#include "CryVR_WiimoteManager.h"


class CryVR_WiimoteMotionSensing : public CFlowBaseNode<eNCT_Instanced>
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
	CryVR_WiimoteMotionSensing(SActivationInfo *pActInfo){}
	virtual ~ CryVR_WiimoteMotionSensing(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Set", _HELP("Activate")),
			InputPortConfig<int>("WiimoteID", _HELP("Wiimote id")),
			InputPortConfig<bool>("Value", _HELP("Activate / Deactivate")),
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
		config.sDescription = _HELP("CryVR node to set motion sensing");
		config.SetCategory(EFLN_APPROVED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		if(event==eFE_Activate  && IsPortActive(pActInfo,0)) {
			bool retour = CryVR_WiimoteManager::SetMotionSensing(GetPortInt(pActInfo, 1),GetPortBool(pActInfo, 2));
			ActivateOutput(pActInfo, 0, retour);
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteMotionSensing(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


	
REGISTER_FLOW_NODE("CryVR:Controlers:WiimoteMotionSensing",  CryVR_WiimoteMotionSensing);

