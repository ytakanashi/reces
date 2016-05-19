//Rename.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r32 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _RENAME_H_8E64FD99_AC88_48a1_B11A_3EA7235A35A8
#define _RENAME_H_8E64FD99_AC88_48a1_B11A_3EA7235A35A8

#include"recesBase.h"



class Rename:public RecesBase{
public:
	Rename(){}
	~Rename(){}
private:
	bool addPattern();
public:
	RecesBase::ARC_RESULT operator()(const tstring& arc_path,tstring& err_msg);
};
#endif //_RENAME_H_8E64FD99_AC88_48a1_B11A_3EA7235A35A8
