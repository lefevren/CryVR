/* Wiimote Manager node - for licensing and copyright see license.txt */

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


void CryVR_WiimoteManager::Init(){
	wiimotes = wiiuse_init(MAX_WIIMOTES); 
	wiiuse_set_bluetooth_stack(wiimotes, MAX_WIIMOTES, WIIUSE_STACK_MS);

	found = wiiuse_find(wiimotes,  MAX_WIIMOTES, 5);
	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);

	CryLogAlways("Wiimotes found : %i",found);

	init = true;
}

void CryVR_WiimoteManager::Init(bool ir_pos, bool motion, int v_time, float v_angle, int v_threshold){
	Init();
	SetIrPosition(ir_pos);
	SetMotionSensing(motion);
	SetAccelThreshold(v_threshold, v_angle);								//10	:	events : <10
																			//1		:	events : >80
																			//5		:	events : ~40 max
																			//7		:	events : <10
																			//3		:	events : ~60

	SetTimeout(timeout);													//20,20	:	events : <10 , 0 si un bouton appuyé
																			//15,15 :	events : ~3
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
			InputPortConfig<bool>("Motion_Sensing",true, _HELP("Motion sensing state")),
			InputPortConfig<int>("Threshold", 1 ,_HELP("Motion sensing state")),
			InputPortConfig<float>("Angle", 0.5 ,_HELP("Motion sensing state")),
			InputPortConfig<int>("Timeout", 0.5 ,_HELP("Motion sensing state")),
			{0},
		};

		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<bool>("Status", _HELP("")),
			{0},
		};

		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Wiimote Manager");
		config.SetCategory(EFLN_APPROVED);
}


void CryVR_WiimoteManager::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo){
	
	if(event==eFE_Activate  && IsPortActive(pActInfo,0)) {
			Init(GetPortBool(pActInfo, 1),GetPortBool(pActInfo, 2),GetPortInt(pActInfo, 3),GetPortFloat(pActInfo, 4),GetPortInt(pActInfo, 5));
			ActivateOutput(pActInfo, 0, true);
	}
	
	
}


REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteManager", CryVR_WiimoteManager);