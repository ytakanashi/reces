//EnvironmentVariable.h

#ifndef _ENVIRONMENTVARIABLE_H_7A770496_4FB2_4c25_807C_78846C6A11E6
#define _ENVIRONMENTVARIABLE_H_7A770496_4FB2_4c25_807C_78846C6A11E6



namespace sslib{
namespace env{

bool get(const TCHAR* name,tstring* result);

inline bool set(const TCHAR* name,const TCHAR* value){
	return ::SetEnvironmentVariable(name,value)!=0;
}

inline bool del(const TCHAR* name){
	return ::SetEnvironmentVariable(name,NULL)!=0;
}

inline bool exist(const TCHAR* name){
	tstring str;

	return get(name,&str);
}

//namespace env
}
//namespace sslib
}

#endif //_ENVIRONMENTVARIABLE_H_7A770496_4FB2_4c25_807C_78846C6A11E6
