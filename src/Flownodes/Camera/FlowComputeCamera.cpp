#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CFlowComputeCamera : public CFlowBaseNode<eNCT_Instanced>
{

	float fov;// = 60.0;
	float ratio;// = 1.777777;
	float ndist;// = 0.25;

	enum EInputPorts
	{
		EYE_ID,
		HEAD_ID,
		VICON_ID,
		SCREEN_ID,
		
		
		//SCREEN_Direction,
		SCREEN_PARAMS,
		EYE_POS,
		//CAMERA_AbsolutePos,
		//SCREEN_AbsolutePos,
		//VICON_AbsolutePos,
	};

	enum EOutputPorts
	{
		CAMERA_CurrentFOV,
	};

public:
	
	EntityId id_tete;// = GetPortEntityId(pActInfo,HEAD_ID);//CONSTANTE
	EntityId id_oeil;// = GetPortEntityId(pActInfo,EYE_ID);//CONSTANTE
	EntityId id_ecran;// = GetPortEntityId(pActInfo,SCREEN_ID);//CONSTANTE
	EntityId id_vicon;// = GetPortEntityId(pActInfo,VICON_ID);//CONSTANTE


	IEntity * head_entity;// = gEnv->pEntitySystem->GetEntity(id_tete);//CONSTANTE
	IEntity * eye_entity;// = gEnv->pEntitySystem->GetEntity(id_oeil);//CONSTANTE
	IEntity * vicon_entity;// = gEnv->pEntitySystem->GetEntity(id_vicon);//CONSTANTE
	IEntity * ecran_entity;// = gEnv->pEntitySystem->GetEntity(id_ecran);//CONSTANTE

	Vec3 ScreenParams;// = GetPortVec3(pActInfo, SCREEN_PARAMS); // CONSTANTE
	Vec3 position_ecran;				

	Quat eye_rotation;// = eye_entity->GetRotation(); //CONSTANTE
	
	

	CFlowComputeCamera(SActivationInfo *pActInfo){}
	virtual ~CFlowComputeCamera(void){}
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<EntityId> ("Oeil",_HELP("Objet parent de la camera")),
			InputPortConfig<EntityId> ("Tete",_HELP("Tete (Tracking vicon)")),
			InputPortConfig<EntityId> ("Vicon",_HELP("Vicon 0")),
			InputPortConfig<EntityId> ("Ecran",_HELP("Ecran de projection")),
			
			InputPortConfig<Vec3> ("ScreenParams",Vec3(4,3,2.25), _HELP("[DistanceProjection,LongueurEcran,HauteurEcran]")),
			InputPortConfig<Vec3> ("PositionCourante",Vec3(0,0,2.25), _HELP("Position courante de la camera par rapport au 0,0,0 du Vicon")),
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<float> ("Current Fov", _HELP("Fov courrante")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("FG calculant les asymetrie et les plans de détourages");
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
			fov = 60*3.141592/180.0;
			ratio = 1.777777; // Dynamique
			ndist = 0.25; 


			pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
		case eFE_Activate:
			{
				/* Initialisation des objets */
				id_tete = GetPortEntityId(pActInfo,HEAD_ID);//CONSTANTE
				id_oeil = GetPortEntityId(pActInfo,EYE_ID);//CONSTANTE
				id_ecran = GetPortEntityId(pActInfo,SCREEN_ID);//CONSTANTE
				id_vicon = GetPortEntityId(pActInfo,VICON_ID);//CONSTANTE
				
				head_entity = gEnv->pEntitySystem->GetEntity(id_tete);//CONSTANTE
				eye_entity = gEnv->pEntitySystem->GetEntity(id_oeil);//CONSTANTE
				vicon_entity = gEnv->pEntitySystem->GetEntity(id_vicon);//CONSTANTE
				ecran_entity = gEnv->pEntitySystem->GetEntity(id_ecran);//CONSTANTE
				/* Fin Initialisation des objets */

				ScreenParams = GetPortVec3(pActInfo, SCREEN_PARAMS); // CONSTANTE
				

				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
			}
			break;
		
		case eFE_Update:
			{	
				if(eye_entity && vicon_entity && ecran_entity && head_entity) {
					Vec3 head_pos = GetPortVec3(pActInfo,EYE_POS); // VARIABLE
					ratio =  gEnv->pSystem->GetViewCamera().GetProjRatio(); // Dynamique
					position_ecran = ecran_entity->GetPos(); // CONSTANTE
					eye_rotation = eye_entity->GetRotation(); //CONSTANTE

					Matrix34 vicon_matrix = vicon_entity->GetWorldTM(); // VARIABLE
					vicon_matrix.SetTranslation(Vec3(0,0,0)); // VARIABLE
					
					/* Positionnement de l'objet head Trouver une solution rapide */
					Vec3 head_world_pos = vicon_matrix.TransformVector(head_pos)+vicon_entity->GetPos(); // VARIABLE
					Matrix34 head_matrix = head_entity->GetWorldTM(); // VARIABLE
					head_matrix.SetTranslation(head_world_pos);
					head_entity->SetWorldTM(head_matrix);
					/* Fin Positionnement de l'objet head */
										
					//vicon_matrix.SetTranslation(vicon_entity->GetPos());
					Vec3 screen_vicon_pos = ecran_entity->GetPos(); 
					
					/* A retravailler */
					Vec3 PositionIdeale = Vec3(0,screen_vicon_pos.y,screen_vicon_pos.z); //CONSTANTE
					/* */

					/* Expression de la différence de position entre l'ecran et l'oeil pour le calcul du fov */
					Vec3 DifferencePosition = head_pos-PositionIdeale;
					DifferencePosition = DifferencePosition * eye_rotation;					
					Vec3 head_local = head_pos * eye_rotation;
					fov = (float)(2.0 * atan ( ScreenParams.z / (2.0 * (ecran_entity->GetPos().x - head_local.x)))) ;
					/* Fin calcul fov */
					
					/* Calcul des asymetries */
					//OPtimisation (right == left, top == bottom)
					float AsymLeft = (float)(2.0 * tan (fov / 2.0) * ndist * ratio * (DifferencePosition.y / ScreenParams.y)); // OK
					//float AsymRight = 2.0 * tan (fov / 2.0) * ndist * ratio * (DifferencePosition.y / ScreenParams.y); // OK
					float AsymTop =  (float)(2.0 * tan (fov / 2.0) * ndist * (-DifferencePosition.z / ScreenParams.z)); // Semble OK X
					//float AsymBottom =  2.0 * tan (fov / 2.0) * ndist * (-DifferencePosition.z / ScreenParams.z); // Semble OK X
					/* fin calcul asymetries */ 


					IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor(); // CONSTANTE
					
					
					if(pActor){
						CCamera cam = gEnv->pSystem->GetViewCamera(); // CONSTANTE
						ratio = gEnv->pSystem->GetViewCamera().GetProjRatio(); // CONSTANTE
						float fov_degre = RAD2DEG(fov);
						ActivateOutput(pActInfo, CAMERA_CurrentFOV,fov_degre);
					
						//cam.SetAsymmetry(AsymLeft,AsymRight,AsymTop,AsymBottom);
						cam.SetAsymmetry(AsymLeft,AsymLeft,AsymTop,AsymTop);

						/* Calcul de la position du centre de l'ecran dans le référentiel camera, quelque soit la rotation du Vicon */
						Vec3 ppCentre = (ecran_entity->GetWorldPos()- eye_entity->GetWorldPos());
						ppCentre = ppCentre * eye_entity->GetWorldRotation();
						//CryLogAlways("CentreEcran in Camera Space : [%f,%f,%f]", ppCentre.x,ppCentre.y,ppCentre.z);
						/* VALIDE */

						/* Calcul de la position des points de l'ecran dans le référentiel camera, quelque soit la rotation du Vicon */
						Vec3 ScreenParams_cam = ScreenParams;
						Vec3	pptl =	ppCentre  ; 
						pptl.x -=		(float)(ScreenParams_cam.y / 2.0 +0.05); 
						pptl.z +=		(float)(ScreenParams_cam.z / 2.0 +0.05); 

						Vec3	pptr =	ppCentre; 
						pptr.x +=		(float)(ScreenParams_cam.y / 2.0 +0.05); 
						pptr.z +=		(float)(ScreenParams_cam.z / 2.0 +0.05); 

						Vec3	ppbr =	ppCentre; // A trouver
						ppbr.x +=		(float)(ScreenParams_cam.y / 2.0 +0.05); 
						ppbr.z -=		(float)(ScreenParams_cam.z / 2.0 +0.05); 
					
						Vec3	ppbl = ppCentre; // A trouver
						ppbl.x -=		(float)(ScreenParams_cam.y / 2.0 +0.05); 
						ppbl.z -=		(float)(ScreenParams_cam.z / 2.0 +0.05); 

						/* Fin calcul des vertex */

						/* Calculs des vertex du near plane et du far plane dans le referentiel monde */
						Vec3 nptl = (pptl.normalized() * cam.GetNearPlane()) * (eye_entity->GetWorldRotation().GetInverted()); // Direction du coin TL * distance near plane
						Vec3 nptr = (pptr.normalized() * cam.GetNearPlane()) * (eye_entity->GetWorldRotation().GetInverted());
						Vec3 npbr = (ppbr.normalized() * cam.GetNearPlane()) * (eye_entity->GetWorldRotation().GetInverted());
						Vec3 npbl = (ppbl.normalized() * cam.GetNearPlane()) * (eye_entity->GetWorldRotation().GetInverted());

						Vec3 fptl = (pptl.normalized() * cam.GetFarPlane()) * (eye_entity->GetWorldRotation().GetInverted());
						Vec3 fptr = (pptr.normalized() * cam.GetFarPlane()) * (eye_entity->GetWorldRotation().GetInverted());
						Vec3 fpbr = (ppbr.normalized() * cam.GetFarPlane()) * (eye_entity->GetWorldRotation().GetInverted());
						Vec3 fpbl = (ppbl.normalized() * cam.GetFarPlane()) * (eye_entity->GetWorldRotation().GetInverted());

						/* fin calcul */
						
						//nptl = nptl * (eye_entity->GetWorldRotation().GetInverted()); //Exprimer dans le referentiel monde? (valeur petite)
						//nptr = nptr * (eye_entity->GetWorldRotation().GetInverted()); 
						//npbr = npbr * (eye_entity->GetWorldRotation().GetInverted());
						//npbl = npbl * (eye_entity->GetWorldRotation().GetInverted());

						//fptl = fptl * (eye_entity->GetWorldRotation().GetInverted());
						//fptr = fptr * (eye_entity->GetWorldRotation().GetInverted());
						//fpbr = fpbr * (eye_entity->GetWorldRotation().GetInverted());
						//fpbl = fpbl * (eye_entity->GetWorldRotation().GetInverted());
						

						
						/* Creation des plans de détourages dans le REFERENTIEL MONDE*/
						cam.SetFrustumPlane(0,Plane::CreatePlane( nptr+cam.GetPosition(),nptl+cam.GetPosition(),npbr+cam.GetPosition())); // Near plane
						cam.SetFrustumPlane(1,Plane::CreatePlane( fpbr+cam.GetPosition(),fptr+cam.GetPosition(),cam.GetPosition())); // OK
						cam.SetFrustumPlane(2,Plane::CreatePlane( fptl+cam.GetPosition(),fpbl+cam.GetPosition(),cam.GetPosition())); // OK
						cam.SetFrustumPlane(3,Plane::CreatePlane( fptr+cam.GetPosition(),fptl+cam.GetPosition(),cam.GetPosition())); // OK

						cam.SetFrustumPlane(4,Plane::CreatePlane( fpbl+cam.GetPosition(),fpbr+cam.GetPosition(),cam.GetPosition())); //OK
						cam.SetFrustumPlane(5,Plane::CreatePlane( fptr+cam.GetPosition(),fpbr+cam.GetPosition(),fptl+cam.GetPosition()));  //Far Plane
						
						/* fin creation des plans dans le referentiel MONDE*/

						gEnv->pSystem->SetViewCamera(cam);
					}
					else {CryLogAlways("Erreur, pas d'acteur");}
				}				
			}
		}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CFlowComputeCamera(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}

};


REGISTER_FLOW_NODE("CryVR:Camera:ComputeCamera", CFlowComputeCamera);
