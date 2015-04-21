//FormatString.cpp
//CString::Format()風関数

#include"../StdAfx.h"
#include"FormatString.h"

namespace sslib{

namespace{
	bool formatStringCore(std::vector<TCHAR>* p_buffer,const TCHAR* fmt,const va_list argp){
		if(fmt!=NULL){
			int buffer_length=_vsctprintf(fmt,argp)+sizeof(TCHAR);
			if(p_buffer->size()<(UINT)buffer_length)p_buffer->resize(buffer_length);
			_vstprintf(&(*p_buffer)[0],
#ifdef _MSC_VER
					   p_buffer->size(),
#endif
					   fmt,argp);
			return true;
		}else{
			return false;
		}
	}
}

tstring format(const TCHAR* fmt,...){
	if(fmt!=NULL){
		std::vector<TCHAR> buffer(512);
		va_list argp;
		va_start(argp,fmt);

		formatStringCore(&buffer,fmt,argp);

		va_end(argp);
		return &buffer[0];
	}
	return _T("");
}

tstring format(const TCHAR* fmt,const va_list argp){
	if(fmt!=NULL){
		std::vector<TCHAR> buffer(512);

		formatStringCore(&buffer,fmt,argp);
		return &buffer[0];
	}
	return _T("");
}

//namespace sslib
}
