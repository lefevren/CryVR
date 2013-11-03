/* Camera Info node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"

//FlowAsymmetricCamera.cpp

class CFlowNode_CameraInfo : public CFlowBaseNode<eNCT_Instanced>
{
	
	enum EInputPorts
	{
		EIP_IsServer = false,
		EIP_AsymL = 0,
		EIP_Asym_R = 0,
		EIP_decalage_local_gauche_droite = 0,
		EIP_ligne_0,
		EIP_ligne_1,
		EIP_ligne_2,
		EIP_Asym_T=0,
		EIP_Asym_B=0,

	};

	enum EOutputPorts
	{
		EOP_Message,
		EOP_MatriceProjectionRow0,
		EOP_MatriceProjectionRow1,
		EOP_MatriceProjectionRow2,
		
		EOP_AsymL,
		EOP_AsymR,
		EOP_AsymT,
		EOP_AsymB,
		EOP_NearC,
		EOP_FarC,

		EOP_Plan_Lointain,
		EOP_Plan_Proche,

		EOP_Plan_Proj_Cam,
		EOP_Plan_Proj_World,
		EOP_Plan_Proj_World_Local,
	};

public:
	
	////////////////////////////////////////////////////
	CFlowNode_CameraInfo(SActivationInfo *pActInfo)
	{
	}

	////////////////////////////////////////////////////
	virtual ~CFlowNode_CameraInfo(void)
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
			InputPortConfig<float>("Decalage_local_gauche_droite",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<Vec3>("Ligne00", _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<Vec3>("Ligne01", _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<Vec3>("Ligne02", _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymT",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			InputPortConfig<float>("AsymB",0.0, _HELP("Determine if this computer is a server for a CAVE")),
			
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Message", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<string>("MatriceProjectionRow0", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<string>("MatriceProjectionRow1", _HELP("Debug message from multiplayer setup")),
			OutputPortConfig<string>("MatriceProjectionRow2", _HELP("Debug message from multiplayer setup")),
			
			OutputPortConfig<string>("AsymL", _HELP("Determine if this computer is a server for a CAVE")),
			OutputPortConfig<string>("AsymR", _HELP("Determine if this computer is a server for a CAVE")),
			OutputPortConfig<string>("AsymT", _HELP("Determine if this computer is a server for a CAVE")),
			OutputPortConfig<string>("AsymB", _HELP("Determine if this computer is a server for a CAVE")),
			OutputPortConfig<string> ("NearC",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			OutputPortConfig<string> ("FarC",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			
			OutputPortConfig<string> ("Plan_Lointain",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			OutputPortConfig<string> ("Plan_Proche",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			
			OutputPortConfig<string> ("Plan_Projection_cam",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			OutputPortConfig<string> ("Plan_Projection_world",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			OutputPortConfig<string> ("Plan_Projection_world_local",_HELP("Optional entity used to act as the camera for the test. Does not have to be a real camera.")),
			
			
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
		
		char * chaineRow0 = (char*)malloc(32*sizeof(char)+8*sizeof(f32)) ;
		char * chaineRow1 = (char*)malloc(32*sizeof(char)+8*sizeof(f32)) ;
		char * chaineRow2 = (char*)malloc(32*sizeof(char)+8*sizeof(f32)) ;
		
		
		char * chaineAsymL = NULL ;
		char * chaineAsymR = NULL ;
		char * chaineAsymT = NULL ;
		char * chaineAsymB = NULL ;
		char * chaineFarC = NULL ;
		char * chaineNearC = NULL ;
		

		char * plan_proche ;
		char * plan_lointain ;

		char * plan_projection_cam ;
		char * plan_projection_world ;

		char * plan_projection_world_Local ;

		
		chaineAsymL = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		chaineAsymR = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		chaineAsymT = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		chaineAsymB = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		chaineFarC = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		chaineNearC = (char*)malloc(32*sizeof(char)+5*sizeof(f32));
		

		plan_proche = (char*)malloc(64*sizeof(char)+16*sizeof(Vec3));
		plan_lointain = (char*)malloc(64*sizeof(char)+16*sizeof(Vec3));
		
		plan_projection_cam = (char*)malloc(64*sizeof(char)+16*sizeof(Vec3));
		plan_projection_world = (char*)malloc(64*sizeof(char)+16*sizeof(Vec3));
		plan_projection_world_Local = (char*)malloc(64*sizeof(char)+16*sizeof(Vec3));



		switch (event)
		{
		case eFE_Initialize:{
		}
		case eFE_Activate:
			{
				
				string pMessage="Flow activated";
			
					chaineRow0 = "Carabo";
					chaineRow1 = "Carabo";
					chaineRow2 = "Carabo";
					
					ActivateOutput(pActInfo, EOP_Message,pMessage );
					
					
					ActivateOutput(pActInfo, EOP_MatriceProjectionRow0,string(chaineRow0));
					ActivateOutput(pActInfo, EOP_MatriceProjectionRow1,string(chaineRow1));
					ActivateOutput(pActInfo, EOP_MatriceProjectionRow2,string(chaineRow2));
					
				
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				
			}
			break;
		
		case eFE_Update:
			{	// Get the current client
				IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor();
				string pMessage="Update activated";
				if(pActor){
					CCamera cam = gEnv->pSystem->GetViewCamera();
					cam.SetAsymmetry(GetPortFloat(pActInfo,1),GetPortFloat(pActInfo,2),GetPortFloat(pActInfo,7),GetPortFloat(pActInfo,8));


					//Matrix34 ma_matrice =  cam.GetViewMatrix();
					Matrix34 ma_matrice = cam.GetViewMatrix();


					if (!GetPortVec3(pActInfo,4).IsZero() && !GetPortVec3(pActInfo,6).IsZero() && !GetPortVec3(pActInfo,5).IsZero()){

						Matrix34 ma_mat = cam.GetViewMatrix();
						ma_mat.SetRow(0,GetPortVec3(pActInfo,4));
						ma_mat.SetRow(1,GetPortVec3(pActInfo,5));
						ma_mat.SetRow(2,GetPortVec3(pActInfo,6));
						cam.SetMatrix(ma_mat);
						
					}

					
					sprintf(chaineRow0,"Ligne 0 : {%.2f, %.2f, %.2f, %.2f}",ma_matrice.m00,ma_matrice.m01,ma_matrice.m02,ma_matrice.m03);
					sprintf(chaineRow1,"Ligne 1 : {%.2f, %.2f, %.2f, %.2f}",ma_matrice.m10,ma_matrice.m11,ma_matrice.m12,ma_matrice.m13);
					sprintf(chaineRow2,"Ligne 2 : {%.2f, %.2f, %.2f, %.2f}",ma_matrice.m20,ma_matrice.m21,ma_matrice.m22,ma_matrice.m23);
									
					sprintf(chaineAsymL,"Asymetrie Gauche : %.2f",cam.GetAsymL());
					sprintf(chaineAsymR,"Asymetrie Droite : %.2f",cam.GetFov());
					sprintf(chaineAsymT,"Asymetrie Haut : %.2f",cam.GetAsymT());
					sprintf(chaineAsymB,"Asymetrie Bas : %.2f",cam.GetAsymB());
					sprintf(chaineFarC,"Plan Lointain : %.2f",cam.GetFarPlane());
					sprintf(chaineNearC,"Plan Proche : %.2f",cam.GetNearPlane());
					
					Vec3 plan [8] ;
					cam.GetFrustumVertices(plan);
					
					sprintf(plan_proche,"Plan proche : [%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f]",plan[0].x,plan[0].y,plan[0].z,plan[1].x,plan[1].y,plan[1].z,plan[2].x,plan[2].y,plan[2].z,plan[3].x,plan[3].y,plan[3].z);
					sprintf(plan_lointain,"Plan Lointain : [%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f]",plan[4].x,plan[4].y,plan[4].z,plan[5].x,plan[5].y,plan[5].z,plan[6].x,plan[6].y,plan[6].z,plan[7].x,plan[7].y,plan[7].z);

					Vec3 plan_proj_cam[8];
					cam.GetFrustumVerticesCam(plan_proj_cam);


					Vec3 plan_proj_world[8];
					cam.GetFrustumVertices(plan_proj_world);

					

					//cam.SetFrustumVertices()

					//WorldToLocal();


					
					cam.GetViewMatrix();
					Vec3 sommet [4] ;
					//Matrix34 camMatrix = cam.GetMatrix();
					Matrix34 camMatrix = cam.GetViewMatrix();
					camMatrix.InvertFast();
					
					sommet[0] = (camMatrix) * plan_proj_world[4] ; //Transformation world -> Local ?
					sommet[1] = (camMatrix) * plan_proj_world[5] ;
					sommet[2] = (camMatrix) * plan_proj_world[6] ;
					sommet[3] = (camMatrix) * plan_proj_world[7] ;

					plan_proj_cam[0].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[1].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[2].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[3].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[4].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[5].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[6].x+=GetPortFloat(pActInfo,3);
					plan_proj_cam[7].x+=GetPortFloat(pActInfo,3);
					
					cam.SetFrustumVertices(plan_proj_cam);
					//cam.SetFru
					//cam.SetMatrix
					
					sprintf(plan_projection_cam,"Plan Projection Proche Camera : [%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f]",plan_proj_cam[0].x,plan_proj_cam[0].y,plan_proj_cam[0].z,plan_proj_cam[1].x,plan_proj_cam[1].y,plan_proj_cam[1].z,plan_proj_cam[2].x,plan_proj_cam[2].y,plan_proj_cam[2].z,plan_proj_cam[3].x,plan_proj_cam[3].y,plan_proj_cam[3].z);
					sprintf(plan_projection_world,"Plan Projection Proche World : [%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f]",plan_proj_world[4].x,plan_proj_world[4].y,plan_proj_world[4].z,plan_proj_world[5].x,plan_proj_world[5].y,plan_proj_world[5].z,plan_proj_world[6].x,plan_proj_world[6].y,plan_proj_world[6].z,plan[7].x,plan_proj_world[7].y,plan_proj_world[7].z);
					

					sprintf(plan_projection_world_Local,"Plan Projection Proche World : [%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f][%.2f,%.2f,%.2f]",sommet[0].x,sommet[0].y,sommet[0].z,sommet[1].x,sommet[1].y,sommet[1].z,sommet[2].x,sommet[2].y,sommet[2].z,sommet[3].x,sommet[3].y,sommet[3].z);

					
					ActivateOutput(pActInfo, EOP_Message,pMessage );
					
					ActivateOutput(pActInfo, EOP_AsymL,string(chaineAsymL) );
					ActivateOutput(pActInfo, EOP_AsymR,string(chaineAsymR) );
					ActivateOutput(pActInfo, EOP_AsymT,string(chaineAsymT) );
					ActivateOutput(pActInfo, EOP_AsymB,string(chaineAsymB) );
					ActivateOutput(pActInfo, EOP_NearC,string(chaineNearC) );
					ActivateOutput(pActInfo, EOP_FarC,string(chaineFarC) );
					
					
					ActivateOutput(pActInfo, EOP_Plan_Proche,string(plan_proche) );
					ActivateOutput(pActInfo, EOP_Plan_Lointain,string(plan_lointain) );
					

					ActivateOutput(pActInfo, EOP_MatriceProjectionRow0,string(chaineRow0));
					ActivateOutput(pActInfo, EOP_MatriceProjectionRow1,string(chaineRow1));
					ActivateOutput(pActInfo, EOP_MatriceProjectionRow2,string(chaineRow2));
					
					ActivateOutput(pActInfo, EOP_Plan_Proj_Cam,string(plan_projection_cam));
					ActivateOutput(pActInfo, EOP_Plan_Proj_World,string(plan_projection_world));
					ActivateOutput(pActInfo, EOP_Plan_Proj_World_Local,string(plan_projection_world_Local));
					
					gEnv->pSystem->SetViewCamera(cam);
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
		return new CFlowNode_CameraInfo(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}
};


REGISTER_FLOW_NODE("CryVR:Camera:CameraInfo", CFlowNode_CameraInfo);