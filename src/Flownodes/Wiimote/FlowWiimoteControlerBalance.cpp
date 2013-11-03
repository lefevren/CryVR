/* Test d'optimisation et de rapidité */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "wiiuse.h" 



#define MAX_WIIMOTES				1


class CFlowWiimoteControlerBalance : public CFlowBaseNode<eNCT_Instanced>
{

	int found;
	int connected;

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
	SActivationInfo m_actInfo;

	////////////////////////////////////////////////////
	CFlowWiimoteControlerBalance(SActivationInfo *pActInfo) :
		m_actInfo()
	{
	}
	
	
	////////////////////////////////////////////////////
	virtual ~CFlowWiimoteControlerBalance(void){}

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
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG node that sets up a CAVE environment");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		
		
		switch (event)
		{
		case eFE_Initialize: 
			m_actInfo = *pActInfo;
			break;
		case eFE_Activate:
			{	
				CryLogAlways("Activate");
				wiimotes = wiiuse_init(MAX_WIIMOTES); 
				found = wiiuse_find(wiimotes,  MAX_WIIMOTES, 5);
				connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
				wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_3);
				wiiuse_motion_sensing(wiimotes[0], 1);
				
				//wiiuse_set_accel_threshold(wiimotes[0],1);
				
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				
			}
		
		case eFE_Update:
			{
				//Ici empilement des données !
				//if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
				while (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {	
					int i = 0;
					for (; i < MAX_WIIMOTES; ++i) {
						switch (wiimotes[i]->event) {
							case WIIUSE_EVENT:
								handle_event(wiimotes[i]);
								break;
							case WIIUSE_CONNECT:
								break;
							case WIIUSE_STATUS:
								handle_ctrl_status(wiimotes[i]);
								break;
							case WIIUSE_DISCONNECT:
							case WIIUSE_UNEXPECTED_DISCONNECT:
								handle_disconnect(wiimotes[i]);
								break;

							case WIIUSE_READ_DATA:
								break;

							case WIIUSE_NUNCHUK_INSERTED:
								handle_ctrl_status(wiimotes[i]);
								break;

							case WIIUSE_CLASSIC_CTRL_INSERTED:
								handle_ctrl_status(wiimotes[i]);
								break;

							case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
								handle_ctrl_status(wiimotes[i]);
								break;

							case WIIUSE_NUNCHUK_REMOVED:
							case WIIUSE_CLASSIC_CTRL_REMOVED:
							case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
								handle_ctrl_status(wiimotes[i]);
								break;

							default:
								break;
						}
			
					}
				}
			}
			//catch(EXCEPINFO e){}
		}
		
		//}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteControlerBalance(pActInfo);}
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
	//CryLogAlways("\n\n--- EVENT [id %i] ---\n", wm->unid);
	
	/* if a button is pressed, report it */
	
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) ActivateOutput(&m_actInfo, WIIMOTE_A, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) ActivateOutput(&m_actInfo, WIIMOTE_B, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) ActivateOutput(&m_actInfo, WIIMOTE_DOWN, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)) ActivateOutput(&m_actInfo, WIIMOTE_LEFT, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)) ActivateOutput(&m_actInfo, WIIMOTE_RIGHT, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) ActivateOutput(&m_actInfo, WIIMOTE_MINUS, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) ActivateOutput(&m_actInfo, WIIMOTE_PLUS, true);// else ActivateOutput(&m_actInfo, WIIMOTE_A, false);
	/*
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
	*/
	/*
	 *	Pressing minus will tell the wiimote we are no longer interested in movement.
	 *	This is useful because it saves battery power.
	 */
	
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {wiiuse_motion_sensing(wm, 0);}
	
	/*
	 *	Pressing plus will tell the wiimote we are interested in movement.
	 */
	
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {wiiuse_motion_sensing(wm, 1);}
	
	/*
	 *	Pressing B will toggle the rumble
	 *
	 *	if B is pressed but is not held, toggle the rumble
	 */
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B)) {wiiuse_toggle_rumble(wm);}
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
	/*
	if (WIIUSE_USING_IR(wm)) {
		int i = 0;

		for (; i < 4; ++i) {
			if (wm->ir.dot[i].visible) {
				CryLogAlways("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
			}
		}

		CryLogAlways("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
		CryLogAlways("IR z distance: %f\n", wm->ir.z);
	}
	*/
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
	
	
	else if (wm->exp.type == EXP_CLASSIC) {
		
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
	else if (wm->exp.type == EXP_WII_BOARD){
		struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
		CryLogAlways("WiiBoard roll  = [%f]\n", wb->update_calib);
		
		CryLogAlways("WiiBoard roll  = %f [%f]\n", wb->bl, wb->br);
		CryLogAlways("WiiBoard pitch = %f [%f]\n", wb->tl,wb->tr);
	}
	
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

	//CryLogAlways("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	//CryLogAlways("finished read of size %i\n", len);
	for (; i < len; ++i) {
		if (!(i % 16)) {
			//CryLogAlways("AA\n");
		}
		//CryLogAlways("%x ", data[i]);
	}
	//CryLogAlways("AAAAA\n\n");
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
	/*
	CryLogAlways("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	CryLogAlways("attachment:      %i\n", wm->exp.type);
	CryLogAlways("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	CryLogAlways("ir:              %i\n", WIIUSE_USING_IR(wm));
	CryLogAlways("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	CryLogAlways("battery:         %f %%\n", wm->battery_level);
	*/
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
	//CryLogAlways("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}


void test(struct wiimote_t* wm, byte* data, unsigned short len) {
	//CryLogAlways("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
}

};

REGISTER_FLOW_NODE("CryVR:Controlers:WiimoteControlerBalance", CFlowWiimoteControlerBalance);

