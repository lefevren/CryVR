/* VRPN tracker node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "vrpn/vrpn_Tracker.h"
#include "vrpn/vrpn_Button.h"
#include "vrpn/vrpn_Analog.h"

void VRPN_CALLBACK handle_tracker_generic(void* userData, const vrpn_TRACKERCB t ){*(vrpn_TRACKERCB*)userData=t;}	

		
class CFlowNode_VRPNTracker : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		ACTIVE,
		IP_SERVER,
		NAME_TRACKER,
	};

	enum EOutputPorts
	{
		EOP_VPosition,
		EOP_VRotation,
		EOP_WRotation,
		EOP_VDirection,
	};

public:
	
	vrpn_Tracker_Remote * vrpnAnalog;
	vrpn_TRACKERCB * test_analog;
	bool active;

	/* Methods */
	CFlowNode_VRPNTracker(SActivationInfo *pActInfo){}
	virtual ~CFlowNode_VRPNTracker(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowNode_VRPNTracker(pActInfo);}
	

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate",true, _HELP("Activation")),
			InputPortConfig<int>( "VRPN_SERVER", _HELP("VRPN_SERVER" ), _HELP("VRPN_SERVER"), _UICONFIG("enum_int:Server0=0,Server1=1,Server2=2,Server3=3,Server4=4,Server5=5,Server6=6,Server7=7,Server8=8,Server9=9")),
			InputPortConfig<int>( "TRACKER_NAME", _HELP("TRACKER_NAME" ), _HELP("TRACKER_NAME"), _UICONFIG("enum_int:Tracker0=0,Tracker1=1,Tracker2=2,Tracker3=3,Tracker4=4,Tracker5=5,Tracker6=6,Tracker7=7,Tracker8=8,Tracker9=9")),
			{0},
		};

		
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("PositionTracker", _HELP("Tracker position")),
			OutputPortConfig<Vec3>("RotationTracker_X_Y_Z", _HELP("Tracker Quaternion xyz")),
			OutputPortConfig<float>("RotationTracker_W", _HELP("Tracker Quaternion w")),
			OutputPortConfig<Vec3>("DirectionTracker_X_Y_Z", _HELP("Tracker direction")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Retrieve values form vicon tracker through VRPN");
		config.SetCategory(EFLN_APPROVED);
	}


	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{

		string serverstring;
		switch (event)
		{
		case eFE_Initialize:
			{
				int input_server = GetPortInt(pActInfo, 1);
				int input_tracker = GetPortInt(pActInfo, 2);

				char * input_server_s = (char*)malloc(256*sizeof(char)) ;
				sprintf(input_server_s,"CryVR_vrpn_server_%i",input_server);
				char * input_tracker_s = (char*)malloc(256*sizeof(char)) ;
				sprintf(input_tracker_s,"CryVR_vrpn_tracker_%i",input_tracker);
				
				string vrpn_server= gEnv->pSystem->GetIConsole()->GetCVar(input_server_s/*+GetPortInt(pActInfo, 0)*/)->GetString();
				string tracker_name = gEnv->pSystem->GetIConsole()->GetCVar(input_tracker_s/*+GetPortInt(pActInfo, 1)*/)->GetString();
				
				serverstring = tracker_name+"@"+vrpn_server;
				
				test_analog = (vrpn_TRACKERCB *) malloc(sizeof(vrpn_TRACKERCB)*2);
				vrpnAnalog = new vrpn_Tracker_Remote(serverstring);
				vrpnAnalog->register_change_handler(test_analog,handle_tracker_generic);

				active = false;
				break;
			}
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
				if(!active) return ;

				if (vrpnAnalog){
					vrpnAnalog->mainloop();
					Vec3 position;
					position.x = (float)test_analog->pos[0];
					position.y = (float)test_analog->pos[1];
					position.z = (float)test_analog->pos[2];

					Vec3 out;
					out.x = (float)test_analog->quat[0];
					out.y= (float)test_analog->quat[1];
					out.z = (float)test_analog->quat[2];
					float outw = (float)test_analog->quat[3];

					Quat quat;
					quat.v.x = (float)test_analog->quat[0];
					quat.v.y=(float)test_analog->quat[1];
					quat.v.z = (float)test_analog->quat[2];
					quat.w = (float)test_analog->quat[3];

					Vec3 dir = quat.GetColumn1();

					if (!quat.IsValid()) {quat.CreateIdentity();}
					if (!out.IsValid()){out.zero();}

					ActivateOutput(pActInfo, EOP_VPosition,position);
					ActivateOutput(pActInfo, EOP_VRotation,out);
					ActivateOutput(pActInfo, EOP_WRotation,outw);
					ActivateOutput(pActInfo, EOP_VDirection,dir);
				}
			}
		}
	}
};

REGISTER_FLOW_NODE("CryVR:VRPN:Tracker", CFlowNode_VRPNTracker);
