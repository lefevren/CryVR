/* Wiimote node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Wiiuse/wiiuse.h" 



#define MAX_WIIMOTES				4

class CFlowWiimoteControler : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_IsServer = 0,
	};

	enum EOutputPorts
	{
		WIIMOTE_STATUS, //String
		WIIMOTE_A, //Pressed
		WIIMOTE_B, //Pressed
		WIIMOTE_UP, //Pressed
		WIIMOTE_DOWN, //Pressed
		WIIMOTE_LEFT, //Pressed
		WIIMOTE_RIGHT, //Pressed
		WIIMOTE_MINUS, //Pressed
		WIIMOTE_PLUS, //Pressed
		WIIMOTE_ONE, //Pressed
		WIIMOTE_TWO, //Pressed
		WIIMOTE_HOME, //Pressed
		WIIMOTE_RPY, //Vec3
		WIIMOTE_IR, //Vec3
		
		NUNCHUK_C,
		NUNCHUK_Z,
		NUNCHUK_RPY,
		NUNCHUK_JOYSTICK, //Vec3 z=0

	};

public:
	wiimote** wiimotes;// = wiiuse_init(2); 
	
	////////////////////////////////////////////////////
	CFlowWiimoteControler(SActivationInfo *pActInfo){
		
	}
	
	////////////////////////////////////////////////////
	virtual ~CFlowWiimoteControler(void){
		
	}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Connexion à un server VRPN")),
			{ 0 },
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("")),
			
			OutputPortConfig<bool>("A", _HELP("")),
			OutputPortConfig<bool>("B", _HELP("")),
			OutputPortConfig<bool>("UP", _HELP("")),
			OutputPortConfig<bool>("DOWN", _HELP("")),
			OutputPortConfig<bool>("LEFT", _HELP("")),
			OutputPortConfig<bool>("RIGHT", _HELP("")),
			OutputPortConfig<bool>("MINUS", _HELP("")),
			OutputPortConfig<bool>("PLUS", _HELP("")),
			OutputPortConfig<bool>("ONE", _HELP("")),
			OutputPortConfig<bool>("TWO", _HELP("")),
			OutputPortConfig<bool>("HOME", _HELP("")),
			
			OutputPortConfig<Vec3>("WIIMOTE_RPY", _HELP("Wiimote Roll pitch yaw vec3")),
			OutputPortConfig<Vec3>("WIIMOTE_IR", _HELP("Wiimote IR vec3")),
			

			OutputPortConfig<bool>("C", _HELP("")),
			OutputPortConfig<bool>("Z", _HELP("")),
			OutputPortConfig<Vec3>("NUNCHUK_RPY", _HELP("Nunchuk Roll pitch yaw vec3")),
			OutputPortConfig<Vec3>("NUNCHUK_JOY", _HELP("Nunchuk Joystick vec3")),
			{ 0 },

		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that retrieve values from wiimote");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		int found =-1;
		int connected;
		
		
		switch (event)
		{
		case eFE_Initialize:{
			wiimotes = wiiuse_init(1); 
			found = wiiuse_find(wiimotes, 1, 5);
			if (found!=0) handle_ctrl_status(wiimotes[0]);

			connected = wiiuse_connect(wiimotes, 1);
			if (connected) CryLogAlways("Connected to %i wiimotes (of %i found).\n", connected, found);
			else CryLogAlways("Failed to connect to any wiimote.\n");
			wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
			
			wiiuse_motion_sensing(wiimotes[0], 1);
			wiiuse_set_ir(wiimotes[0], 1);

			//trop rapide !
			
		}
		case eFE_Activate:
			{	
				
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
			break;
		
		case eFE_Update:
			{
				//int connected = wiiuse_connect(wiimotes, 1); 
				wiiuse_poll(wiimotes, 1);
				wiimote* wm =  wiimotes[0];
				//Ne change rien ! juste qu'on a un retour d'IR
				//handle_ctrl_status(wiimotes[0]);
				CryLogAlways("OBSOLETE USE WIIMOTE CONTROLER TEST :      %i",wm->exp.type);

				
				
				// A commenter
				switch (wiimotes[0]->event) {
					case WIIUSE_EVENT:
						/* a generic event occured */
						handle_event(wiimotes[0]);
						break;

					case WIIUSE_STATUS:
						/* a status event occured */
						handle_ctrl_status(wiimotes[0]);
						break;

					case WIIUSE_DISCONNECT:
					case WIIUSE_UNEXPECTED_DISCONNECT:
						/* the wiimote disconnected */
						handle_disconnect(wiimotes[0]);
						break;

					case WIIUSE_READ_DATA:
						/*
						 *	Data we requested to read was returned.
						 *	Take a look at wiimotes[i]->read_req
						 *	for the data.
						 */
						break;

					case WIIUSE_NUNCHUK_INSERTED:
						/*
						 *	a nunchuk was inserted
						 *	This is a good place to set any nunchuk specific
						 *	threshold values.  By default they are the same
						 *	as the wiimote.
						 */
						//struct nunchuk_t* nc = (nunchuk_t*)&wiimotes[0]->exp.nunchuk;
						 wiiuse_set_nunchuk_orient_threshold(wiimotes[0], 90.0f);
						 wiiuse_set_nunchuk_accel_threshold(wiimotes[0], 100);
						CryLogAlways("Nunchuk inserted.\n");
						break;

					case WIIUSE_CLASSIC_CTRL_INSERTED:
						CryLogAlways("Classic controller inserted.\n");
						break;

					case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
						/* some expansion was inserted */
						handle_ctrl_status(wiimotes[0]);
						//printf("Guitar Hero 3 controller inserted.\n");
						break;

					case WIIUSE_NUNCHUK_REMOVED:
					case WIIUSE_CLASSIC_CTRL_REMOVED:
					case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
						/* some expansion was removed */
						handle_ctrl_status(wiimotes[0]);
						//printf("An expansion was removed.\n");
						break;

					default:
						break;
				}














				
					//if (wiiuse_poll(wiimotes, 1)) {
						//int i = 0;
				
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_A))			ActivateOutput(pActInfo, WIIMOTE_A,true); else ActivateOutput(pActInfo, WIIMOTE_A,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_B))			ActivateOutput(pActInfo, WIIMOTE_B,true); else ActivateOutput(pActInfo, WIIMOTE_B,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_UP))			ActivateOutput(pActInfo, WIIMOTE_UP,true); else ActivateOutput(pActInfo, WIIMOTE_UP,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_DOWN))		ActivateOutput(pActInfo, WIIMOTE_DOWN,true); else ActivateOutput(pActInfo, WIIMOTE_DOWN,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_LEFT))		ActivateOutput(pActInfo, WIIMOTE_LEFT,true); else ActivateOutput(pActInfo, WIIMOTE_LEFT,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_RIGHT))		ActivateOutput(pActInfo, WIIMOTE_RIGHT,true); else ActivateOutput(pActInfo, WIIMOTE_RIGHT,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_MINUS))		ActivateOutput(pActInfo, WIIMOTE_MINUS,true); else ActivateOutput(pActInfo, WIIMOTE_MINUS,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_PLUS))		ActivateOutput(pActInfo, WIIMOTE_PLUS,true); else ActivateOutput(pActInfo, WIIMOTE_PLUS,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_ONE))		ActivateOutput(pActInfo, WIIMOTE_ONE,true); else ActivateOutput(pActInfo, WIIMOTE_ONE,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_TWO))		ActivateOutput(pActInfo, WIIMOTE_TWO,true); else ActivateOutput(pActInfo, WIIMOTE_TWO,false);
						if (IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_HOME))		ActivateOutput(pActInfo, WIIMOTE_HOME,true); else ActivateOutput(pActInfo, WIIMOTE_HOME,false);

						if (WIIUSE_USING_ACC(wiimotes[0])) {
							Vec3 output = Vec3(wiimotes[0]->orient.roll,wiimotes[0]->orient.pitch,wiimotes[0]->orient.yaw);
							ActivateOutput(pActInfo, WIIMOTE_RPY,output);
						}
						if (WIIUSE_USING_IR(wiimotes[0])) {
							Vec3 output = Vec3((float)wiimotes[0]->ir.x,(float)wiimotes[0]->ir.y,(float)wiimotes[0]->ir.z);
							ActivateOutput(pActInfo, WIIMOTE_IR,output);
						}
						/* nunchuk */
						//Toujours faux !
						
						if (wm->exp.type == EXP_NUNCHUK) {
							
							//nunchuk_t* pt =  wm->exp.nunchuk;
						//REssort 0 a chaque fois !!!!
							struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;
							CryLogAlways("nun gravity x EXPANSION : %f",nc->gforce.x);
							if (IS_PRESSED(nc, NUNCHUK_BUTTON_C)) ActivateOutput(pActInfo, NUNCHUK_C,true); else ActivateOutput(pActInfo, NUNCHUK_C,false);
							if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z)) ActivateOutput(pActInfo, NUNCHUK_Z,true); else ActivateOutput(pActInfo, NUNCHUK_Z,false);
							
							Vec3 output = Vec3(nc->orient.a_roll,nc->orient.a_pitch,nc->orient.yaw);
							ActivateOutput(pActInfo, NUNCHUK_RPY,output);
							
							Vec3 output2 = Vec3(nc->js.ang,nc->js.mag,0);
							ActivateOutput(pActInfo, NUNCHUK_JOYSTICK,output2);
						} 


						/*
						switch (wiimotes[0]->event) {
							//case WIIUSE_EVENT:
								//{
								//}
								//break;

							case WIIUSE_STATUS:
								handle_ctrl_status(wiimotes[0]);
								break;

							case WIIUSE_DISCONNECT:
							case WIIUSE_UNEXPECTED_DISCONNECT:
								handle_disconnect(wiimotes[0]);
								break;
								
							default:
								break;
						}
						*/
					//}
				//}
			}
				
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteControler(pActInfo);}
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
	CryLogAlways("\n\n--- EVENT [id %i] ---\n", wm->unid);

	/* if a button is pressed, report it */
	
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))			{CryLogAlways("A pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))			{CryLogAlways("B pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))			{CryLogAlways("UP pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))		{CryLogAlways("DOWN pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))		{CryLogAlways("LEFT pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))		{CryLogAlways("RIGHT pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))		{CryLogAlways("MINUS pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))		{CryLogAlways("PLUS pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))			{CryLogAlways("ONE pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))			{CryLogAlways("TWO pressed\n");}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))		{CryLogAlways("HOME pressed\n");}
	
	/*
	 *	Pressing minus will tell the wiimote we are no longer interested in movement.
	 *	This is useful because it saves battery power.
	 */
	/*
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {wiiuse_motion_sensing(wm, 0);}
	*/
	/*
	 *	Pressing plus will tell the wiimote we are interested in movement.
	 */
	/*
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {wiiuse_motion_sensing(wm, 1);}
	*/
	/*
	 *	Pressing B will toggle the rumble
	 *
	 *	if B is pressed but is not held, toggle the rumble
	 */
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B)) {wiiuse_toggle_rumble(wm);}
	/*
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP)) {wiiuse_set_ir(wm, 1);}
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) {wiiuse_set_ir(wm, 0);}
	*/
	/*
	 * Motion+ support
	 */
	
	/* if the accelerometer is turned on then print angles */
	/*
	if (WIIUSE_USING_ACC(wm)) {
		CryLogAlways("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
		CryLogAlways("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
		CryLogAlways("wiimote yaw   = %f\n", wm->orient.yaw);
	}
	*/
	/*
	 *	If IR tracking is enabled then print the coordinates
	 *	on the virtual screen that the wiimote is pointing to.
	 *
	 *	Also make sure that we see at least 1 dot.
	 */

	if (WIIUSE_USING_IR(wm)) {
		int i = 0;

		/* go through each of the 4 possible IR sources */
		for (; i < 4; ++i) {
			/* check if the source is visible */
			if (wm->ir.dot[i].visible) {
				CryLogAlways("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
			}
		}

		CryLogAlways("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
		CryLogAlways("IR z distance: %f\n", wm->ir.z);
	}

	/* show events specific to supported expansions */
	
	if (wm->exp.type == EXP_NUNCHUK) {
		struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

		if (IS_PRESSED(nc, NUNCHUK_BUTTON_C)) {
			CryLogAlways("Nunchuk: C pressed\n");
		}
		if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z)) {
			CryLogAlways("Nunchuk: Z pressed\n");
		}

		CryLogAlways("nunchuk roll  = %f\n", nc->orient.roll);
		CryLogAlways("nunchuk pitch = %f\n", nc->orient.pitch);
		CryLogAlways("nunchuk yaw   = %f\n", nc->orient.yaw);

		CryLogAlways("nunchuk joystick angle:     %f\n", nc->js.ang);
		CryLogAlways("nunchuk joystick magnitude: %f\n", nc->js.mag);
	} 
	
	/*
	if (wm->exp.type == EXP_CLASSIC) {
		
		struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;

		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZL))			{CryLogAlways("Classic: ZL pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_B))			{CryLogAlways("Classic: B pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_Y))			{CryLogAlways("Classic: Y pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_A))			{CryLogAlways("Classic: A pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_X))			{CryLogAlways("Classic: X pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZR))			{CryLogAlways("Classic: ZR pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_LEFT))		{CryLogAlways("Classic: LEFT pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP))			{CryLogAlways("Classic: UP pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_RIGHT))		{CryLogAlways("Classic: RIGHT pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_DOWN))		{CryLogAlways("Classic: DOWN pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_L))		{CryLogAlways("Classic: FULL L pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_MINUS))		{CryLogAlways("Classic: MINUS pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_HOME))		{CryLogAlways("Classic: HOME pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_PLUS))		{CryLogAlways("Classic: PLUS pressed\n");}
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_R))		{CryLogAlways("Classic: FULL R pressed\n");}

		CryLogAlways("classic L button pressed:         %f\n", cc->l_shoulder);
		CryLogAlways("classic R button pressed:         %f\n", cc->r_shoulder);
		CryLogAlways("classic left joystick angle:      %f\n", cc->ljs.ang);
		CryLogAlways("classic left joystick magnitude:  %f\n", cc->ljs.mag);
		CryLogAlways("classic right joystick angle:     %f\n", cc->rjs.ang);
		CryLogAlways("classic right joystick magnitude: %f\n", cc->rjs.mag);
	} 
	*/
}

