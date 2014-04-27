//VariableArgument.cpp
//va_xxx()の利用を少しだけ便利にするクラス

#include"../StdAfx.h"
#include"VariableArgument.h"

namespace sslib{

namespace{
	bool variableArgumentCore(std::vector<TCHAR>* p_buffer,const TCHAR* format,const va_list argp){
		if(format!=NULL){
			int buffer_length=_vsctprintf(format,argp)+sizeof(TCHAR);
			if(p_buffer->size()<(UINT)buffer_length)p_buffer->resize(buffer_length);
			_vstprintf(&(*p_buffer)[0],format,argp);
			return true;
		}else{
			return false;
		}
	}
}

VariableArgument::VariableArgument(const TCHAR* format,const va_list argp):
	m_buffer(1024){
	variableArgumentCore(&m_buffer,format,argp);
}

VariableArgument::VariableArgument(const TCHAR* format,...):
	m_buffer(1024){
	if(format!=NULL){
		va_list argp;
		va_start(argp,format);

		variableArgumentCore(&m_buffer,format,argp);

		va_end(argp);
	}
}

void VariableArgument::add(const TCHAR* format,...){
	std::vector<TCHAR> buffer(1024);

	if(format!=NULL){
		va_list argp;
		va_start(argp,format);

		variableArgumentCore(&buffer,format,argp);

		va_end(argp);

		m_buffer.insert(find(m_buffer.begin(),m_buffer.end(),'\0'),
						buffer.begin(),
						buffer.end());
	}
}

VariableArgument::~VariableArgument(){}

//namespace sslib
}
