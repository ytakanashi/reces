//Library.cpp
//ライブラリ読み込み

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

Library::Library():
	m_module(NULL),
	m_library_name(),
	m_library_prefix(){
}

Library::Library(const TCHAR* library_name):
	m_module(NULL),
	m_library_name(library_name),
	m_library_prefix(){
}

Library::Library(const TCHAR* library_name,const TCHAR* library_prefix):
	m_module(NULL),
	m_library_name(library_name),
	m_library_prefix(library_prefix){
}

Library::~Library(){
	unload();
}

//ライブラリを読み込む
bool Library::load(){
	unload();

	if(!m_library_name.empty())return load(m_library_name.c_str(),m_library_prefix.c_str());
	else return false;
}

//ライブラリを読み込む
bool Library::load(const TCHAR* library_name,const TCHAR* library_prefix){
	unload();

	if(library_name==NULL)return load();

	if((m_module=::LoadLibrary(library_name))!=NULL){
		m_library_name=library_name;
		setPrefix(library_prefix);
		return true;
	}
	return false;
}

//ライブラリを解放する
bool Library::unload(){
	if(m_module!=NULL){
		::FreeLibrary(m_module);
		m_module=NULL;
		return true;
	}
	return false;
}

//プレフィックスを設定
void Library::setPrefix(const TCHAR* library_prefix){
	if(library_prefix!=NULL)m_library_prefix=library_prefix;
}

//プレフィックスをクリア
void Library::clearPrefix(){
	m_library_prefix.clear();
}

//関数のアドレスを取得
FARPROC WINAPI Library::getAddress(const TCHAR* proc_name_format,...){
	if(proc_name_format==NULL)return NULL;

	if(!isLoaded()&&!load())return NULL;

	va_list argp;
	va_start(argp,proc_name_format);
	tstring proc_name=format(proc_name_format,argp);
	va_end(argp);

#ifndef UNICODE
	return ::GetProcAddress(m_module,(m_library_prefix+proc_name).c_str());
#else
	return ::GetProcAddress(m_module,str::utf162sjis(m_library_prefix+proc_name).c_str());
#endif
}

//namespace sslib
}
