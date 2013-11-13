/* TouchDevices node - for licensing and copyright see license.txt */

#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidTouch : public CFlowBaseNode<eNCT_Instanced>
{
	
	enum EInputPorts
	{
		EIP_Enable,
		EIP_Disable,
		EIP_Port, 
		EIP_Relative,
	};

	enum EOutputs
	{
		EOP_Status = 0,
		EOP_finger0,
		EOP_finger1,
		EOP_finger2,
		EOP_finger3,
		EOP_finger4,
		EOP_finger5,
	};
	
	UdpListener* udpListener;
	bool m_bEnabled;
	CTimeValue m_lastTime;

public:
	////////////////////////////////////////////////////
	CryVR_AndroidTouch(SActivationInfo *pActInfo)
	{
		udpListener = new UdpListener();
	}

	
	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidTouch(void)
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
			InputPortConfig<bool>("Disable", _HELP("Enable receiving signals")),
			InputPortConfig<int>("Port", 26001, _HELP("Port number"), 0,0),
			InputPortConfig<bool>("RelativePosition", _HELP("Relative from first sent position ?")),
			{0},
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("UDP socket successfully opened for listening")), 
			OutputPortConfig<Vec3>("Touch0", _HELP("First fingerId value")),
			OutputPortConfig<Vec3>("Touch1", _HELP("Second fingerId value")),
			OutputPortConfig<Vec3>("Touch2", _HELP("Third fingerId value")),
			OutputPortConfig<Vec3>("Touch3", _HELP("Fourth fingerId value")),
			OutputPortConfig<Vec3>("Touch4", _HELP("Fifth fingerId value")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve fingers data");
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidTouch(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				
				int fingers = udpListener->GetTokenCount();
				
				switch (fingers)
				{
				case 5:{
					string token4 = udpListener->GetToken(4);
					Vec3 finger4 = udpListener->TokenToVec3(token4);
					ActivateOutput(pActInfo, EOP_finger3, finger4);
					}
				case 4:{
					string token3 = udpListener->GetToken(3);
					Vec3 finger3 = udpListener->TokenToVec3(token3);
					ActivateOutput(pActInfo, EOP_finger3, finger3);
					}
				case 3:{
					string token2 = udpListener->GetToken(2);
					Vec3 finger2 = udpListener->TokenToVec3(token2);
					ActivateOutput(pActInfo, EOP_finger2, finger2);
					}
				case 2:{
					string token1 = udpListener->GetToken(1);
					Vec3 finger1 = udpListener->TokenToVec3(token1);
					ActivateOutput(pActInfo, EOP_finger1, finger1);
					}

				case 1:{
					string token0 = udpListener->GetToken(0);
					Vec3 finger0 = udpListener->TokenToVec3(token0);
					ActivateOutput(pActInfo, EOP_finger0, finger0);
					}
				default : break;
				}
				bResult = true;
			}
		}
	
		//if (bResult) ActivateOutput(pActInfo, EOP_Received, true);
		return;
	}

};

REGISTER_FLOW_NODE("CryVR:HandledDevices:TouchListener", CryVR_AndroidTouch);