/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	This function is called automatically by the wiiuse library when
 *	the wiimote has returned the full data requested by a previous
 *	call to wiiuse_read_data().
 *
 *	You can read data on the wiimote, such as Mii data, if
 *	you know the offset address and the length.
 *
 *	The \a data pointer was specified on the call to wiiuse_read_data().
 *	At the time of this function being called, it is not safe to deallocate
 *	this buffer.
 */
void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
	int i = 0;

	CryLogAlways("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	CryLogAlways("finished read of size %i\n", len);
	for (; i < len; ++i) {
		if (!(i % 16)) {
			CryLogAlways("AA\n");
		}
		CryLogAlways("%x ", data[i]);
	}
	CryLogAlways("AAAAA\n\n");
}



/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiiuse_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t* wm) {
	CryLogAlways("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	CryLogAlways("attachment:      %i\n", wm->exp.type);
	CryLogAlways("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	CryLogAlways("ir:              %i\n", WIIUSE_USING_IR(wm));
	CryLogAlways("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	CryLogAlways("battery:         %f %%\n", wm->battery_level);
}

/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
	CryLogAlways("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}


void test(struct wiimote_t* wm, byte* data, unsigned short len) {
	CryLogAlways("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
}

};

REGISTER_FLOW_NODE("CryVR:Controlers:Wii:WiimoteControler", CFlowWiimoteControler);

