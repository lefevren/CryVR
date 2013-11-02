#include "StdAfx.h"

typedef double TimeStamp ; // in seconds
static const TimeStamp UndefinedTime = -1.0 ;


class LowPassFilter {
public:    
  double y, a, s ;
  bool initialized ;

  void setAlpha(double alpha) {
    if (alpha<=0.0 || alpha>1.0) {
		CryLogAlways("Alpha invalide : [%f] -> [0.5] par defaut",alpha);
		a = 0.5;
	}
    a = alpha ;
  };

  LowPassFilter(double alpha, double initval=0.0) {
	CryLogAlways("Creation du filtre");
	y = s = initval ;
	setAlpha(alpha) ;
	initialized = false ;
  };

  double filter(double value) {
    double result ;
    if (initialized)
      result = a*value + (1.0-a)*s ;
    else {
      result = value ;
      initialized = true ;
    }
    y = value ;
    s = result ;
    return result ;
  };

  double filterWithAlpha(double value, double alpha) {
    setAlpha(alpha) ;
    return filter(value) ;
  };

  bool hasLastRawValue(void) {return initialized ;};
  double lastRawValue(void) {return y ;};
};