/* Wii Balance Board node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiimote/CryVR_WiimoteManager.h"
#include "wiiuse/wiiuse.h"


class CryVR_WiimoteGuitarHeroControler : public CFlowBaseNode<eNCT_Instanced>
{
public:

	bool active;
	bool was_motion_sensing_active;
	bool gh3_move;
	
	SActivationInfo m_actInfo;
	int CryVR_WiimoteID;
	
	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1,
		EIP_MAX_EVENTS = 2,
		EIP_FPS_FREE = 3,
	};

	enum EOutputPorts
	{
		WIIMOTE_STATUS, //String
		
		GH3_STRUM_UP,
		GH3_STRUM_DOWN,
		GH3_YELLOW,
		GH3_GREEN,
		GH3_BLUE,
		GH3_RED,
		GH3_ORANGE,
		GH3_PLUS,
		GH3_MINUS,
		GH3_BAR,
		GH3_JOYSTICK_ANGLE,
		GH3_JOYSTICK_MAG,
	};


	////////////////////////////////////////////////////
	 CryVR_WiimoteGuitarHeroControler(SActivationInfo *pActInfo) :
		m_actInfo()
	{
	}
	
	
	////////////////////////////////////////////////////
	virtual ~ CryVR_WiimoteGuitarHeroControler(void){}

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
			OutputPortConfig<bool>("STRUM_UP", _HELP("")),
			OutputPortConfig<bool>("STRUM_UP", _HELP("")),
			OutputPortConfig<bool>("YELLOW", _HELP("")),
			OutputPortConfig<bool>("GREEN", _HELP("")),
			OutputPortConfig<bool>("BLUE", _HELP("")),
			OutputPortConfig<bool>("RED", _HELP("")),
			OutputPortConfig<bool>("ORANGE", _HELP("")),
			OutputPortConfig<bool>("PLUS", _HELP("")),
			OutputPortConfig<bool>("MINUS", _HELP("")),
			OutputPortConfig<float>("BAR", _HELP("")),
			OutputPortConfig<float>("JOYSTICK_ANGLE", _HELP("")),
			OutputPortConfig<float>("JOYSTICK_MAG", _HELP("")),
			
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that retrieve values from Wii Guitar Hero 3 controler");
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
			CryLogAlways(" *** GuitarHero Controler beta version *** ");
			active = GetPortBool(pActInfo, 0);
			
			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			return ;
		}

		if(event== eFE_Update) 
			{
				gh3_move = false;
				int i = 0;
				int w_id = GetPortInt(pActInfo, 1);
				if(!active || w_id < 0 || w_id >= CryVR_WiimoteManager::found) return ;

				
				
				if(CryVR_WiimoteManager::GetMotionSensing(w_id) && CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == 0){
					was_motion_sensing_active = true;
					CryLogAlways(" *** GuitarHero Controler force Motion Sensing off during connection *** ");
					CryVR_WiimoteManager::SetMotionSensing(w_id,false);
				}

				if(CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == EXP_CLASSIC && was_motion_sensing_active){
					CryLogAlways(" *** GuitarHero set Motion Sensing previous state *** ");
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteGuitarHeroControler(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}




void handle_event(struct wiimote_t* wm) {
	
	if (wm->exp.type == EXP_GUITAR_HERO_3) 
	{
		
		struct guitar_hero_3_t* gh3 = (guitar_hero_3_t*)&wm->exp.gh3;

		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_UP))				ActivateOutput(&m_actInfo, GH3_STRUM_UP, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_DOWN))			ActivateOutput(&m_actInfo, GH3_STRUM_DOWN, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_YELLOW))				ActivateOutput(&m_actInfo, GH3_YELLOW, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_GREEN))				ActivateOutput(&m_actInfo, GH3_GREEN, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_BLUE))					ActivateOutput(&m_actInfo, GH3_BLUE, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_RED))					ActivateOutput(&m_actInfo, GH3_RED, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_ORANGE))				ActivateOutput(&m_actInfo, GH3_ORANGE, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_PLUS))					ActivateOutput(&m_actInfo, GH3_PLUS, true);
		if(IS_JUST_PRESSED(gh3, GUITAR_HERO_3_BUTTON_MINUS))				ActivateOutput(&m_actInfo, GH3_MINUS, true);
			
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_STRUM_UP))					ActivateOutput(&m_actInfo, GH3_STRUM_UP, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_STRUM_DOWN))				ActivateOutput(&m_actInfo, GH3_STRUM_DOWN, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_YELLOW))					ActivateOutput(&m_actInfo, GH3_YELLOW, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_GREEN))					ActivateOutput(&m_actInfo, GH3_GREEN, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_BLUE))						ActivateOutput(&m_actInfo, GH3_BLUE, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_RED))						ActivateOutput(&m_actInfo, GH3_RED, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_ORANGE))					ActivateOutput(&m_actInfo, GH3_ORANGE, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_PLUS))						ActivateOutput(&m_actInfo, GH3_PLUS, false);
		if(IS_RELEASED(gh3, GUITAR_HERO_3_BUTTON_MINUS))					ActivateOutput(&m_actInfo, GH3_MINUS, false);
		
		
		if(!gh3_move) {
			ActivateOutput(&m_actInfo, GH3_BAR,  gh3->whammy_bar);
			ActivateOutput(&m_actInfo, GH3_JOYSTICK_ANGLE, gh3->js.ang);
			ActivateOutput(&m_actInfo, GH3_JOYSTICK_MAG, gh3->js.mag);
		}

		gh3_move=true;
	}
}

};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteGuitarHeroControler",  CryVR_WiimoteGuitarHeroControler);

