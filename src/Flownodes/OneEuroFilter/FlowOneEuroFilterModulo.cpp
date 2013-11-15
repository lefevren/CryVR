/* Onde Euro Filter mod float node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"
#include "filters/OneEuroFilter.h"

class CFlowOneEuroFilterModulo : public CFlowBaseNode<eNCT_Instanced>
{
	OneEuroFilter *oef;
	float previous;
	float min;
	float max;
	bool use_modulo;
	int nb_tour;

	enum EInputPorts
	{
		ACTIVE,
		IN_Value,
		IN_USE_MODULO,
		IN_Min,
		IN_Max,
	};

	enum EOutputPorts
	{
		OUT_Value,
		OUT_CumValue,
	};

public:
	
	bool active;

	////////////////////////////////////////////////////
	CFlowOneEuroFilterModulo(SActivationInfo *pActInfo){}
	////////////////////////////////////////////////////
	virtual ~CFlowOneEuroFilterModulo(void){}
	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		 static const SInputPortConfig inputs[] =
            {
				InputPortConfig<bool> ( "Active", true, _HELP( "Activation" ) ),
				InputPortConfig<float> ( "Value", 1.0, _HELP( "Value to filter" ) ),
                InputPortConfig<bool> ( "UseModulo", true, _HELP( "Activation" ) ),
				InputPortConfig<float> ( "Min", 1.0, _HELP( "Min mod valuer" ) ),
                InputPortConfig<float> ( "Max", 1.0, _HELP( "Max mod value" ) ),
                {0},
            };

            // Define output ports here, in same order as EOutputPorts
            static const SOutputPortConfig outputs[] =
            {
                OutputPortConfig<float> ( "FilteredValue", _HELP( "Filtered value" ) ),
                OutputPortConfig<float> ( "CumulatedValue", _HELP( "Cumulated Filtered value" ) ),
                {0},
            };

            // Fill in configuration
            config.pInputPorts = inputs;
            config.pOutputPorts = outputs;
            config.sDescription = _HELP( "FG node that filter a value with OneEuro filter" );
            config.SetCategory( EFLN_APPROVED );
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{	
		switch (event)
		{
		case eFE_Initialize:
			{
				oef = new OneEuroFilter ();
				CryLogAlways("*** ModFloatFilter : Try to use quaternion filter if using angle input *** ");
			}
			break;
		case eFE_Activate:
			{
				if ( IsPortActive( pActInfo, ACTIVE ) ){
					active = GetPortBool( pActInfo, ACTIVE);
					use_modulo = GetPortBool( pActInfo, IN_USE_MODULO);
					
					min = GetPortFloat( pActInfo, IN_Min);
					max = GetPortFloat( pActInfo, IN_Max);
					nb_tour = 0;
					
					previous = GetPortFloat( pActInfo, IN_Value);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,true);
				}
			}
			break;
		
		case eFE_Update:
			{
				if(!active) return;

				oef->increaseTimeStamp(1.0/oef->getFrequence());
				float valeur =  GetPortFloat(pActInfo, IN_Value);
				
				float filtered = 0;//(float)oef->filter(valeur) ;

				if(use_modulo){
					if (abs(previous-valeur) > abs(max-min)/2){
						CryLogAlways("Bascule : %f : [%f] %f : %f",(valeur + abs(max-min) * nb_tour),nb_tour, previous , valeur);
						if(previous < valeur) {
							filtered = (float)oef->filter(valeur + abs(max-min) * (nb_tour-1));
							nb_tour--;
						}
						else {
							filtered = (float)oef->filter(valeur + abs(max-min) * (nb_tour+1));
							nb_tour++;
						}
					}
					else filtered = (float)oef->filter(valeur + abs(max-min) * nb_tour);
				}
				else filtered = (float)oef->filter(valeur) ;

				previous = valeur;
				//float val = filtered % 5;
				
				/* todo remettre la valeur dans l'intervale */
				ActivateOutput(pActInfo, OUT_Value,filtered);
				
				ActivateOutput(pActInfo, OUT_CumValue,filtered);
			}
		}
	}

	////////////////////////////////////////////////////
	virtual void GetMemoryStatistics(ICrySizer *s){
		s->Add(*this);
	}

	////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
	{
		return new CFlowOneEuroFilterModulo(pActInfo);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}
};


REGISTER_FLOW_NODE("CryVR:Filters:OneEuroFilterModFloat", CFlowOneEuroFilterModulo);
