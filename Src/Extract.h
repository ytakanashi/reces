//Extract.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _EXTRACT_H_473E7A70_1A32_4256_BB7D_026667255C37
#define _EXTRACT_H_473E7A70_1A32_4256_BB7D_026667255C37

#include"recesBase.h"



class Extract:public RecesBase{
public:
	Extract(){}
	~Extract(){}
public:
	RecesBase::ARC_RESULT operator()(const tstring& arc_path,tstring& err_msg);
};
#endif //_EXTRACT_H_473E7A70_1A32_4256_BB7D_026667255C37
