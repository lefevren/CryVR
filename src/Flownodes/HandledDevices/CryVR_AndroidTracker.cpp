/* TouchDevices node - for licensing and copyright see license.txt */
// [CryVR][id:x,y,z]*

#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidTracker : public CFlowBaseNode<eNCT_Instanced>
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
	Vec3* tab_state ;

public:
	////////////////////////////////////////////////////
	CryVR_AndroidTracker(SActivationInfo *pActInfo)
	{
		tab_state = new Vec3[5];
		udpListener = new UdpListener();
	}

	
	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidTracker(void)
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
			OutputPortConfig<Vec3>("Tracker0", _HELP("1st tracker value")),
			OutputPortConfig<Vec3>("Tracker1", _HELP("2nd tracker value")),
			OutputPortConfig<Vec3>("Tracker2", _HELP("3rd tracker value")),
			OutputPortConfig<Vec3>("Tracker3", _HELP("4th tracker value")),
			OutputPortConfig<Vec3>("Tracker4", _HELP("5th tracker value")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve tracker (x,y,z) data");
		//config.SetCategory(EFLN_ADVANCED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		switch (event)
		{
		case eFE_Initialize : {
							  for (int i=0;i<5;i++) tab_state[i] = Vec3(0,0,0);
							  break;
							  }
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidTracker(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				
				int fingers = udpListener->GetTokenCount();
				
				for(int i=0;i<fingers;i++){
					string token = udpListener->GetToken(i);
					Vec3 pos = udpListener->TokenToVec3(token);
					int id = udpListener->GetId(token);
					
					
					if(tab_state[id]!=pos) {
						tab_state[id] = pos;
						ActivateOutput(pActInfo,id+1,pos);
					}
				}
				bResult = true;
			}
		}
		return;
	}

};

REGISTER_FLOW_NODE("CryVR:HandledDevices:TrackerListener", CryVR_AndroidTracker);

