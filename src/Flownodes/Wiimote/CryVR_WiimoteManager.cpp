/* Wiimote Manager node - for licensing and copyright see license.txt */

/* Todo : Aspect Ratio */


#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Wiimote/CryVR_WiimoteManager.h"


int CryVR_WiimoteManager::found				= 0;
int CryVR_WiimoteManager::connected			= 0;
bool CryVR_WiimoteManager::init				= false;
wiimote** CryVR_WiimoteManager::wiimotes	= 0;
int CryVR_WiimoteManager::timeout			= 15;
int CryVR_WiimoteManager::threshold			= 15;
float CryVR_WiimoteManager::angle_threshold	= 1;
int CryVR_WiimoteManager::bt = 0;

void CryVR_WiimoteManager::Init(){
	wiimotes = wiiuse_init(MAX_WIIMOTES); 
	SetBluetooth(bt);

	found = wiiuse_find(wiimotes,  MAX_WIIMOTES, 5);
	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);

	CryLogAlways("Wiimotes found : %i",found);

	init = true;
}

void CryVR_WiimoteManager::Init(bool ir_pos, bool motion, int v_time, float v_angle, int v_threshold, int bts, bool aspect, int irlevel){
	bt=bts;
	Init();
	SetIrPosition(ir_pos);
	SetAspectRatio(aspect);
	SetIRSensivity(irlevel);
	//SetMotionSensing(motion);
	SetMotionSensing(false);
	
	
	
	SetAccelThreshold(v_threshold, v_angle);								//10	:	events : <10
																			//1		:	events : >80
																			//5		:	events : ~40 max
																			//7		:	events : <10
																			//3		:	events : ~60

	SetTimeout(timeout);													//20,20	:	events : <10 , 0 si un bouton appuyé
	
	
}

bool CryVR_WiimoteManager::Stop() {
	if (init) {
		wiiuse_cleanup(wiimotes, found);
		return true;
	}
	return false;
}


bool CryVR_WiimoteManager::SetIRSensivity(int level){
	if (init) {
		int irlevel = 3;
		if(level>=0 && level <=5) irlevel = level; 
		for(int i=0;i<found;i++) wiiuse_set_ir_sensitivity(wiimotes[i],irlevel);
		return true;
	}
	return false;
}

bool CryVR_WiimoteManager::SetAspectRatio(bool aspect){
	if (init) {
		if(aspect) {
			for(int i=0;i<found;i++) wiiuse_set_aspect_ratio(wiimotes[i],WIIUSE_ASPECT_16_9);
		}
		else for(int i=0;i<found;i++) wiiuse_set_aspect_ratio(wiimotes[i],WIIUSE_ASPECT_4_3);
		return true;
	}
	return false;
}

void CryVR_WiimoteManager::SetBluetooth(int value){
	switch (value)
	{
	case 1 : {
				wiiuse_set_bluetooth_stack(wiimotes,found,WIIUSE_STACK_MS);
				break;
			 }
	case 2 : {
				wiiuse_set_bluetooth_stack(wiimotes,found,WIIUSE_STACK_BLUESOLEIL);
				break;
			 }	
	default: wiiuse_set_bluetooth_stack(wiimotes,found,WIIUSE_STACK_UNKNOWN);
	}
}

bool CryVR_WiimoteManager::SetTimeout(int val){
	if(init && found>0) {
		timeout = val;
		wiiuse_set_timeout(wiimotes, found, timeout, timeout);
		return true;
	}
	return false;
}

bool CryVR_WiimoteManager::SetAccelThreshold(int ii, float angle){

	threshold = ii;
	angle_threshold	= angle;
	
	if(init && found>0) {
		int i = 0;
		for (; i < MAX_WIIMOTES; ++i){
			wiiuse_set_accel_threshold(wiimotes[i],ii);
			wiiuse_set_nunchuk_accel_threshold(wiimotes[i],ii);
			wiiuse_set_orient_threshold(wiimotes[i],angle_threshold);
			wiiuse_set_nunchuk_orient_threshold(wiimotes[i],angle_threshold);
		}
		
		return true;
	}
	return false;
}

int CryVR_WiimoteManager::GetType(int id){
	if(init && id<found && id>=0) return wiimotes[id]->exp.type;
	return -1;
}	

bool CryVR_WiimoteManager::ToggleRumble(int id){
	if(init && id<found && id>=0) {
		wiiuse_toggle_rumble(wiimotes[id]);
		return true;
	}
	return false;
}	

bool CryVR_WiimoteManager::SetIr(int id, bool value){
	if(init && id<found && id>=0) {
		if(value) wiiuse_set_ir(wiimotes[id],1);
		else  wiiuse_set_ir(wiimotes[id],0);
		return true;
	}
	return false;
}


bool CryVR_WiimoteManager::SetIrPosition(bool above){
	if(init && found>0) {
		int i = 0;
		ir_position_t pos = WIIUSE_IR_ABOVE;
		if (!above) pos = WIIUSE_IR_BELOW;
		for (; i < MAX_WIIMOTES; ++i) wiiuse_set_ir_position(wiimotes[0],pos);
		return true;
	}
	return false;
}


