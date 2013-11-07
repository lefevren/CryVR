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
		EIP_WiimoteTimeOut = 4,
		EIP_aspect = 5,
		EIP_irlevel = 6,
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
			InputPortConfig<bool>("Activate", _HELP("Activate")),
			InputPortConfig<int>("Threshold", _HELP("Min threshold needed to create an event")),
			InputPortConfig<float>("Angle", _HELP("Min angle needed to create")),
			InputPortConfig<int>("Timeout", _HELP("Timeout events")),
			InputPortConfig<bool>("Aspect_ratio",true, _HELP("Set Aspect Ratio : [false, 4/3] [true, 16/9]")),
			InputPortConfig<int>("IR_Sensivity", 3 ,_HELP("Set ir sensivity [0;5]")),
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
		config.sDescription = _HELP("FG node that set wiimote nodes");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
				
		if(event == eFE_Activate){
			if(IsPortActive(pActInfo,0)){
				bool retour = CryVR_WiimoteManager::SetTimeout(GetPortInt(pActInfo, 3));
				retour = retour && CryVR_WiimoteManager::SetAccelThreshold(GetPortInt(pActInfo, 1),GetPortFloat(pActInfo,2));
				retour = retour && CryVR_WiimoteManager::SetAspectRatio(GetPortBool(pActInfo,4));
				retour = retour && CryVR_WiimoteManager::SetIRSensivity(GetPortInt(pActInfo,5));
				ActivateOutput(pActInfo, 0, retour);
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteSetup(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};




class CryVR_WiimoteMotionSensing : public CFlowBaseNode<eNCT_Instanced>
{
public:

	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_WiimoteMotionSensing = 2
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


class CryVR_WiimoteRumble : public CFlowBaseNode<eNCT_Instanced>
{
public:

	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_WiimoteRumble = 2
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



class CFlowWiimoteStop : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_IsServer = 0,
	};

	enum EOutputPorts
	{
		EOP_Status		
	};


public:
	
	
	wiimote** wiimotes;// = wiiuse_init(2); 
	
	////////////////////////////////////////////////////
	CFlowWiimoteStop(SActivationInfo *pActInfo){}
	virtual ~CFlowWiimoteStop(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Activate")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<bool>("Status", _HELP("")),
			{ 0 },
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that cleanup wiimotes stack");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
				
		if(event == eFE_Activate){
			if(IsPortActive(pActInfo,0)){
				bool retour = CryVR_WiimoteManager::Stop();
				ActivateOutput(pActInfo, 0, retour);
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteStop(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};

class CryVR_WiimoteBatteryLevel : public CFlowBaseNode<eNCT_Instanced>
{
public:

	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_WiimoteBatteryLevel = 2
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


	
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteBatteryLevel",  CryVR_WiimoteBatteryLevel);
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteStop", CFlowWiimoteStop);
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteIR",  CryVR_WiimoteIR);
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteRumble",  CryVR_WiimoteRumble);
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteMotionSensing",  CryVR_WiimoteMotionSensing);
REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteSetup", CFlowWiimoteSetup);

