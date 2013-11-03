#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


#include "vrpn_Tracker.h"
#include "vrpn_Button.h"
#include "vrpn_Analog.h"

//FlowAsymmetricCamera.cpp


const string VRPNSERVER = "tracker0@localhost";
	
void VRPN_CALLBACK handle_analog(void* userData, const vrpn_ANALOGCB a ){
	*(vrpn_ANALOGCB*)userData=a;
		//CryLogAlways("%s",temp1); 
		
		//for (int i=0;i<a.num_channel;i++){
			//CryLogAlways("%d", a.channel[i]);
		//}
	}
		
	
void VRPN_CALLBACK handle_button_change( void* userData, const vrpn_BUTTONCB b ){
	//*(vrpn_BUTTONCB*)userData = b;
	 static int buttonstate = 1;  
	 if (b.state != buttonstate) {    
		 printf("button #%d is in state %d\n", b.button, b.state);    
		 buttonstate = b.state;  
	 }
	*(int *)userData = buttonstate;
}


void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t )
	{
		*(vrpn_TRACKERCB*)userData=t;
		//cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," <<  t.pos[1] << "," << t.pos[2] << endl;
	}
		
class CFlowNode_VRPNTest : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_IsServer = 0,
	};

	enum EOutputPorts
	{
		EOP_Message,
		EOP_VPosition,
		EOP_VRotation,
		EOP_WRotation,
		EOP_VDirection,
	};

public:
	
	//float testValeur;
	vrpn_Tracker_Remote * vrpnAnalog;
	vrpn_TRACKERCB * test_analog;

	////////////////////////////////////////////////////
	CFlowNode_VRPNTest(SActivationInfo *pActInfo)
	{
	}
	
	////////////////////////////////////////////////////
	virtual ~CFlowNode_VRPNTest(void)
	{

	}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser)
	{

	}

	////////////////////////////////////////////////////
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
			OutputPortConfig<string>("Message", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<Vec3>("PositionTracker", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<Vec3>("RotationTracker_X_Y_Z", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<float>("RotationTracker_W", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<Vec3>("DirectionTracker_X_Y_Z", _HELP("Debug message from multiplayer setup")),
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
		
		void * chaineRow0 = (void*)malloc(40*sizeof(vrpn_float64)) ;
		char * chaine = (char*)malloc(1024*sizeof(vrpn_float64)) ;

		int butt= 0;

		switch (event)
		{
		case eFE_Initialize:{
			test_analog = (vrpn_TRACKERCB *) malloc(sizeof(vrpn_TRACKERCB)*2);
			//vrpnAnalog = new vrpn_Analog_Remote("Analog0@192.168.242.101");
			vrpnAnalog = new vrpn_Tracker_Remote(GetPortString(pActInfo, 0));
			vrpnAnalog->register_change_handler(test_analog,handle_tracker);
			
			string temp1 = "Initialisation";
			CryLogAlways("%s", temp1);
			
		}
		case eFE_Activate:
			{
				if (vrpnAnalog){
					vrpnAnalog->mainloop();
				
					string temp1 = "Activation";
					CryLogAlways("%s", temp1);
					//CryLogAlways("%s", string(butt));
					//ActivateOutput(pActInfo, EOP_Message,static_cast<string>butt);
					ActivateOutput(pActInfo, EOP_Message,temp1);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				}
			}
			break;
		
		case eFE_Update:
			{	
				if (vrpnAnalog){
					vrpnAnalog->mainloop();
					//sprintf(chaine,"%f",test_analog.channel[1]);
					//string temp1 = "Update";
					//CryLogAlways("%s", temp1);
					char * temp1;
					temp1 = (char*)malloc(256*sizeof(vrpn_float64)); 
					//Fonctionne !
					/*
					for (int i=0;i<test_analog->num_channel;i++){
						sprintf(temp1,"%lf", test_analog->channel[i]); //FONCTIONNE BIEN
					}
					*/
					Vec3 position;
					position.x = (float)test_analog->pos[0];
					position.y = (float)test_analog->pos[1];
					position.z = (float)test_analog->pos[2];

					sprintf(temp1,"Tracker : Pos[%lf,%lf,%lf] | Quat[%lf,%lf,%lf]", test_analog->pos[0],test_analog->pos[1],test_analog->pos[2],test_analog->quat[0],test_analog->quat[1],test_analog->quat[2],test_analog->quat[3]);
					ActivateOutput(pActInfo, EOP_Message,string(temp1));

					
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

					ActivateOutput(pActInfo, EOP_VPosition,position);
					ActivateOutput(pActInfo, EOP_VRotation,out);
					ActivateOutput(pActInfo, EOP_WRotation,outw);
					ActivateOutput(pActInfo, EOP_VDirection,dir);
				}
				else {
					char * chaine = (char*)malloc(1024*sizeof(char)+1024*sizeof(f32)) ;
					string temp1 = "Erreur";
					//CryLogAlways("%s", temp1);
					sprintf(chaine,"ERREUR");
					ActivateOutput(pActInfo, EOP_Message,string(chaine));
				}
			}
			break;
		}
	}

	////////////////////////////////////////////////////
	virtual void GetMemoryStatistics(ICrySizer *s)
	{
		s->Add(*this);
	}

	////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
	{
		return new CFlowNode_VRPNTest(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}


};


REGISTER_FLOW_NODE("CryVR:VRPN:VRPNTest", CFlowNode_VRPNTest);
