/* Wii Balance Board node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiimote/CryVR_WiimoteManager.h"
#include "wiiuse/wiiuse.h"

class CryVR_WiimoteClassicControler : public CFlowBaseNode<eNCT_Instanced>
{
public:

	bool active;
	bool was_motion_sensing_active;
	bool classic_move;
	
	SActivationInfo m_actInfo;
	int CryVR_WiimoteID;
	
	enum EInputPorts
	{
		EIP_Activate = 0,
		EIP_WiimoteID = 1,
		EIP_MAX_EVENTS = 2,
		EIP_FPS_FREE = 3,
	};

	enum EOutputPorts
	{
		WIIMOTE_STATUS, //String
		CLASSIC_A, //Pressed
		CLASSIC_B, //Pressed
		CLASSIC_X, //Pressed
		CLASSIC_Y, //Pressed
		CLASSIC_ZL, //Pressed
		CLASSIC_ZR, //Pressed
		CLASSIC_TOP, //Pressed
		CLASSIC_DOWN, //Pressed
		CLASSIC_LEFT, //Pressed
		CLASSIC_RIGHT, //Pressed
		CLASSIC_FULL_L, //Pressed
		CLASSIC_FULL_R, //Pressed
		CLASSIC_MINUS, //Pressed
		CLASSIC_PLUS, //Pressed
		CLASSIC_HOME, //Pressed
		


		CLASSIC_ANALOG_SHOULDER_L, //Pressed
		CLASSIC_ANALOG_SHOULDER_R, //Pressed
		CLASSIC_ANALOG_LEFT_ANGLE, //Pressed
		CLASSIC_ANALOG_LEFT_MAG,
		CLASSIC_ANALOG_RIGHT_ANGLE, //Pressed
		CLASSIC_ANALOG_RIGHT_MAG,

	};


	////////////////////////////////////////////////////
	 CryVR_WiimoteClassicControler(SActivationInfo *pActInfo) :
		m_actInfo()
	{
	}
	
	
	////////////////////////////////////////////////////
	virtual ~ CryVR_WiimoteClassicControler(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Activate")),
			InputPortConfig<int>("WiimoteID",0, _HELP("Wiimote ID")),
			InputPortConfig<int>("MaxEvents",3,_HELP("Maximum events by frame")),
			InputPortConfig<bool>("FPS_FREE",true, _HELP("Force Node to execute minimum events")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("")),
			OutputPortConfig<bool>("A", _HELP("")),
			OutputPortConfig<bool>("B", _HELP("")),
			OutputPortConfig<bool>("X", _HELP("")),
			OutputPortConfig<bool>("Y", _HELP("")),
			OutputPortConfig<bool>("ZL", _HELP("")),
			OutputPortConfig<bool>("ZR", _HELP("")),
			OutputPortConfig<bool>("TOP", _HELP("")),
			OutputPortConfig<bool>("DOWN", _HELP("")),
			OutputPortConfig<bool>("LEFT", _HELP("")),
			OutputPortConfig<bool>("RIGHT", _HELP("")),
			OutputPortConfig<bool>("FULL_L", _HELP("")),
			OutputPortConfig<bool>("FULL_R", _HELP("")),
			OutputPortConfig<bool>("MINUS", _HELP("")),
			OutputPortConfig<bool>("PLUS", _HELP("")),
			OutputPortConfig<bool>("HOME", _HELP("")),
			
			OutputPortConfig<float>("LEFT_SHOULDER", _HELP("")),
			OutputPortConfig<float>("RIGHT_SHOULDER", _HELP("")),
			OutputPortConfig<float>("LEFT_JOYSTICK_ANGLE", _HELP("")),
			OutputPortConfig<float>("LEFT_JOYSTICK_MAG", _HELP("")),
			
			OutputPortConfig<float>("RIGHT_JOYSTICK_ANGLE", _HELP("")),
			OutputPortConfig<float>("RIGHT_JOYSTICK_MAG", _HELP("")),
					
			
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that retrieve values from Wii classic controler");
		config.SetCategory(EFLN_APPROVED);
	}	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{

		if(event==eFE_Initialize) {
			m_actInfo = *pActInfo;
			return ;
		}

		if(event==eFE_Activate){
			CryLogAlways(" *** Classic Controler beta version *** ");
			active = GetPortBool(pActInfo, 0);
			
			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			return ;
		}

		if(event== eFE_Update) 
			{
				classic_move = false;
				int i = 0;
				int w_id = GetPortInt(pActInfo, 1);
				if(!active || w_id < 0 || w_id >= CryVR_WiimoteManager::found) return ;

				
				
				if(CryVR_WiimoteManager::GetMotionSensing(w_id) && CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == 0){
					was_motion_sensing_active = true;
					CryLogAlways(" *** Classic Controler force Motion Sensing off during connection *** ");
					CryVR_WiimoteManager::SetMotionSensing(w_id,false);
				}

				if(CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == EXP_CLASSIC && was_motion_sensing_active){
					CryLogAlways(" *** Classic set Motion Sensing previous state *** ");
					//CryVR_WiimoteManager::SetMotionSensing(w_id,true);
					was_motion_sensing_active = false;
					CryVR_WiimoteManager::Status(CryVR_WiimoteManager::GetWiimotes(w_id));
				}
				
				
				/* fait planter la balance board ! */

				while (wiiuse_poll(CryVR_WiimoteManager::GetWiimotes(), CryVR_WiimoteManager::found) && i<GetPortInt(pActInfo, 2)) 
					{	
						if(CryVR_WiimoteManager::GetWiimotes(w_id)->event == WIIUSE_EVENT) {
							
							handle_event(CryVR_WiimoteManager::GetWiimotes(w_id));
						}

						//Recherche d'optimisation.
						if (GetPortBool(pActInfo, 3)) {
							return ; 
						}
						i++;
					}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteClassicControler(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}




