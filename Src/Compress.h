//Compress.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _COMPRESS_H_0F5B41E6_F70C_49db_A3EA_EB0437FA060E
#define _COMPRESS_H_0F5B41E6_F70C_49db_A3EA_EB0437FA060E

#include"recesBase.h"



class Compress:public RecesBase{
public:
	Compress(){}
	~Compress(){}
public:
	RecesBase::ARC_RESULT operator()(std::list<tstring>& compress_file_list,tstring& err_msg);
};
#endif //_COMPRESS_H_0F5B41E6_F70C_49db_A3EA_EB0437FA060E
