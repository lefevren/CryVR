#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

//FlowAsymmetricCamera.cpp

class CFlowNode_AsymmetricCamera : public CFlowBaseNode<eNCT_Instanced>
{
	
	enum EInputPorts
	{
		EIP_IsServer = false,
		EIP_AsymL = 0,
		EIP_AsymR = 0,
		EIP_AsymT = 0,
		EIP_AsymB = 0,
	};

	enum EOutputPorts
	{
		EOP_Message,
	};

public:
	
	////////////////////////////////////////////////////
	CFlowNode_AsymmetricCamera(SActivationInfo *pActInfo)
	{
	}

	////////////////////////////////////////////////////
	virtual ~CFlowNode_AsymmetricCamera(void)
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
			InputPortConfig<bool>("IsServer",true, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymL",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymR",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymT",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymB",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<EntityId> ("Camera",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Message", _HELP("Debug message from multiplayer setup")),
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
		case eFE_Initialize:{
		}
		case eFE_Activate:
			{
				string pMessage="AsymetricCamera activated!";
				ActivateOutput(pActInfo, EOP_Message,pMessage );

				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);

				/*
				IActor * pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
				
				if(pClientActor){
						pClientActor->ToggleThirdPerson();
						pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				}

				//string chaine = gEnv->pConsole->GetCVar("Essai");
				*/
			}
			break;
		
		case eFE_Update:
			{	// Get the current client
				IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor();
				string pMessage="Update activated";
				// only add something if this is not the server
				if(pActor){
					CCamera cam = gEnv->pSystem->GetViewCamera();
					
					cam.SetAsymmetry(GetPortFloat(pActInfo,1),GetPortFloat(pActInfo,2),GetPortFloat(pActInfo,3),GetPortFloat(pActInfo,4));

					//Erreur 3.5.4 ici
					//pMessage=static_cast<string>(cam.GetAsymL());
					pMessage="3.5.4Error";

					gEnv->pSystem->SetViewCamera(cam);
					ActivateOutput(pActInfo, EOP_Message,pMessage );
				}
				else {
					ActivateOutput(pActInfo, EOP_Message,pMessage );
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
		return new CFlowNode_AsymmetricCamera(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}
};

////////////////////////////////////////////////////
////////////////////////////////////////////////////

//REGISTER_FLOW_NODE("Multiplayer:GetServerEntity", CFlowNode_GetServerEntity);
REGISTER_FLOW_NODE("CryVR:Camera:AsymmetricCamera", CFlowNode_AsymmetricCamera);