/* TouchDevices node - for licensing and copyright see license.txt */

#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidJoystick : public CFlowBaseNode<eNCT_Instanced>
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
		EOP_JoyLeft,
		EOP_JoyRight,
		EOP_JoyTop,
		EOP_JoyBottom,
		EOP_button0,
		EOP_button1,
		EOP_button2,
		EOP_button3,
		EOP_button4,
		EOP_button5,
		EOP_button6,
		EOP_button8,
		EOP_button9,
		EOP_button10,
		EOP_button11,
		EOP_finger12,
	};
	
	UdpListener* udpListener;
	bool m_bEnabled;
	CTimeValue m_lastTime;

public:
	////////////////////////////////////////////////////
	CryVR_AndroidJoystick(SActivationInfo *pActInfo)
	{
		udpListener = new UdpListener();
	}

	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidJoystick(void)
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
			{0},
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Status", _HELP("UDP socket successfully opened for listening")), 
			OutputPortConfig<Vec3>("Joystick0", _HELP("First xy value")),
			OutputPortConfig<Vec3>("Joystick1", _HELP("Second xy value")),
			OutputPortConfig<Vec3>("Joystick2", _HELP("Third xy value")),
			OutputPortConfig<Vec3>("Joystick3", _HELP("Fourth xy value")),
			OutputPortConfig<bool>("Button0", _HELP("Button 0")),
			OutputPortConfig<bool>("Button1", _HELP("Button 1")),
			OutputPortConfig<bool>("Button2", _HELP("Button 2")),
			OutputPortConfig<bool>("Button3", _HELP("Button 3")),
			OutputPortConfig<bool>("Button4", _HELP("Button 4")),
			OutputPortConfig<bool>("Button5", _HELP("Button 5")),
			OutputPortConfig<bool>("Button6", _HELP("Button 6")),
			OutputPortConfig<bool>("Button7", _HELP("Button 7")),
			OutputPortConfig<bool>("Button8", _HELP("Button 8")),
			OutputPortConfig<bool>("Button9", _HELP("Button 9")),
			OutputPortConfig<bool>("Button10", _HELP("Button 10")),
			OutputPortConfig<bool>("Button11", _HELP("Button 11")),
			OutputPortConfig<bool>("Button12", _HELP("Button 12")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve joystick data");
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidJoystick(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				
				int fingers = udpListener->GetTokenCount();
				/* todo ! 
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
				*/
				bResult = true;
			}
		}
	
		//if (bResult) ActivateOutput(pActInfo, EOP_Received, true);
		return;
	}

};

REGISTER_FLOW_NODE("CryVR:HandledDevices:JoystickListener", CryVR_AndroidJoystick);

