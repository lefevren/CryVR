/* Wiimote IR node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "Wiiuse/wiiuse.h" 
#include "Wiimote/CryVR_WiimoteManager.h"


class CryVR_WiimoteIR : public CFlowBaseNode<eNCT_Instanced>
{
public:

	enum EInputPorts
	{
		EIP_Set = 0,
		EIP_Get = 1,
		EIP_WiimoteID = 2,
		EIP_WiimoteMotionSensing = 3
	};

	enum EOutputPorts
	{
		EOP_Status		
	};


	////////////////////////////////////////////////////
	CryVR_WiimoteIR(SActivationInfo *pActInfo){}
	virtual ~ CryVR_WiimoteIR(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Set", _HELP("Activate")),
			InputPortConfig<bool>("Get", _HELP("Activate")),
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
		config.sDescription = _HELP("CryVR node to activate / deactivate IR output values");
		config.SetCategory(EFLN_APPROVED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		if(event==eFE_Activate  && IsPortActive(pActInfo,0)) {
			bool retour = CryVR_WiimoteManager::SetIr(GetPortInt(pActInfo, 2),GetPortBool(pActInfo, 3));
			ActivateOutput(pActInfo, 0, retour);
		}
		if(event==eFE_Activate  && IsPortActive(pActInfo,1)) {
			bool retour = CryVR_WiimoteManager::GetIr(GetPortInt(pActInfo, 2));
			ActivateOutput(pActInfo, 0, retour);
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteIR(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


	
REGISTER_FLOW_NODE("CryVR:Controlers:WiimoteIR",  CryVR_WiimoteIR);

