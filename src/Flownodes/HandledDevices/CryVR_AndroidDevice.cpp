/* HandledDevices node - for licensing and copyright see license.txt */

#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidDevice : public CFlowBaseNode<eNCT_Instanced>
{
	
	enum EInputPorts
	{
		EIP_Enable,
		EIP_Disable,
		EIP_Port, 
	};

	enum EOutputs
	{
		EOP_Status = 0,
		EOP_Gyro_xyz,
		EOP_Gyro_w,
		EOP_Gyro_dir,
		EOP_Accel,
		EOP_Compass,
	};
	
	UdpListener* udpListener;
	bool m_bEnabled;
	CTimeValue m_lastTime;

public:
	////////////////////////////////////////////////////
	CryVR_AndroidDevice(SActivationInfo *pActInfo)
	{
		udpListener = new UdpListener();
	}

	
	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidDevice(void)
	{
		if (udpListener->IsWorking())udpListener->EndSocket();
	}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool>("Enable", _HELP("Enable receiving signals")),
			InputPortConfig<bool>("Disable", _HELP("Disable receiving signals")),
			InputPortConfig<int>("Port", 26000, _HELP("Port number"), 0,0),
			{0},
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("UDP socket successfully opened for listening")), 
			OutputPortConfig<Vec3>("Gyroscope_xyz", _HELP("Gyroscope xyz value")),
			OutputPortConfig<float>("Gyroscope_w", _HELP("Gyroscope w value")),
			OutputPortConfig<Vec3>("Gyroscope_direction", _HELP("Gyroscope quaternion as direction")),
			OutputPortConfig<Vec3>("Accelerometer", _HELP("Accelerometer vec3")),
			OutputPortConfig<float>("Compass", _HELP("Compass float angle")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve data from gyroscope, accelerometer and compass");
		//config.SetCategory(EFLN_ADVANCED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		switch (event)
		{
			case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_Enable)) {
					
					if (udpListener->IsWorking()) udpListener->EndSocket();
					
					m_bEnabled = true;
					udpListener->StartSocket(GetPortInt(pActInfo, EIP_Port));

					Execute(pActInfo);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
				}

				if (IsPortActive(pActInfo, EIP_Disable)) {
					m_bEnabled = false;
					udpListener->EndSocket();
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, false);
				}
			}
			break;

			case eFE_Update:
			{
				CTimeValue currTime(gEnv->pTimer->GetCurrTime());
				float delay = 0;  // processing delay
				delay -= (currTime-m_lastTime).GetSeconds();
				m_lastTime = currTime;
				if (delay <= 0.0f)
				{
					Execute(pActInfo);
				}
			}
			break;
		}
	}

	////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidDevice(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				string chaine = udpListener->GetRecMessage();
				string token0 = udpListener->GetToken(0);
				Quat gyro = udpListener->TokenToQuat(token0);
				string token1 = udpListener->GetToken(1);
				Vec3 accel = udpListener->TokenToVec3(token1);
				string token2 = udpListener->GetToken(2);
				float compass = udpListener->TokenToFloat(token2);
				
				/* Attention, changement de repère ! */
				/* Unity vs cryengine */
				//Quat qu;
				//qu.SetRotationX(3.141592/2);
				//gyro = qu * gyro;

				//Matrix33 mm = Matrix33(Vec3(1,0,0),Vec3(0,0,-1),Vec3(0,1,0));
			
				//gyro = gyro.
				//Ajouter une entity et tenter de faire une rotation directe avec le quaternion...

				ActivateOutput(pActInfo, EOP_Gyro_xyz, Vec3(gyro.v.x,gyro.v.y,gyro.v.z));
				ActivateOutput(pActInfo, EOP_Gyro_w, gyro.w);
				ActivateOutput(pActInfo, EOP_Gyro_dir, gyro.GetColumn1());
				
				ActivateOutput(pActInfo, EOP_Accel, accel);
				ActivateOutput(pActInfo, EOP_Compass, compass);
				
				bResult = true;
			}
		}
		return;
	}

};

REGISTER_FLOW_NODE("CryVR:HandledDevices:DeviceListener", CryVR_AndroidDevice);

