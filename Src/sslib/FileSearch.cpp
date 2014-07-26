//FileSearch.cpp
//ファイル検索

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{


//最初のファイルの情報を取得
WIN32_FIND_DATA* FileSearch::first(const TCHAR* search_dir,const TCHAR* wildcard){
	close();

	if(wildcard==NULL){
		//ファイル情報取得用
		tstring search_path(path::removeTailSlash(search_dir));

		m_handle=::FindFirstFileEx(path::addLongPathPrefix(search_path).c_str(),FindExInfoStandard,&m_file_data,FindExSearchNameMatch,NULL,0);
	}else if(search_dir!=NULL&&path::isDirectory(search_dir)){
		m_base_path=path::addTailSlash(search_dir);

		tstring search_path(m_base_path);
		search_path+=_T("*");
		//ワイルドカードは自前で処理
		m_wildcard=wildcard;

		m_handle=::FindFirstFileEx(path::addLongPathPrefix(search_path).c_str(),FindExInfoStandard,&m_file_data,FindExSearchNameMatch,NULL,0);
	}

	while(lstrcmp(m_file_data.cFileName,_T("."))==0||lstrcmp(m_file_data.cFileName,_T(".."))==0||
		  //ワイルドカードは自前で処理
		  !str::matchWildcards(m_file_data.cFileName,m_wildcard.c_str())){
		if(!::FindNextFile(m_handle,&m_file_data)){
			close();
			break;
		}
	}
	return (m_handle!=INVALID_HANDLE_VALUE)?&m_file_data:NULL;
}

//次のファイルの情報を取得
WIN32_FIND_DATA* FileSearch::next(){
	if(m_handle==INVALID_HANDLE_VALUE||
	   m_wildcard.empty())return NULL;

	if(m_first){
		m_first=false;
		return &m_file_data;
	}

	if(!::FindNextFile(m_handle,&m_file_data))return NULL;

	while(lstrcmp(m_file_data.cFileName,_T("."))==0||lstrcmp(m_file_data.cFileName,_T(".."))==0||
		  //ワイルドカードは自前で処理
		  !str::matchWildcards(m_file_data.cFileName,m_wildcard.c_str())){
		if(!::FindNextFile(m_handle,&m_file_data)){
			return NULL;
		}
	}
	return &m_file_data;
}

//ファイルの検索を終了
bool FileSearch::close(){
	bool result=false;

	m_first=true;

	if(m_handle!=INVALID_HANDLE_VALUE){
		result=::FindClose(m_handle)!=0;
	}
	m_handle=INVALID_HANDLE_VALUE;
	return result;
}



//namespace sslib
}
