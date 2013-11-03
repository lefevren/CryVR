/* Camera projection node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CFlowCamProjection : public CFlowBaseNode<eNCT_Instanced>
{

	float fov;// = 60.0;
	float ratio;// = 1.777777;
	float ndist;// = 0.25;

	enum EInputPorts
	{
		VEC_POS,
	};

	enum EOutputPorts
	{
		VEC_PROJ_POS,
		FLOAT_DISTANCE,
		IS_VISIBLE,
	};

public:
	
	CFlowCamProjection(SActivationInfo *pActInfo){}
	virtual ~CFlowCamProjection(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<Vec3> ("WorldPosition",Vec3(0,0,0), _HELP("World Position to project")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3> ("ScreenPosition", _HELP("World position in screen coordinates")),
			OutputPortConfig<float> ("distance", _HELP("world distance from cam")),
			OutputPortConfig<bool> ("IsVisible", _HELP("isVisible")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("WorldPos to ScreenPos");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		switch (event)
		{
		case eFE_Initialize:
			{
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
		case eFE_Activate:
			{
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
			break;
		
		case eFE_Update:
			{	
				
					//IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor(); // CONSTANTE
					//if(pActor){
					CCamera cam = gEnv->pSystem->GetViewCamera(); // CONSTANTE
					Vec3 resultat = Vec3(0,0,0);
					bool isVisible = cam.Project(GetPortVec3(pActInfo,0),resultat);
					
					float distance = GetPortVec3(pActInfo,0).GetDistance(cam.GetPosition());
					
					ActivateOutput(pActInfo, VEC_PROJ_POS,resultat);
					ActivateOutput(pActInfo, FLOAT_DISTANCE,distance);
					ActivateOutput(pActInfo, IS_VISIBLE,isVisible);
					//}
					//else {CryLogAlways("Erreur, pas d'acteur");}
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowCamProjection(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};
REGISTER_FLOW_NODE("CryVR:Flash:CameraProjection", CFlowCamProjection);
