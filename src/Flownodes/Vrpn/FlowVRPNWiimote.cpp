/* VRPN Wiimote node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


#include "vrpn/vrpn_Tracker.h"
#include "vrpn/vrpn_Button.h"
#include "vrpn/vrpn_Analog.h"
#include "vrpn/vrpn_WiiMote.h"
#include "vrpn/vrpn_Tracker_RazerHydra.h"


const string VRPNSERVER = "tracker0@localhost";
	
void VRPN_CALLBACK handle_analog_wii(void* userData, const vrpn_ANALOGCB a ){*(vrpn_ANALOGCB*)userData=a;}
void VRPN_CALLBACK handle_button_change_wii( void* userData, const vrpn_BUTTONCB b ){*(vrpn_BUTTONCB*)userData = b;}



class CFlowNode_VRPNWiimote : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		ACTIVE,
		SERVER,
	};

	enum EOutputPorts
	{
		WII_BUTTON_00,
		WII_BUTTON_01,
		WII_BUTTON_02,
		WII_BUTTON_03,
		WII_BUTTON_04,
		WII_BUTTON_05,
		WII_BUTTON_06,
		WII_BUTTON_07,
		WII_BUTTON_08,
		WII_BUTTON_09,
		WII_BUTTON_10,
		WII_BUTTON_11,
		WII_BUTTON_12,
		WII_BUTTON_13,
		WII_BUTTON_14,
		
		WII_BUTTON_15,
		WII_BUTTON_16,
		WII_BUTTON_17,
		
		WII_NUN_INTENSITY,
		WII_NUN_ANGLE,
		
		WII_WIIMOTE_GRAVITY_X,
		WII_WIIMOTE_GRAVITY_Y,
		WII_WIIMOTE_GRAVITY_Z,
	};

public:
	
	bool active;
	vrpn_Analog_Remote * vrpn_analog_remote;
	vrpn_ANALOGCB * vrpn_analog;
	vrpn_Button_Remote * vrpn_buttons_remote;
	vrpn_BUTTONCB * vrpn_buttons;// = new vrpn_BUTTONCB[5];

	CFlowNode_VRPNWiimote(SActivationInfo *pActInfo){}
	virtual ~CFlowNode_VRPNWiimote(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<string>("VRPNServer",VRPNSERVER, _HELP("Connexion à un server VRPN")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<int>("Bouton_2", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_1", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Gachette", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_A", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_-", _HELP("Debug message from multiplayer setup")),


			OutputPortConfig<int>("Bouton_2", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_1", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_home", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Fleche_gauche", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Fleche_droite", _HELP("Debug message from multiplayer setup")),

			OutputPortConfig<int>("Fleche_bas", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Fleche_haut", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_XXX", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_XXX", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_XXX", _HELP("Debug message from multiplayer setup")),

			OutputPortConfig<int>("Bouton_XXX", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_Z", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<int>("Bouton_C", _HELP("Debug message from multiplayer setup")),


			OutputPortConfig<float>("Nunchuck intensity", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<float>("Nunchuck angle", _HELP("Debug message from multiplayer setup")),

			OutputPortConfig<float>("Gravity X", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<float>("Gravity Y", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<float>("Gravity Z", _HELP("Debug message from multiplayer setup")),
			
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
		

		int butt= 0;

		switch (event)
		{
		case eFE_Initialize:
			{
				vrpn_analog = (vrpn_ANALOGCB *) malloc(sizeof(vrpn_ANALOGCB)*2);
				vrpn_analog_remote = new vrpn_Analog_Remote(GetPortString(pActInfo, 1));
				vrpn_analog_remote->register_change_handler(vrpn_analog,handle_analog_wii);
			
				vrpn_buttons = (vrpn_BUTTONCB *) malloc(sizeof(vrpn_BUTTONCB)*2);
				vrpn_buttons_remote = new vrpn_Button_Remote(GetPortString(pActInfo, 1));
				vrpn_buttons_remote->register_change_handler(vrpn_buttons,handle_button_change_wii);
			}
			break;
		case eFE_Activate:
			{
				if ( IsPortActive( pActInfo, ACTIVE ) ){
					active = GetPortBool( pActInfo, ACTIVE);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				}
				
			}
			break;
		
		case eFE_Update:
			{	
				if (!active) return ;

				if (vrpn_analog_remote){
					vrpn_analog_remote->mainloop();
					float angleJoystick = (float)vrpn_analog->channel[19];
					float intensityJoystick = (float)vrpn_analog->channel[20];

					if (intensityJoystick != 0) {
						ActivateOutput(pActInfo, WII_NUN_INTENSITY,intensityJoystick);
						ActivateOutput(pActInfo, WII_NUN_ANGLE,angleJoystick);
					}

					float gravity_x = (float)vrpn_analog->channel[1];
					float gravity_y = (float)vrpn_analog->channel[2];
					float gravity_z = (float)vrpn_analog->channel[3];

					ActivateOutput(pActInfo, WII_WIIMOTE_GRAVITY_X,gravity_x);
					ActivateOutput(pActInfo, WII_WIIMOTE_GRAVITY_Y,gravity_y);
					ActivateOutput(pActInfo, WII_WIIMOTE_GRAVITY_Z,gravity_z);
					
				}	

				if(vrpn_buttons_remote){
					vrpn_buttons_remote->mainloop();
					ActivateOutput(pActInfo,(int)vrpn_buttons->button, (int)vrpn_buttons->state);
				}
			}
		}
	}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowNode_VRPNWiimote(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const {s->Add(*this);}
};


REGISTER_FLOW_NODE("CryVR:VRPN:VRPNWiiMote", CFlowNode_VRPNWiimote);
