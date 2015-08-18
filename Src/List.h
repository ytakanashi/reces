//List.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r29 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _LIST_H_16F1B33D_22D3_41f5_9339_A48E7F86337B
#define _LIST_H_16F1B33D_22D3_41f5_9339_A48E7F86337B

#include"recesBase.h"



class List:public RecesBase{
public:
	List(){}
	~List(){}
public:
	RecesBase::ARC_RESULT operator()(const tstring& arc_path,tstring& err_msg);
};
#endif //_LIST_H_16F1B33D_22D3_41f5_9339_A48E7F86337B
