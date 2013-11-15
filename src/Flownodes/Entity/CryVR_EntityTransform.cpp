/* Entity simple orientation node - for licensing and copyright see license.txt */
/* Debug node */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CryVR_EntityTransform : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_EntityID,
		EIP_ParentEntityID,
		//EIP_OffsetPosition,
		EIP_Position,
		EIP_Rotation_XYZ,
		EIP_Rotation_W,
		EIP_Scale,
	};

	enum EOutputPorts
	{
		EOP_Message,
	};

public:
	
	////////////////////////////////////////////////////
	CryVR_EntityTransform(SActivationInfo *pActInfo){}
	
	////////////////////////////////////////////////////
	virtual ~CryVR_EntityTransform(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<EntityId> ("Entity",_HELP("Objet a positionner")),
			InputPortConfig<EntityId> ("ParentEntity",_HELP("Id de l'objet Vicon")),
			//InputPortConfig<Vec3>("OffsetPositionXYZ", _HELP("Connexion à un server VRPN")),
			InputPortConfig<Vec3>("PositionXYZ", _HELP("Position [x,y,z] du tracker dans le repere Vicon")),
			InputPortConfig<Vec3>("RotationXYZ", _HELP("Rotation [x,y,z] du tracker dans le repere Vicon")),
			InputPortConfig<float>("RotationW", _HELP("Rotation [w] du tracker dans le repere Vicon")),
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
		config.sDescription = _HELP("FG pour le positionnement d'un objet dans le repere d'un second objet. La matrice de transformation est donnee par un tracker Vicon. Le second objet correspond au point 0 du Vicon");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		
		switch (event)
		{
		case eFE_Initialize:{
			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
		}
		case eFE_Activate:
			{
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
			break;
		
		case eFE_Update:
			{	
				//if (IsPortActive(pActInfo, PORT_IN_ENABLE))
				//{
					EntityId id = GetPortEntityId(pActInfo,EIP_EntityID);
					EntityId parentid = GetPortEntityId(pActInfo,EIP_ParentEntityID);
					//IGameObject *pGameObj = gEnv->pGameFramework->GetGameObject(id);
					IEntity * entity = gEnv->pEntitySystem->GetEntity(id);
					IEntity * parententity = gEnv->pEntitySystem->GetEntity(parentid);
					if(entity){
						Matrix34 trans; // = entity->GetWorldTM();
						trans.CreateIdentity();
						Matrix34 transparent;
						transparent.CreateIdentity(); 

						if (parententity){
							//quatParent = parententity->GetRotation();
							transparent = parententity->GetWorldTM();
							//CryLogAlways("Parent : [%f,%f,%f]",transparent.GetColumn0().x,transparent.GetColumn0().y,transparent.GetColumn0().z);
							//CryLogAlways("Parent : [%f,%f,%f]",transparent.GetColumn1().x,transparent.GetColumn1().y,transparent.GetColumn1().z);
							//CryLogAlways("Parent : [%f,%f,%f]",transparent.GetColumn2().x,transparent.GetColumn2().y,transparent.GetColumn2().z);
							//CryLogAlways("Parent : [%f,%f,%f]",transparent.GetColumn3().x,transparent.GetColumn3().y,transparent.GetColumn3().z);
							
						}
						
						Quat quat =  entity->GetRotation();
						quat.v = GetPortVec3( pActInfo, EIP_Rotation_XYZ);
						quat.w = GetPortFloat( pActInfo, EIP_Rotation_W);
						quat.NormalizeFast();
						

						Vec3 position = GetPortVec3(pActInfo, EIP_Position);
						
						trans.Set(Vec3(1,1,1),quat.GetNormalized(),position);
						//transparent.SetTranslation(Vec3(0,0,0));
						
						Vec3 positionlocal = transparent.TransformVector(position);

						trans = transparent * trans;

						//Matrix34 transresult ;
						//transresult.CreateIdentity();
						//transresult.Scale
						
						trans.SetTranslation(positionlocal+parententity->GetPos());

						if(trans.IsValid()){
							entity->SetWorldTM(trans);
						}
						else {
							//CryLogAlways("[%f,%f,%f]",trans.GetColumn0().x,trans.GetColumn0().y,trans.GetColumn0().z);
							//CryLogAlways("[%f,%f,%f]",trans.GetColumn1().x,trans.GetColumn1().y,trans.GetColumn1().z);
							//CryLogAlways("[%f,%f,%f]",trans.GetColumn2().x,trans.GetColumn2().y,trans.GetColumn2().z);
							//CryLogAlways("[%f,%f,%f]",trans.GetColumn3().x,trans.GetColumn3().y,trans.GetColumn3().z);
							//CryLogAlways("rot : [%f,%f,%f,%f]",trans.GetRo);
							//CryLogAlways("sca : [%f,%f,%f]",trans.GetColumn0().x,trans.GetColumn0().y,trans.GetColumn0().z);
						}
					}
					//CryLogAlways("%i",id);
				//}
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_EntityTransform(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


};


REGISTER_FLOW_NODE("CryVR:Entities:EntityTransform", CryVR_EntityTransform);
