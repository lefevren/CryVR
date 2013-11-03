/* Wii Balance Board node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


#include "Wiimote/CryVR_WiimoteManager.h"
#include "wiiuse/wiiuse.h"

class CryVR_WiimoteBalanceBoard : public CFlowBaseNode<eNCT_Instanced>
{
public:

	SActivationInfo m_actInfo;
	int CryVR_WiimoteID;
	//imote** wiimotes;

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
		switch (event)
		{
		case eFE_Initialize: 
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{	
				CryVR_WiimoteManager::SetLeds(0,GetPortInt(pActInfo, 1));
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				
			}
		
		case eFE_Update:
			{
				int w_id = GetPortInt(pActInfo, 1);
				//CryLogAlways("expansion : %i",CryVR_WiimoteManager::wiimotes[w_id]->exp.type);
				if(CryVR_WiimoteManager::wiimotes[w_id]->exp.type == 0){
					CryVR_WiimoteManager::Init();
				}
				
				
				
				if(CryVR_WiimoteManager::found >w_id && w_id>=0 && CryVR_WiimoteManager::wiimotes[w_id]->exp.type == EXP_WII_BOARD) {
					while (wiiuse_poll(CryVR_WiimoteManager::wiimotes, MAX_WIIMOTES)) {
						if(CryVR_WiimoteManager::wiimotes[w_id]->event == WIIUSE_EVENT) handle_event(CryVR_WiimoteManager::wiimotes[w_id]);
					}
				}
					
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteBalanceBoard(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}




	/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (wm->exp.type == EXP_WII_BOARD){
		
		struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
		ActivateOutput(&m_actInfo, BALANCE_BOARD_LEFT, Vec3(wb->tl,wb->bl,0));
		ActivateOutput(&m_actInfo, BALANCE_BOARD_RIGHT, Vec3(wb->tr,wb->br,0));
	}
}

/*
void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {}
void handle_ctrl_status(struct wiimote_t* wm) {}
void handle_disconnect(wiimote* wm) {}
void test(struct wiimote_t* wm, byte* data, unsigned short len) {}
*/
};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteBalanceBoard",  CryVR_WiimoteBalanceBoard);

