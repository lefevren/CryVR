/* Onde Euro Filter vec2 node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "Kali/kali.h"


class CFlowKaliSender : public CFlowBaseNode<eNCT_Instanced>
{
	//OneEuroFilter *oef_x;
	//OneEuroFilter *oef_y;
	
	enum EInputPorts
	{
		ACTIVE,
		//IN_Value,
		
	};

	enum EOutputPorts
	{
		OUT_Value,
	};

public:
	bool active;

	////////////////////////////////////////////////////
	CFlowKaliSender(SActivationInfo *pActInfo){}
	////////////////////////////////////////////////////
	virtual ~CFlowKaliSender(void){}
	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		 // Define input ports here, in same order as EInputPorts
            static const SInputPortConfig inputs[] =
            {
                InputPortConfig<bool> ( "Activate", _HELP( "Activation" ) ),
                //InputPortConfig<string> ( "Value", "default value", _HELP( "Value to filter" ) ),
                {0},
            };

            // Define output ports here, in same order as EOutputPorts
            static const SOutputPortConfig outputs[] =
            {
                OutputPortConfig<bool> ( "Done", _HELP( "Filtered value" ) ),
                {0},
            };

            // Fill in configuration
            config.pInputPorts = inputs;
            config.pOutputPorts = outputs;
            config.sDescription = _HELP( "CryVR node that send char text to Kali" );
            config.SetCategory( EFLN_APPROVED );
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{	
		switch (event)
		{
		case eFE_Initialize:
			{
				CryLogAlways("Configuration de Kali");
				SetModeLectureKali(0);
				SetComptaKali(0);
				SetEpelleKali(0);
				SetSyntaxeKali(1);
				SetProsodieKali(8);
				SetCoefPauseKali(4);
				SetLocalisationKali(50);
			}
			break;
		case eFE_Activate:
			{

				if (QueryIndexKali()>0) return;

				if ( IsPortActive( pActInfo, ACTIVE ) ){
					//active = GetPortBool( pActInfo, ACTIVE);
					
					char * mon_texte = (char*)malloc(128*sizeof(char)+8*sizeof(f32)) ;
					mon_texte = "carabo   ";
					CryLogAlways("Emission de son");


					if(!(QueryIndexKali()>0)) MessageKali((unsigned char *)mon_texte);
					
					active = true;
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
					CryLogAlways("Fin d'Emission de son");
					// Ici, on lance un appel a Kali
					
				}
			}
			break;
		
		case eFE_Update:
			{

				if(!active) return;
			
				
				if (QueryIndexKali()>0) return;
				ExitKali();

					
					
				//MessageKaliStd((unsigned char *)"carabote");
				//MessageKali((unsigned char *)"Carabo");
				
				//Message
				//ExitKali();
				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,false);
				active = false;
			}
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
		return new CFlowKaliSender(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
		
	}


};

REGISTER_FLOW_NODE("CryVR:Kali:KaliSender", CFlowKaliSender);
