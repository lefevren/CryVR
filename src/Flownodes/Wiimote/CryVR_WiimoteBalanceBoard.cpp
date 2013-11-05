/* Wii Balance Board node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiimote/CryVR_WiimoteManager.h"
#include "wiiuse/wiiuse.h"

class CryVR_WiimoteBalanceBoard : public CFlowBaseNode<eNCT_Instanced>
{
public:

	bool active;
	bool was_motion_sensing_active;

	SActivationInfo m_actInfo;
	int CryVR_WiimoteID;
	
	enum EInputPorts
	{
		EIP_IsServer = 0,
		EIP_WiimoteID = 1
	};

	enum EOutputPorts
	{
		WIIMOTE_STATUS, //String
		WIIMOTE_A, //Pressed
		BALANCE_BOARD_LEFT,
		BALANCE_BOARD_RIGHT,
		BALANCE_BOARD_FRONT,
		BALANCE_BOARD_BACK
	};


	////////////////////////////////////////////////////
	 CryVR_WiimoteBalanceBoard(SActivationInfo *pActInfo) :
		m_actInfo()
	{
	}
	
	
	////////////////////////////////////////////////////
	virtual ~ CryVR_WiimoteBalanceBoard(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Connexion à un server VRPN")),
			InputPortConfig<int>("WiimoteID", _HELP("Connexion à un server VRPN")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("")),
			OutputPortConfig<bool>("A", _HELP("")),
			OutputPortConfig<Vec3>("Balance_board_left", _HELP("Balance board left values")),
			OutputPortConfig<Vec3>("Balance_board_right", _HELP("Balance board right values")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that retrieve values from Wii Balance Board");
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
			active = GetPortBool(pActInfo, 0);
			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			return ;
		}

		if(event== eFE_Update) 
			{
				int w_id = GetPortInt(pActInfo, 1);
				if(!active || w_id < 0 || w_id >= CryVR_WiimoteManager::found) return ;

				
				
				if(CryVR_WiimoteManager::GetMotionSensing(w_id) && CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == 0){
					was_motion_sensing_active = true;
					CryLogAlways(" *** Balance Board force Motion Sensing off during connection *** ");
					CryVR_WiimoteManager::SetMotionSensing(w_id,false);
				}

				if(CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == EXP_WII_BOARD && was_motion_sensing_active){
					CryLogAlways(" *** Balance Board set Motion Sensing previous state *** ");
					CryVR_WiimoteManager::SetMotionSensing(w_id,true);
					was_motion_sensing_active = false;
					CryVR_WiimoteManager::Status(CryVR_WiimoteManager::GetWiimotes(w_id));
				}
				
				
				/* fait planter la balance board ! */

				//if(CryVR_WiimoteManager::found >w_id && w_id>=0 && CryVR_WiimoteManager::wiimotes[0]->exp.type == EXP_WII_BOARD) {
				while (wiiuse_poll(CryVR_WiimoteManager::GetWiimotes(), MAX_WIIMOTES)) {
					CryLogAlways("Event in loop");
					if(CryVR_WiimoteManager::GetWiimotes(w_id)->event == WIIUSE_EVENT) 
						handle_event(CryVR_WiimoteManager::GetWiimotes(w_id));
				}
			}
		
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteBalanceBoard(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


void handle_event(struct wiimote_t* wm) {
	if(IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, true);
	if(IS_RELEASED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	
	struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;



	ActivateOutput(&m_actInfo, BALANCE_BOARD_LEFT, Vec3(wb->tl,wb->bl,0));
	ActivateOutput(&m_actInfo, BALANCE_BOARD_RIGHT, Vec3(wb->tr,wb->br,0));
	
}

};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteBalanceBoard",  CryVR_WiimoteBalanceBoard);

