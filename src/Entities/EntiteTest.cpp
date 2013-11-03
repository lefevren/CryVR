/* Entite Test - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Entities/EntiteTest.h"
#include "Game.h"
#include "Actor.h"

//------------------------------------------------------------------------
CEntiteTest::CEntiteTest(){}

//------------------------------------------------------------------------
CEntiteTest::~CEntiteTest(){}


	// IGameObjectExtension
	void CEntiteTest::InitClient(int channelId) {};
	void CEntiteTest::PostInitClient(int channelId) {};
	bool CEntiteTest::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { return false; }
	void CEntiteTest::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {};
	void CEntiteTest::PostSerialize() {};
	void CEntiteTest::SerializeSpawnInfo( TSerialize ser ) {};
	void CEntiteTest::PostUpdate(float frameTime ) {};
	void CEntiteTest::PostRemoteSpawn() {};
	void CEntiteTest:: SetChannelId(uint16 id) {}
	ISerializableInfoPtr CEntiteTest::GetSpawnInfo() {return 0;}
	bool CEntiteTest::GetEntityPoolSignature( TSerialize signature ) { return false; }
	void CEntiteTest::PostInit(IGameObject *pGameObject){GetGameObject()->EnableUpdateSlot(this, 0);}
	void CEntiteTest::Release(){delete this;}
	void CEntiteTest::HandleEvent( const SGameObjectEvent &){};
	void CEntiteTest::SetAuthority(bool auth){};
//Fonctions simples

	//A defninir
	bool CEntiteTest::Init(IGameObject *pGameObject){
		SetGameObject(pGameObject);
		const int nSlot = 0;
		IEntity* pEntity = GetEntity();
		pEntity->LoadGeometry(nSlot,"objects/default.cgf");
		GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);
		if (!Reset())
			return false;
		if(!GetGameObject()->BindToNetwork())
			return false;

		return true;
	}

	void CEntiteTest::GetMemoryUsage( ICrySizer *s ) const
	{
		s->Add(*this);
		//Pour un tableau de type Vector
		//si : std::vector<int> m_spinningEnts
		//alors
		//s->AddContainer(m_spinningEnts);
	}
	
	void CEntiteTest::FullSerialize( TSerialize ser ){
		//ser.Value("Pos", m_position);
		//ser.Value("Dir", m_direction);
	}
	
	//~IGameObjectExtension


	//Autre fonctions

	bool CEntiteTest::Reset()
	{
	//Initialize default values before (in case ScriptTable fails)
		
		m_float = 10.0f;
		m_bool = false;
		
		SmartScriptTable props;
		IScriptTable* pScriptTable = GetEntity()->GetScriptTable();
		if(!pScriptTable || !pScriptTable->GetValue("Properties", props))
			return false;

		props->GetValue("fFloat", m_float);
		props->GetValue("bBool", m_bool);
		props->GetValue("fPositionX", m_posX);
		props->GetValue("fPositionY", m_posY);
		props->GetValue("fPositionZ", m_posZ);
  
		Matrix34 m = GetEntity()->GetWorldTM();
		m_position = GetEntity()->GetWorldPos();
		m_direction = m.GetColumn(1)*0.414214f;
		m_posOrigin = m_position;

		Vec3 pos = GetEntity()->GetWorldPos();
		gEnv->pLog->Log("EntiteTest INIT POS: %f %f %f", pos.x, pos.y, pos.z);
		/*
		m_points[0] = pos;
		m_points[1] = pos + Vec3(0,0,m_cloudHeight/8.0f);
		m_points[2] = pos + Vec3(0,0,m_cloudHeight/2.0f);
		m_points[3] = pos + Vec3(0,0,m_cloudHeight);
		for (int i=0; i<4; ++i)
			m_oldPoints[i] = m_points[i];
		*/
		//m_currentPos = GetEntity()->GetWorldPos();
		//A voir ici !!!
		//GetGameObject()->ChangedNetworkState(POSITION_ASPECT);
		

		//UpdateTornadoSpline();
		return true;
	}

	bool CEntiteTest::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags )
	{
		if (aspect == POSITION_ASPECT){
			ser.BeginGroup( "EntiteTest" );
			ser.Value("Pos", m_position);
			ser.Value("Dir", m_direction);
			ser.EndGroup();
		}
		
		return true;
	}

	void CEntiteTest::ProcessEvent(SEntityEvent &event)
	{
		switch (event.event)
		{
		case ENTITY_EVENT_RESET:
			Reset();
			break;
		}
	}

	void CEntiteTest::Update(SEntityUpdateContext &ctx, int updateSlot)
	{
		if (g_pGame->GetIGameFramework()->IsEditing())
			return;

		// wandering
		Matrix34 m = GetEntity()->GetWorldTM();
		Vec3 dir(m.GetColumn(1));
		//m.set
		
		m_oldValue = m_position.x;
		m_nextValue = m_oldValue+5*sin(m_float*25)+m_posOrigin.x;
		

		if(gEnv->bServer)
		{
			
			//IActor * pActor = g_pGame->GetIGameFramework()->GetClientActor();
			//m = pActor->GetEntity()->GetWorldTM();
			//m_position = pActor->GetEntity()->GetPos();
			//m_direction = m.GetColumn(1);
			//m_float+=0;
			
			if (m_float > 2*3.1415) m_float = 0;
			else m_float = m_float+(float)0.001;
			m_position.x = 5*sin(m_float*25)+m_posOrigin.x;
			GetGameObject()->ChangedNetworkState(POSITION_ASPECT);
			m.SetTranslation(m_position);
			GetEntity()->SetWorldTM(m);
			GetGameObject()->RequestRemoteUpdate(eEA_GameServerDynamic);
			GetGameObject()->ForceUpdate(true);
			//CryLogAlways("%f",m_position.x);
		}
		else
		{
			/*
			m_nextValue = m_position.x;
			if (m_position.x == m_old_value) {
				m_nextValue = m_oldValue+5*sin(m_float*25);
			}
			*/
			GetGameObject()->ForceUpdate(true);
			//CryLogAlways("%f",m_position.x);
			//CryLogAlways("Cote Client");
			m.SetTranslation(m_position);
			//m.SetColumn(1, m_direction);
			GetEntity()->SetWorldTM(m);
		}
	}
