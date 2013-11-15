/* Auler to Quat converter node - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Actor.h"


class CryVR_EulerToQuat : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_Get,
		EIP_Euler,
		EIP_Radian,
		
	};

	enum EOutputPorts
	{
		EOP_XYZ,
		EOP_W,
	};

public:
	
	////////////////////////////////////////////////////
	CryVR_EulerToQuat(SActivationInfo *pActInfo){}
	
	////////////////////////////////////////////////////
	virtual ~CryVR_EulerToQuat(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool> ("Get",_HELP("Get quaternion")),
			InputPortConfig<Vec3>("EulerAngle",Vec3(0,0,0), _HELP("Euler angle to convert")),
			InputPortConfig<bool> ("Radians",true,_HELP("Radian ?")),
			
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("Quat_XYZ", _HELP("Quaternion XYZ part")),
			OutputPortConfig<float>("Quat_W", _HELP("Quaternion W part")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node that convert an Euler Angle to a quaternion");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		switch (event)
		{
			case eFE_Initialize:
				{
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,false);
				}
			case eFE_Activate:
				{
					if(IsPortActive(pActInfo, 0)) 
					{
						Vec3 v = GetPortVec3(pActInfo,1);
						
						if (GetPortBool(pActInfo,2)){
							const Ang3 an = Ang3(v.x, v.y, v.z); 
						
							Quat q = Quat(an);
						
							ActivateOutput(pActInfo, EOP_XYZ, Vec3(q.v.x,q.v.y,q.v.z));
							ActivateOutput(pActInfo, EOP_W, q.w);
						}
						else {
							float mpi = 3.141592/180;
							
							const Ang3 an = Ang3(v.x*mpi,v.y*mpi, v.z*mpi); 
							Quat q = Quat(an);
						
							ActivateOutput(pActInfo, EOP_XYZ, Vec3(q.v.x,q.v.y,q.v.z));
							ActivateOutput(pActInfo, EOP_W, q.w);
						}
					}
				}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_EulerToQuat(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


};


class CryVR_QuatToDir : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_Get,
		EIP_Quat_XYZ,
		EIP_Quat_W,
		
	};

	enum EOutputPorts
	{
		EOP_dir,
		
	};

public:
	
	////////////////////////////////////////////////////
	CryVR_QuatToDir(SActivationInfo *pActInfo){}
	
	////////////////////////////////////////////////////
	virtual ~CryVR_QuatToDir(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool> ("Get",_HELP("Get direction")),
			InputPortConfig<Vec3>("Quat_XYZ", _HELP("Quaternion XYZ part")),
			InputPortConfig<float> ("Quat_W",_HELP("Quaternion W part")),
			
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("Direction", _HELP("Vec3 direction")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node that convert quaternion to a direction");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		switch (event)
		{
			case eFE_Initialize:
				{
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,false);
				}
			case eFE_Activate:
				{
					if(IsPortActive(pActInfo, 0)) 
					{
						Vec3 v = GetPortVec3(pActInfo,1);
						float w = GetPortFloat(pActInfo,2);
						Quat q = Quat(w,v.x,v.y,v.z);
						Vec3 dir = q.GetColumn1();

						ActivateOutput(pActInfo, EOP_dir, dir);

					}
				}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_QuatToDir(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


};




class CryVR_QuatToEuler : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		EIP_Get,
		EIP_Quat_XYZ,
		EIP_Quat_W,
		
	};

	enum EOutputPorts
	{
		EOP_Euler,
		
	};

public:
	
	////////////////////////////////////////////////////
	CryVR_QuatToEuler(SActivationInfo *pActInfo){}
	
	////////////////////////////////////////////////////
	virtual ~CryVR_QuatToEuler(void){}

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser){}

	////////////////////////////////////////////////////
	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<bool> ("Get",_HELP("Get direction")),
			InputPortConfig<Vec3>("Quat_XYZ", _HELP("Quaternion XYZ part")),
			InputPortConfig<float> ("Quat_W",_HELP("Quaternion W part")),
			
			{0},
		};

		// Define output ports here, in same order as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("Euler", _HELP("Euler angle")),
			{0},
		};
		
		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("CryVR node that convert quaternion to an Euler angle");
		config.SetCategory(EFLN_APPROVED);
	}

	


	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		
		switch (event)
		{
			case eFE_Initialize:
				{
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID,false);
				}
			case eFE_Activate:
				{
					if(IsPortActive(pActInfo, 0)) 
					{
						Vec3 v = GetPortVec3(pActInfo,1);
						float w = GetPortFloat(pActInfo,2);
						Quat q = Quat(w,v.x,v.y,v.z);
						
						Ang3 an = Ang3(q);

						ActivateOutput(pActInfo, EOP_Euler, Vec3(an.x,an.y,an.z));

					}
				}
			}
	}

	virtual void GetMemoryStatistics(ICrySizer *s){s->Add(*this);}
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo){return new CryVR_QuatToEuler(pActInfo);}
	virtual void GetMemoryUsage(ICrySizer * s) const{s->Add(*this);}


};

REGISTER_FLOW_NODE("CryVR:Maths:QuatToEuler", CryVR_QuatToEuler);
REGISTER_FLOW_NODE("CryVR:Maths:QuatToDir", CryVR_QuatToDir);
REGISTER_FLOW_NODE("CryVR:Maths:EulerToQuat", CryVR_EulerToQuat);
