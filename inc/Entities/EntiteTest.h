
#ifndef __ENTITETEST_H__
#define __ENTITETEST_H__
#pragma once

#include <IGameObject.h>


class CEntiteTest : public CGameObjectExtensionHelper<CEntiteTest, IGameObjectExtension>
{
public:
	
	static const int POSITION_ASPECT = eEA_GameServerDynamic;
	
	float							m_float,m_posX,m_posY,m_posZ;
	float							m_oldValue,m_nextValue,m_currentValue;
	bool							m_bool;
	Vec3							m_position;
	Vec3							m_direction;
	Vec3							m_posOrigin;
	
	CEntiteTest();
	virtual ~CEntiteTest();

	// IGameObjectExtension
	
	virtual void InitClient(int channelId);
	virtual void PostInitClient(int channelId);
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual bool GetEntityPoolSignature( TSerialize signature );
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual void PostInit(IGameObject *pGameObject);
	virtual bool Init(IGameObject *pGameObject);
	virtual void Release();
	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser );
	virtual ISerializableInfoPtr GetSpawnInfo();
	virtual void Update( SEntityUpdateContext &ctx, int updateSlot);
	virtual void PostUpdate(float frameTime );
	virtual void PostRemoteSpawn();
	virtual void HandleEvent( const SGameObjectEvent &);
	virtual void ProcessEvent(SEntityEvent &);
	virtual void SetChannelId(uint16 id);
	virtual void SetAuthority(bool auth);
	virtual void GetMemoryUsage(ICrySizer * s) const;
	//~IGameObjectExtension

	bool Reset();


};

#endif //__ENTITETEST_H__
