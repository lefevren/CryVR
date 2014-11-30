/* Compute Camera node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CFlowSetCameraFrustum : public CFlowBaseNode<eNCT_Instanced>
{

	float fov;// = 60.0;
	float ratio;// = 1.777777;
	float ndist;// = 0.25;

	enum EInputPorts
	{
		IN_ACTIVE,
		IN_WIDTH,
		IN_HEIGHT,
		IN_FOV,
		IN_RATIO,
		
	};

	enum EOutputPorts
	{
		OUT_MESSAGE,
	};

public:
	
	bool active;

	CFlowSetCameraFrustum(SActivationInfo *pActInfo){}
	virtual ~CFlowSetCameraFrustum(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool> ("Activation",_HELP("Active")),
			InputPortConfig<int> ("Width",_HELP("ScreenWidth")),
			InputPortConfig<int> ("Height",_HELP("Tete (Tracking vicon)")),
			InputPortConfig<float> ("fov",_HELP("Fov")),
			InputPortConfig<float> ("ratio",_HELP("Ratio")),
			
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string> ("Message", _HELP("Return")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG SetFrustum");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		Quat eye_rotation;
		switch (event)
		{
		case eFE_Initialize:
			{
			// A modifier
			//fov = 60*3.141592/180.0;
			//ratio = 1.777777; // Dynamique
			//ndist = 0.25; 


			//pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
		case eFE_Activate:
			{
				if ( IsPortActive( pActInfo, IN_ACTIVE ) ){
					CryLogAlways("Port active");

					active = GetPortBool( pActInfo, IN_ACTIVE);
					CryLogAlways("Active ok");

					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,active);
					CryLogAlways("Regular update ok");

					if(!active) {
						CryLogAlways("ResetCamFrustum");
						CCamera cam = gEnv->pSystem->GetViewCamera(); // CONSTANTE
						int w = GetPortInt(pActInfo,IN_WIDTH);
						int h = GetPortInt(pActInfo,IN_HEIGHT);
						float fo = GetPortFloat(pActInfo,IN_FOV);
						cam.SetFrustum(w,h,fo,cam.GetNearPlane(),cam.GetFarPlane(),1);
						
						string pMessage="Frustum Reseted";				
						
						ActivateOutput(pActInfo, OUT_MESSAGE,pMessage);
					
						gEnv->pSystem->SetViewCamera(cam);
					}


				}
					
			}
			break;
		
		case eFE_Update:
			{	
				
				IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor(); // CONSTANTE
				if(pActor){

						CCamera cam = gEnv->pSystem->GetViewCamera(); // CONSTANTE
						int w = GetPortInt(pActInfo,IN_WIDTH);
						int h = GetPortInt(pActInfo,IN_HEIGHT);
						float fo = GetPortFloat(pActInfo,IN_FOV);
						float ra = GetPortFloat(pActInfo,IN_RATIO);
						
						cam.SetFrustum(w,h,fo,cam.GetNearPlane(),cam.GetFarPlane(),ra);
						string pMessage="Frustum updated";				
						ActivateOutput(pActInfo, OUT_MESSAGE,pMessage);
					
						gEnv->pSystem->SetViewCamera(cam);

				}
				else {CryLogAlways("Erreur, pas d'acteur");}
								
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowSetCameraFrustum(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


REGISTER_FLOW_NODE("CryVR:Camera:SetCameraFrustum", CFlowSetCameraFrustum);
