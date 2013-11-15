/* Entity simple orientation node - for licensing and copyright see license.txt */


#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CryVR_EntitySimpleOrientation : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_EntityID,
		EIP_Set,
		EIP_Rotation_XYZ,
		EIP_Rotation_W,
	};

	enum EOutputPorts
	{
		EOP_Message,
	};

public:
	
	////////////////////////////////////////////////////
	CryVR_EntitySimpleOrientation(SActivationInfo *pActInfo){}
	
	////////////////////////////////////////////////////
	virtual ~CryVR_EntitySimpleOrientation(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<EntityId> ("Entity",_HELP("Object to rotate")),
			InputPortConfig<bool> ("Set",_HELP("Set orientation")),
			InputPortConfig<Vec3>("RotationXYZ", _HELP("Quaternion XYZ part")),
			InputPortConfig<float>("RotationW", _HELP("Quaternion W part")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node to set orientation by a quaternion for the given Entity");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		switch (event)
		{
			case eFE_Initialize:
				{
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,false);
				}
			case eFE_Activate:
				{
					if(GetPortBool(pActInfo, 1)) {
						EntityId id = GetPortEntityId(pActInfo,EIP_EntityID);
						IEntity * entity = gEnv->pEntitySystem->GetEntity(id);
						if(entity)
						{
							Matrix34 trans; // = entity->GetWorldTM();
							trans.CreateIdentity();
															
							Quat quat =  entity->GetRotation();
							quat.v = GetPortVec3( pActInfo, EIP_Rotation_XYZ);
							quat.w = GetPortFloat( pActInfo, EIP_Rotation_W);
							
							//quat.NormalizeFast();
					
							Vec3 position = entity->GetPos();
							trans.Set(Vec3(1,1,1),quat.GetNormalized(),position);
							
							entity->SetPosRotScale(position,quat,Vec3(1,1,1));

							
						}
					}
				}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_EntitySimpleOrientation(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


};


REGISTER_FLOW_NODE("CryVR:Entities:EntitySimpleOrientation", CryVR_EntitySimpleOrientation);
