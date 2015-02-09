//Test.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _TEST_H_B11C7969_347E_4058_9525_0B3B3B519899
#define _TEST_H_B11C7969_347E_4058_9525_0B3B3B519899

#include"recesBase.h"



class Test:public RecesBase{
public:
	Test(){}
	~Test(){}
public:
	RecesBase::ARC_RESULT operator()(const tstring& arc_path,tstring& err_msg);
};
#endif //_TEST_H_B11C7969_347E_4058_9525_0B3B3B519899
