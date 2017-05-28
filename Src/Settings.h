//Settings.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r33 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _SETTINGS_H_D83E612E_064F_40c5_9498_D10D309A1134
#define _SETTINGS_H_D83E612E_064F_40c5_9498_D10D309A1134

#include"recesBase.h"



class Settings:public RecesBase{
public:
	Settings(){}
	~Settings(){}
public:
	RecesBase::ARC_RESULT operator()(HWND wnd,tstring& err_msg);
};
#endif //_SETTINGS_H_D83E612E_064F_40c5_9498_D10D309A1134
