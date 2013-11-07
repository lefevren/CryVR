/* Wiimote Manager header - for licensing and copyright see license.txt */

#pragma once


#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "wiiuse/wiiuse.h" 

#define MAX_WIIMOTES				1

class CryVR_WiimoteManager : public CFlowBaseNode<eNCT_Instanced>
{

		static wiimote** wiimotes;
		


	public:
		bool active;
		
		static int found;
		static int connected;
		
		static bool init;
		
		static int timeout;
		static int threshold;
		static float angle_threshold;
		static int bt;
		
		

		SActivationInfo m_actInfo;

		enum EInputPorts
		{
			EIP_bActivate = 0,
			EIP_bIR = 1,
			//EIP_bMotion = 2,
			EIP_iThreshold = 3,
			EIP_iAngle = 4,
			EIP_iTimeout = 5,
			EIP_BLUETOOTH = 6,
			EIP_ASPECT_RATIO = 7,
			EIP_IRLEVEL = 8,
		};

		enum EOutputPorts
		{
			EIP_sStatus = 0,
		};
		


		CryVR_WiimoteManager(SActivationInfo *pActInfo);
		static wiimote** GetWiimotes(){return wiimotes;}
		
		//To avoid array index out of bounds
		static wiimote* GetWiimotes(int id){
			if(id<0 || id>= found) {
				CryLogAlways("Wiimotes index out of bounds ! returning wiimote 0");
				return wiimotes[0];
			}
			return wiimotes[id];
		}

		/* Init */
		static void Init();
		static void Init(bool ir_pos,bool motion, int threshold , float angle, int timeout,int bluetooth, bool aspect, int irlevel);						//Init avec paramètres
		static void SetBluetooth(int value);

		/* Leds */
		static bool SetLeds(int id,int led);					
		
		/* Motion Sensing */
		static bool SetMotionSensing(int id,bool value);		
		static bool SetMotionSensing(bool value);				
		static bool GetMotionSensing(int id){return WIIUSE_USING_ACC(wiimotes[id]);}
		
		/* Battery */
		static float GetBatteryLevel(int id);
		
		/* Types */
		static int GetType(int id);
		static void Status(struct wiimote_t* wm);
		static void Status(int id);
		
		/* Rumble */
		static bool ToggleRumble(int id);			
		
		/* IR */
		static bool SetIr(int id, bool value);
		static bool SetIr(bool value);
		static bool GetIr(int id);				
		static bool SetIrPosition(bool above);
		static bool SetAspectRatio(bool aspect);
		static bool SetIRSensivity(int level);

		static bool SetAccelThreshold(int i,float angle);
		static bool SetTimeout(int value);


		/* stop */
		static bool Stop();
		
		/* FlowNode */
		virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo);
		virtual ~CryVR_WiimoteManager(void);
		virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser);
		virtual void GetConfiguration(SFlowNodeConfig& config);
		virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_WiimoteManager(pActInfo);}
		virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
};