bool CryVR_WiimoteManager::GetIr(int id){
	if(init && id<found && id>=0) {
		return WIIUSE_USING_IR(wiimotes[id]);
	}
	return false;
}


float CryVR_WiimoteManager::GetBatteryLevel(int id){
	if(init){
		if(id<found && id>=0) 
		{
			return (wiimotes[id]->battery_level);
		}
	}
	return -1;
}

bool CryVR_WiimoteManager::SetMotionSensing(int id,bool value){
	if(init){
		if(id<found && id>=0) 
		{
			if(value) wiiuse_motion_sensing(wiimotes[id], 1);
			else wiiuse_motion_sensing(wiimotes[id], 0);
			return true;
		}
	}
	return false;
}

bool CryVR_WiimoteManager::SetMotionSensing(bool value){
	if(init && found>0){
		int i = 0;
		int mot = 0;
		if(value) mot = 1;
		for (; i < MAX_WIIMOTES; ++i) wiiuse_motion_sensing(wiimotes[i], mot);
		return true;
	}
	return false;
}
	


bool CryVR_WiimoteManager::SetLeds(int id,int led){
	if(init) {
		switch (led) {
			case 1 : 
				{
					wiiuse_set_leds(wiimotes[id], WIIMOTE_LED_2); 
					break;
				}
			case 2 : 
				{
					wiiuse_set_leds(wiimotes[id], WIIMOTE_LED_3); 
					break;
				}
			case 3 : 
				{
					wiiuse_set_leds(wiimotes[id], WIIMOTE_LED_4); 
					break;
				}
			default : wiiuse_set_leds(wiimotes[id], WIIMOTE_LED_1);
		}
		return true;
	}
	else return false;
}

CryVR_WiimoteManager::CryVR_WiimoteManager(SActivationInfo *pActInfo){}
CryVR_WiimoteManager::~CryVR_WiimoteManager(void){}
void CryVR_WiimoteManager::Serialize(SActivationInfo *pActInfo, TSerialize ser){}


void CryVR_WiimoteManager::GetConfiguration(SFlowNodeConfig& config){
	static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Initialisation")),
			InputPortConfig<bool>("Ir_Above",true, _HELP("IR position (Above or below)")),
			//InputPortConfig<bool>("Motion_Sensing",true, _HELP("Motion sensing state")),
			InputPortConfig<int>("Threshold", 1 ,_HELP("Motion sensing state")),
			InputPortConfig<float>("Angle", 0.5 ,_HELP("Motion sensing state")),
			InputPortConfig<int>("Timeout", 20 ,_HELP("Motion sensing state")),
			InputPortConfig<int>("Bluetooth_stack", 0 ,_HELP("Set bluetooth stack [0, Auto] [1, MS] [2, BlueSoleil]")),
			InputPortConfig<bool>("Aspect_ratio",true, _HELP("Set Aspect Ratio : [false, 4/3] [true, 16/9]")),
			InputPortConfig<int>("IR_Sensivity", 3 ,_HELP("Set ir sensivity [0;5]")),
			{0},
		};

		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<bool>("Status", _HELP("")),
			{0},
		};

		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Wiimote_Manager");
		config.SetCategory(EFLN_APPROVED);
}


void CryVR_WiimoteManager::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo){
	
	
	if(event==eFE_Activate  && GetPortBool(pActInfo, 0)) {
		CryLogAlways("Evenement init wiimote");
		Init(GetPortBool(pActInfo, 1),GetPortBool(pActInfo, 1),GetPortInt(pActInfo, 2),GetPortFloat(pActInfo, 3),GetPortInt(pActInfo, 4),GetPortInt(pActInfo,5),GetPortBool(pActInfo,6),GetPortInt(pActInfo,7));
		//Sleep(1000);
		while (wiiuse_poll(wiimotes, CryVR_WiimoteManager::found)) {	
			CryLogAlways("Event initial");
			Status(wiimotes[0]);
		}

		ActivateOutput(pActInfo, 0, true);
	}


	
}


void CryVR_WiimoteManager::Status(struct wiimote_t* wm) {
	CryLogAlways("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);
	CryLogAlways("attachment:      %i\n", wm->exp.type);
	CryLogAlways("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	CryLogAlways("ir:              %i\n", WIIUSE_USING_IR(wm));
	CryLogAlways("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	CryLogAlways("battery:         %f %%\n", wm->battery_level);
}



void CryVR_WiimoteManager::Status(int id) {
	if(id<=0 && id>found) {
		struct wiimote_t* wm = wiimotes[id];

		CryLogAlways("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);
		CryLogAlways("attachment:      %i\n", wm->exp.type);
		CryLogAlways("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
		CryLogAlways("ir:              %i\n", WIIUSE_USING_IR(wm));
		CryLogAlways("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
		CryLogAlways("battery:         %f %%\n", wm->battery_level);
	}
}


REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteManager", CryVR_WiimoteManager);