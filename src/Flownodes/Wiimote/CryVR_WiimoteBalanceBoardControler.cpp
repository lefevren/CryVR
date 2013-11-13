/* Wii Balance Board node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiimote/CryVR_WiimoteManager.h"
#include "wiiuse/wiiuse.h"

class CryVR_WiimoteBalanceBoardControler : public CFlowBaseNode<eNCT_Instanced>
{
public:

	bool active;
	bool was_motion_sensing_active;
	bool balance_move;
	bool use_calibration;
	
	int nb_values_calib;
	int max_calib;
	Vec3 balance_left_calib,balance_right_calib;


	SActivationInfo m_actInfo;
	int CryVR_WiimoteID;
	
	enum EInputPorts
	{
		EIP_Activater = 0,
		EIP_WiimoteID = 1,
		EIP_USE_CALIB = 2,
	};

	enum EOutputPorts
	{
		WIIMOTE_STATUS, //String
		WIIMOTE_A, //Pressed
		BALANCE_BOARD_LEFT,
		BALANCE_BOARD_RIGHT,
		BALANCE_BOARD_XY,
		BALANCE_BOARD_WEIGHT,
	};


	////////////////////////////////////////////////////
	 CryVR_WiimoteBalanceBoardControler(SActivationInfo *pActInfo) :
		m_actInfo()
	{
	}
	
	
	////////////////////////////////////////////////////
	virtual ~ CryVR_WiimoteBalanceBoardControler(void){}

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
			InputPortConfig<bool>("UseCalibration",true, _HELP("Use calibration (need balance board calibration before")),
			InputPortConfig<int>("CalibrationEvents",100, _HELP("Event minimum needed to calibrate the balance board")),
			InputPortConfig<int>("MaxEvents",3,_HELP("Maximum events by frame")),
			InputPortConfig<bool>("FPS_FREE",true, _HELP("Force Node to execute minimum events")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("")),
			OutputPortConfig<bool>("A", _HELP("")),
			OutputPortConfig<Vec3>("Balance_board_left", _HELP("Balance board left values")),
			OutputPortConfig<Vec3>("Balance_board_right", _HELP("Balance board right values")),
			OutputPortConfig<Vec3>("Balance_board_xy", _HELP("Balance board xy gravity point")),
			OutputPortConfig<float>("Balance_board_weight", _HELP("Calculated weight")),
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
			nb_values_calib = 0;
			balance_left_calib = Vec3(0,0,0);
			balance_right_calib = Vec3(0,0,0);
			m_actInfo = *pActInfo;
			return ;
		}

		if(event==eFE_Activate){
			active = GetPortBool(pActInfo, 0);
			nb_values_calib = 0;
			use_calibration = GetPortBool(pActInfo, 2);
			balance_left_calib = Vec3(0,0,0);
			balance_right_calib = Vec3(0,0,0);
			
			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			return ;
		}

		if(event== eFE_Update) 
			{
				balance_move = false;
				int i = 0;
				int w_id = GetPortInt(pActInfo, 1);
				if(!active || w_id < 0 || w_id >= CryVR_WiimoteManager::found) return ;

				
				
				if(CryVR_WiimoteManager::GetMotionSensing(w_id) && CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == 0){
					was_motion_sensing_active = true;
					CryLogAlways(" *** Balance Board force Motion Sensing off during connection *** ");
					CryVR_WiimoteManager::SetMotionSensing(w_id,false);
				}

				if(CryVR_WiimoteManager::GetWiimotes(w_id)->exp.type == EXP_WII_BOARD && was_motion_sensing_active){
					CryLogAlways(" *** Balance Board set Motion Sensing previous state *** ");
					//CryVR_WiimoteManager::SetMotionSensing(w_id,true);
					was_motion_sensing_active = false;
					CryVR_WiimoteManager::Status(CryVR_WiimoteManager::GetWiimotes(w_id));
				}
				
				
				/* fait planter la balance board ! */

				//if(CryVR_WiimoteManager::found >w_id && w_id>=0 && CryVR_WiimoteManager::wiimotes[0]->exp.type == EXP_WII_BOARD) {
				while (wiiuse_poll(CryVR_WiimoteManager::GetWiimotes(), MAX_WIIMOTES) && i<GetPortInt(pActInfo, 4)) 
					{
						i++;
						if(CryVR_WiimoteManager::GetWiimotes(w_id)->event == WIIUSE_EVENT) {
							if(use_calibration && nb_values_calib<GetPortInt(pActInfo, 3)) {
								handle_event_calib(CryVR_WiimoteManager::GetWiimotes(w_id));
								nb_values_calib ++ ;
							}
							else {
								if(GetPortBool(pActInfo, 5) && i>GetPortInt(pActInfo, 4) ) return ;
								handle_event(CryVR_WiimoteManager::GetWiimotes(w_id));
								//optimisation
								//return ;
							}
						}						
					}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new  CryVR_WiimoteBalanceBoardControler(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}



	
