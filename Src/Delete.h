//Delete.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r33 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _DELETE_H_7FA67ED9_665F_46f3_8D09_3146CF2E9A44
#define _DELETE_H_7FA67ED9_665F_46f3_8D09_3146CF2E9A44

#include"recesBase.h"



class Delete:public RecesBase{
public:
	Delete(){}
	~Delete(){}
public:
	RecesBase::ARC_RESULT operator()(const tstring& arc_path,tstring& err_msg);
};
#endif //_DELETE_H_7FA67ED9_665F_46f3_8D09_3146CF2E9A44