void handle_event(struct wiimote_t* wm) {
	
	if (wm->exp.type == EXP_CLASSIC) {
		
			struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;
			//OK
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_A))			ActivateOutput(&m_actInfo, CLASSIC_A, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_B))			ActivateOutput(&m_actInfo, CLASSIC_B, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_X))			ActivateOutput(&m_actInfo, CLASSIC_X, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_Y))			ActivateOutput(&m_actInfo, CLASSIC_Y, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZL))			ActivateOutput(&m_actInfo, CLASSIC_ZL, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZR))			ActivateOutput(&m_actInfo, CLASSIC_ZR, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP))			ActivateOutput(&m_actInfo, CLASSIC_TOP, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_DOWN))		ActivateOutput(&m_actInfo, CLASSIC_DOWN, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_LEFT))		ActivateOutput(&m_actInfo, CLASSIC_LEFT, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_RIGHT))		ActivateOutput(&m_actInfo, CLASSIC_RIGHT, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_L))		ActivateOutput(&m_actInfo, CLASSIC_FULL_L, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_R))		ActivateOutput(&m_actInfo, CLASSIC_FULL_R, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_MINUS))		ActivateOutput(&m_actInfo, CLASSIC_MINUS, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_PLUS))		ActivateOutput(&m_actInfo, CLASSIC_PLUS, true);
			if(IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_HOME))		ActivateOutput(&m_actInfo, CLASSIC_HOME, true);
			
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_A))				ActivateOutput(&m_actInfo, CLASSIC_A, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_B))				ActivateOutput(&m_actInfo, CLASSIC_B, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_X))				ActivateOutput(&m_actInfo, CLASSIC_X, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_Y))				ActivateOutput(&m_actInfo, CLASSIC_Y, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_ZL))				ActivateOutput(&m_actInfo, CLASSIC_ZL, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_ZR))				ActivateOutput(&m_actInfo, CLASSIC_ZR, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_UP))				ActivateOutput(&m_actInfo, CLASSIC_TOP, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_DOWN))			ActivateOutput(&m_actInfo, CLASSIC_DOWN, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_LEFT))			ActivateOutput(&m_actInfo, CLASSIC_LEFT, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_RIGHT))			ActivateOutput(&m_actInfo, CLASSIC_RIGHT, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_FULL_L))			ActivateOutput(&m_actInfo, CLASSIC_FULL_L, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_FULL_R))			ActivateOutput(&m_actInfo, CLASSIC_FULL_R, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_MINUS))			ActivateOutput(&m_actInfo, CLASSIC_MINUS, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_PLUS))			ActivateOutput(&m_actInfo, CLASSIC_PLUS, false);
			if(IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_HOME))			ActivateOutput(&m_actInfo, CLASSIC_HOME, false);
			
		
			//Optimisation
			if(!classic_move) {
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_SHOULDER_L, cc->l_shoulder);
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_SHOULDER_R, cc->r_shoulder);
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_LEFT_ANGLE, cc->ljs.ang);
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_LEFT_MAG, cc->ljs.mag);
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_RIGHT_ANGLE, cc->rjs.ang);
				ActivateOutput(&m_actInfo, CLASSIC_ANALOG_RIGHT_MAG, cc->rjs.mag);

				classic_move = true;
			}
		}
	/*
	else {
		//ActivateOutput(&m_actInfo, 0, "Incompatible expansion type");
		//CryLogAlways("Bad Expansion type");
	}
	*/
	}
};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteClassicControler",  CryVR_WiimoteClassicControler);

