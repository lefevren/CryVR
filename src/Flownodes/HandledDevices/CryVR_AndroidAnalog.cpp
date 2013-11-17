/* Analog device node - for licensing and copyright see license.txt */
// [CryVR][id:newX]*

#include "StdAfx.h"

//#include "process.h"
#include "Android/UdpListener.h"
#include "Nodes/G2FlowBaseNode.h"

////////////////////////////////////////////////////
class CryVR_AndroidAnalog : public CFlowBaseNode<eNCT_Instanced>
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
		EOP_Analog0,
		EOP_Analog1,
		EOP_Analog2,
		EOP_Analog3,
		EOP_Analog4,
		EOP_Analog5,
		EOP_Analog6,
		EOP_Analog7,
		EOP_Analog8,
		EOP_Analog9,
		
	};
	
	UdpListener* udpListener;
	bool m_bEnabled;
	float* current_values;
	CTimeValue m_lastTime;

public:
	////////////////////////////////////////////////////
	CryVR_AndroidAnalog(SActivationInfo *pActInfo)
	{
		
		udpListener = new UdpListener();
	}

	
	////////////////////////////////////////////////////
	virtual ~CryVR_AndroidAnalog(void)
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
			OutputPortConfig<float>("Analog0", _HELP("1st analog value")),
			OutputPortConfig<float>("Analog1", _HELP("2nd analog value")),
			OutputPortConfig<float>("Analog2", _HELP("3rd analog value")),
			OutputPortConfig<float>("Analog3", _HELP("4th analog value")),
			OutputPortConfig<float>("Analog4", _HELP("5th analog value")),
			OutputPortConfig<float>("Analog5", _HELP("6th analog value")),
			OutputPortConfig<float>("Analog6", _HELP("7th analog value")),
			OutputPortConfig<float>("Analog7", _HELP("8th analog value")),
			OutputPortConfig<float>("Analog8", _HELP("9th analog value")),
			OutputPortConfig<float>("Analog9", _HELP("10th analog value")),
			{0},
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Opens a UDP listener and retrieve up to 10 analog data");
		//config.SetCategory(EFLN_ADVANCED);
	}

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		switch (event)
		{
		case eFE_Initialize : 
			{
				current_values = new float[5];
				for(int i = 0 ;i<5;i++) current_values[i]= 0;
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
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_AndroidAnalog(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}
	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}


	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		if (udpListener->IsWorking()) {
			if (udpListener->ReceiveLine() != -1) {
				
				int analogs = udpListener->GetTokenCount();
				CryLogAlways("Nombre d'event %i",analogs);
				for(int i=0;i<analogs;i++){
					string token = udpListener->GetToken(i);
					float value = udpListener->TokenToFloat(token);
					
					int id = udpListener->GetId(token);
					if(id >= 10) {
						CryLogAlways("Error index out of bounds : token %s ",token);
						return;
					}

					if(current_values[id]!=value){
						current_values[id] = value;
						ActivateOutput(pActInfo,id+1,value);
					}
				}
				bResult = true;
			}
		}
		return;
	}
};

REGISTER_FLOW_NODE("CryVR:HandledDevices:AnalogListener", CryVR_AndroidAnalog);

