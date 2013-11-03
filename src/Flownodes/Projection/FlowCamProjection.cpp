/* Camera projection node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CFlowCamProjection : public CFlowBaseNode<eNCT_Instanced>
{

	enum EInputPorts
	{
		ACTIVE,
		VEC_POS,
	};

	enum EOutputPorts
	{
		VEC_PROJ_POS,
		FLOAT_DISTANCE,
		IS_VISIBLE,
	};

public:
	
	bool active;
	CFlowCamProjection(SActivationInfo *pActInfo){}
	virtual ~CFlowCamProjection(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Activate", _HELP("Activate")),
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
		config.sDescription = _HELP("Cry VR node to convert WorldPos to ScreenPos");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		switch (event)
		{
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

					if (!active) return;
				
					CCamera cam = gEnv->pSystem->GetViewCamera();
					Vec3 resultat = Vec3(0,0,0);
					bool isVisible = cam.Project(GetPortVec3(pActInfo,0),resultat);
					float distance = GetPortVec3(pActInfo,0).GetDistance(cam.GetPosition());
					
					ActivateOutput(pActInfo, VEC_PROJ_POS,resultat);
					ActivateOutput(pActInfo, FLOAT_DISTANCE,distance);
					ActivateOutput(pActInfo, IS_VISIBLE,isVisible);
					
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowCamProjection(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};
REGISTER_FLOW_NODE("CryVR:Camera:CameraProjection", CFlowCamProjection);
