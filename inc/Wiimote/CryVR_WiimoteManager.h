#pragma once


#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "wiiuse.h" 

#define MAX_WIIMOTES				1

class CryVR_WiimoteManager : public CFlowBaseNode<eNCT_Instanced>
{
	public:
		static int found;
		static int connected;
		static wiimote** wiimotes;
		static bool init;

		SActivationInfo m_actInfo;

		enum EInputPorts
		{
			EIP_bActivate = 0,
			EIP_bIR = 1,
			EIP_bMotion = 2,
		};

		enum EOutputPorts
		{
			EIP_sStatus = 0,
		};
		


		CryVR_WiimoteManager(SActivationInfo *pActInfo);
		
		/* Init */
		static void Init();
		static void Init(bool ir_pos,bool motion);						//Init avec paramètres

		/* Leds */
		static bool SetLeds(int id,int led);					
		
		/* Motion Sensing */
		static bool SetMotionSensing(int id,bool value);		
		static bool SetMotionSensing(bool value);				//Set pour toutes les wiimotes
		
		/* Battery */
		static float GetBatteryLevel(int id);
		
		/* Types */
		static int GetType(int id);							
		
		/* Rumble */
		static bool ToggleRumble(int id);			
		
		/* IR */
		static bool SetIr(int id, bool value);
		static bool SetIr(bool value);
		static bool GetIr(int id);				
		static bool SetIrPosition(bool above);
		
		static bool SetAccelThreshold(int i);
		
			/* FlowNode */
		virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo);
		virtual ~CryVR_WiimoteManager(void);
		virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser);
		virtual void GetConfiguration(SFlowNodeConfig& config);
		virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_WiimoteManager(pActInfo);}
		virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
		

};