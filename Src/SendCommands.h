//SendCommands.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _SENDCOMMANDS_H_E7CDF602_72BD_4939_8CFD_FF122553385D
#define _SENDCOMMANDS_H_E7CDF602_72BD_4939_8CFD_FF122553385D

#include"recesBase.h"



class SendCommands:public RecesBase{
public:
	SendCommands(){}
	~SendCommands(){}
public:
	RecesBase::ARC_RESULT operator()(std::list<tstring>& commands_list,tstring& err_msg);
};
#endif //_SENDCOMMANDS_H_E7CDF602_72BD_4939_8CFD_FF122553385D
