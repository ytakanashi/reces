//EnvironmentVariable.cpp
//環境変数の設定/取得

#include"../StdAfx.h"
#include"EnvironmentVariable.h"

namespace sslib{
namespace env{

bool get(const TCHAR* name,tstring* result){
	LPTSTR v=NULL;
	DWORD buffer_size=0;

	if(name==NULL||result==NULL)return false;

	if((buffer_size=::GetEnvironmentVariable(name,v,0))!=0){
		std::vector<TCHAR> buffer(buffer_size);

		DWORD num_of_chars=::GetEnvironmentVariable(name,&buffer[0],buffer_size);

		result->assign(&buffer[0]);
		return num_of_chars!=0;
	}
	return false;
}

//namespace env
}
//namespace sslib
}
