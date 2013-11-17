/* TouchDevices node - for licensing and copyright see license.txt */
//Message type : [CryVR][id:NewState][id:NewState]...
				
#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidButton : public CFlowBaseNode<eNCT_Instanced>
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
		EOP_button0,
		EOP_button1,
		EOP_button2,
		EOP_button3,
		EOP_button4,
		EOP_button5,
		EOP_button6,
		EOP_button7,
		EOP_button8,
		EOP_button9,
		EOP_button10,
		EOP_button11,
	};
	
	UdpListener* udpListener;
	bool m_bEnabled;
	CTimeValue m_lastTime;

	bool* tab_state;


public:
	////////////////////////////////////////////////////
	CryVR_AndroidButton(SActivationInfo *pActInfo)
	{
		udpListener = new UdpListener();
		tab_state = new bool[12];
	}

	
	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidButton(void)
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
			InputPortConfig<int>("Port", 26010, _HELP("Port number"), 0,0),
			{0},
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("UDP socket successfully opened for listening")), 
			OutputPortConfig<bool>("Button0", _HELP("First Button0 activation")),
			OutputPortConfig<bool>("Button1", _HELP("First Button1 activation")),
			OutputPortConfig<bool>("Button2", _HELP("First Button2 activation")),
			OutputPortConfig<bool>("Button3", _HELP("First Button3 activation")),
			OutputPortConfig<bool>("Button4", _HELP("First Button4 activation")),
			OutputPortConfig<bool>("Button5", _HELP("First Button5 activation")),
			OutputPortConfig<bool>("Button6", _HELP("First Button6 activation")),
			OutputPortConfig<bool>("Button7", _HELP("First Button7 activation")),
			OutputPortConfig<bool>("Button8", _HELP("First Button8 activation")),
			OutputPortConfig<bool>("Button9", _HELP("First Button9 activation")),
			OutputPortConfig<bool>("Button10", _HELP("First Button10 activation")),
			OutputPortConfig<bool>("Button11", _HELP("First Button11 activation")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve buttons data as bool value");
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidButton(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				
				int buttons = udpListener->GetTokenCount();
				
				for (int i=0;i<buttons;i++){
					string token = udpListener->GetToken(i);
					//CryLogAlways("%s",token);
					int id = udpListener->GetId(token);
					bool state = udpListener->TokenToBool(token);
					if(id>=12) {
						CryLogAlways("Index array out of bounds");
						break;
					}
					if(tab_state[id] != state){
						tab_state[id] = state;
						ActivateOutput(pActInfo,(id+1),state);
					}
				}
				
				bResult = true;
			}
		}
	
		//if (bResult) ActivateOutput(pActInfo, EOP_Received, true);
		return;
	}

};

REGISTER_FLOW_NODE("CryVR:HandledDevices:ButtonListener", CryVR_AndroidButton);

