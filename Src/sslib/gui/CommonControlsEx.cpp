//CommonControlsEx.cpp
//CommonControlsの初期化


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{

bool CommonControlsEx::init(DWORD icc){
	INITCOMMONCONTROLSEX ic={};

	ic.dwICC=icc;
	ic.dwSize=sizeof(INITCOMMONCONTROLSEX);
	return !!::InitCommonControlsEx(&ic);
}

//namespace sslib
}