void BalanceBoardCalib(Vec3 left, Vec3 right){
	//Faire la moyenne
	balance_left_calib =  Vec3((balance_left_calib.x * nb_values_calib + left.x) / (nb_values_calib+1), (balance_left_calib.y * nb_values_calib+ left.y)  / (nb_values_calib+1),0); 
	balance_right_calib =  Vec3((balance_right_calib.x * nb_values_calib + left.x) / (nb_values_calib+1), (balance_right_calib.y * nb_values_calib+ left.y)  / (nb_values_calib+1),0); 
	//CryLogAlways("Balance : [%f,%f]", balance_left_calib.x,balance_left_calib.y);
	
	nb_values_calib ++;

}



void handle_event(struct wiimote_t* wm) {
	if(IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, true);
	if(IS_RELEASED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	

	if(!balance_move) {
		struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
		
		Vec3 calib_left = Vec3(0,0,0);
		Vec3 calib_right = Vec3(0,0,0);

		if(use_calibration) {
			calib_left = balance_left_calib;
			calib_right = balance_right_calib;

		}

		Vec3 out_left = Vec3(wb->tl-calib_left.x,wb->bl-calib_left.y,0);
		Vec3 out_right = Vec3(wb->tr-calib_right.x,wb->br-calib_right.y,0);

		ActivateOutput(&m_actInfo, BALANCE_BOARD_LEFT, out_left);
		ActivateOutput(&m_actInfo, BALANCE_BOARD_RIGHT, out_right);
		
		/*
			 * Board orientation:
			 * +---------------------+
			 * | tl               tr |
			 * |        y            |
			 * |        ^            |
			 * |        |            |
			 * |        +----> x     |
			 * | bl               br |
			 * +---------vvv---------+
			 *           Button-A (power)
			 */


		float poids = out_right.x + out_right.y + out_left.y + out_left.x;
		//Fonctionne
		float percent_tl = (wb->tl-calib_left.x) / poids;
		float percent_tr =( wb->tr-calib_right.x) / poids;
		float percent_bl =( wb->bl-calib_left.y) / poids;
		float percent_br =( wb->br-calib_right.y) / poids;


		Vec3 output = Vec3(0,0,0);
		
		output.x = percent_tr-percent_tl+percent_br-percent_bl;
		output.y = percent_tr+percent_tl-percent_br-percent_bl;
		
		
		ActivateOutput(&m_actInfo, BALANCE_BOARD_XY, output);
		ActivateOutput(&m_actInfo, BALANCE_BOARD_WEIGHT, poids);


		balance_move = true;

	}
}


void handle_event_calib(struct wiimote_t* wm) {
		struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
		BalanceBoardCalib(Vec3(wb->tl,wb->bl,0),Vec3(wb->tr,wb->br,0));
	}
};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteBalanceBoardControler",  CryVR_WiimoteBalanceBoardControler);

