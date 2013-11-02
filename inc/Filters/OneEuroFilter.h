#include "StdAfx.h"
#include "VR/LowPassFilter.h"


//IConsole* pConsole = gEnv->pSystem->GetIConsole();


class OneEuroFilter
{
	double freq ;
	double mincutoff ;
	double beta_ ;
	double dcutoff ;
	LowPassFilter *x ;
	LowPassFilter *dx ;
	double lasttime ;
	double timestamp ;

public:
	OneEuroFilter(){
		float frequence_ = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_frequence")->GetFVal();
		float beta__ = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_beta")->GetFVal();
		float mincutoff_ = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_mincutoff")->GetFVal();
		
		setFrequency(frequence_) ;
		setMinCutoff(mincutoff_) ;
		setBeta(beta__) ;
		setDerivateCutoff(1.0) ;
		
		init();
		
		
	}

	~OneEuroFilter(){};

	void init(){
		x = new LowPassFilter(alpha(mincutoff)) ;
		dx = new LowPassFilter(alpha(1.0)) ;
		lasttime = UndefinedTime ;
		timestamp = 0.0;
	}

	void updateFilter(){
		//Variable console non mise a jour !

		float frequence = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_frequence")->GetFVal();
		//Inutile pour le moment !
		float beta = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_beta")->GetFVal();
		float mincutoff_ = gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_mincutoff")->GetFVal();
		
		if(beta != beta_ || mincutoff != mincutoff_)
		{
			setMinCutoff(mincutoff_) ;
			setBeta(beta) ;
			init();
		}
		

		//setFrequency(frequence) ;
		//setMinCutoff(mincutoff) ;
		//setBeta(beta) ;
	}

	double filter(double value){
		//Voir la necessité a chaque frame !
		
		updateFilter();
		if (gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_enable")->GetFVal() != 1.0){ 
			//CryLogAlways("Filtres globaux desactives");
			return value;
		}
		if (gEnv->pSystem->GetIConsole()->GetCVar("CryVR_filters_euro_enable")->GetFVal() != 1.0){ 
			//CryLogAlways("Filtre Euro desactive");
			return value;
		}
		else {
			// update the sampling frequency based on timestamps
			if (lasttime!=UndefinedTime  && timestamp!=UndefinedTime )
				freq = 1.0 / (timestamp-lasttime) ;
	
			lasttime = timestamp ;
			double dvalue = x->hasLastRawValue() ? (value - x->lastRawValue())*freq : value ; // FIXME: 0.0 or value?
			double edvalue = dx->filterWithAlpha(dvalue, alpha(dcutoff)) ;
			double cutoff = mincutoff + beta_*fabs(edvalue) ;

			return x->filterWithAlpha(value, alpha(cutoff)) ;
		}
		
	};




	void increaseTimeStamp(double up_){timestamp+=up_;};
	double getTimeStamp(){return timestamp;};
	double getFrequence(){return freq;};
	
	double alpha(double cutoff){
		double te = 1.0 / freq ;
		double tau = 1.0 / (2*3.141592*cutoff) ;
		return 1.0 / (1.0 + tau/te) ;
	};

	void setFrequency(double f){
		if (f<=0) {
			CryLogAlways("Frequence doit etre >0 [%f] -> [abs(%f)]",f) ;
			freq=-f;
		}
		else freq = f ;
	};

	void setMinCutoff(double mc){
		if (mc<=0) {
			if (mc==0){
				CryLogAlways("MinCutOff doit etre >0 [%f] -> [1.0]",mc) ;
				mincutoff = 1.0;
			}
			else {
				CryLogAlways("MinCutOff doit etre >0 [%f] -> [abs(%f)]",mc,mc) ;
				mincutoff = -mc;
			}
		
		}
		else mincutoff = mc ;
	};

	void setBeta(double b){beta_ = b ;};
	
	void setDerivateCutoff(double dc){
		if (dc<=0) {
			if (dc==0){
				CryLogAlways("DerivateCutOff doit etre >0 [%f] -> [1.0]",dc) ;
				dcutoff = 1.0;
			}
			else {
				CryLogAlways("MinCutOff doit etre >0 [%f] -> [abs(%f)]",dc,dc) ;
				dcutoff = -dc;
			}
		}
		else dcutoff = dc ;
	};
};




	