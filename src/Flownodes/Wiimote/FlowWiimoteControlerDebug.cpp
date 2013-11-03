#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

#include "wiiuse.h" 



#define MAX_WIIMOTES				4


class CFlowWiimoteControlerDebug : public CFlowBaseNode<eNCT_Instanced>
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
	
	////////////////////////////////////////////////////
	CFlowWiimoteControlerDebug(SActivationInfo *pActInfo){
		//wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
	}
	
	////////////////////////////////////////////////////
	virtual ~CFlowWiimoteControlerDebug(void){
		//wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
		CryLogAlways("Suppression");
		/*
		if (wiimotes){
			if (wiimotes[0]){
				wiiuse_cleanup(wiimotes, 1);
			}
		}
		*/
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
			//Sleep(5000);
			break;
		case eFE_Activate:
			{	
				//Sleep(3000); // <-- Dernière chose ajoutée
				wiimotes = wiiuse_init(MAX_WIIMOTES); 
				found = wiiuse_find(wiimotes,  MAX_WIIMOTES, 5);
				//if (found!=0) 
					//handle_ctrl_status(wiimotes[0]);

				connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
				if (connected) {

					CryLogAlways("Connected to %i wiimotes (of %i found).\n", connected, found);
				}
				else CryLogAlways("Failed to connect to any wiimote.\n");
				wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_2);
			
				wiiuse_motion_sensing(wiimotes[0], 1);
				//wiiuse_set_ir(wiimotes[0], 1);
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				//break; // <-- Dernière chose commentée !
			}
		
		case eFE_Update:
			{
				//CryLogAlways("Update !");
				if(GetPortBool(pActInfo, 0)){
					wiimotes = wiiuse_init(MAX_WIIMOTES); 
					connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
				}
				
		
				if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
					/*
					 *	This happens if something happened on any wiimote.
					 *	So go through each one and check if anything happened.
					 */
					int i = 0;
					for (; i < MAX_WIIMOTES; ++i) {
						CryLogAlways("attachment:  wiimote :%i  attachement : %i",i,wiimotes[i]->exp.type);

						switch (wiimotes[i]->event) {
							case WIIUSE_EVENT:
								/* a generic event occured */
								CryLogAlways("Generic event");
								handle_event(wiimotes[i]);
								break;
							case WIIUSE_CONNECT:
								CryLogAlways("Connexion");
								break;
							case WIIUSE_STATUS:
								/* a status event occured */
								CryLogAlways("Demande de status");
								handle_ctrl_status(wiimotes[i]);
								break;

							case WIIUSE_DISCONNECT:
							case WIIUSE_UNEXPECTED_DISCONNECT:
								/* the wiimote disconnected */
								CryLogAlways("Deconnexion");
								handle_disconnect(wiimotes[i]);
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
								 //wiiuse_set_nunchuk_orient_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f);
								 //wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100);
								handle_ctrl_status(wiimotes[i]);
								printf("Nunchuk inserted.\n");
								break;

							case WIIUSE_CLASSIC_CTRL_INSERTED:
								handle_ctrl_status(wiimotes[i]);
								printf("Classic controller inserted.\n");
								break;

							case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
								/* some expansion was inserted */
								handle_ctrl_status(wiimotes[i]);
								printf("Guitar Hero 3 controller inserted.\n");
								break;

							case WIIUSE_NUNCHUK_REMOVED:
							case WIIUSE_CLASSIC_CTRL_REMOVED:
							case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
								/* some expansion was removed */
								handle_ctrl_status(wiimotes[i]);
								printf("An expansion was removed.\n");
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowWiimoteControlerDebug(pActInfo);}
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
	
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {wiiuse_motion_sensing(wm, 0);}
	
	/*
	 *	Pressing plus will tell the wiimote we are interested in movement.
	 */
	
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {wiiuse_motion_sensing(wm, 1);}
	
	/*
	 *	Pressing B will toggle the rumble
	 *
	 *	if B is pressed but is not held, toggle the rumble
	 */
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B)) {wiiuse_toggle_rumble(wm);}
	
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP)) {wiiuse_set_ir(wm, 1);}
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) {wiiuse_set_ir(wm, 0);}
	
	/*
	 * Motion+ support
	 */
	
	/* if the accelerometer is turned on then print angles */
	
	if (WIIUSE_USING_ACC(wm)) {
		CryLogAlways("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
		CryLogAlways("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
		CryLogAlways("wiimote yaw   = %f\n", wm->orient.yaw);
	}
	
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

REGISTER_FLOW_NODE("CryVR:Controlers:WiimoteControlerDebug", CFlowWiimoteControlerDebug);

