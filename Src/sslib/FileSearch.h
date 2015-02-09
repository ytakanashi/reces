//FileSearch.h

#ifndef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
#define _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3



namespace sslib{
class FileSearch{
public:
	FileSearch():
		m_handle(INVALID_HANDLE_VALUE),
		m_file_data(),
		m_base_path(),
		m_wildcard(),
		m_wildcard_list(),
		m_first(true){
	}
	virtual ~FileSearch(){close();}

protected:
	HANDLE m_handle;
	WIN32_FIND_DATA m_file_data;
	tstring m_base_path;
	tstring m_wildcard;
	std::list<tstring> m_wildcard_list;
	bool m_first;

private:
	bool match(const TCHAR* file_path);

public:
	//最初のファイルの情報を取得
	WIN32_FIND_DATA* first(const TCHAR* search_dir,const TCHAR* wildcard=_T("*"));

	//次のファイルの情報を取得
	WIN32_FIND_DATA* next();

	//ファイルの検索を終了
	bool close();

	inline const WIN32_FIND_DATA* data()const{
		return &m_file_data;
	}

	//ファイルのフルパスを取得
	inline const tstring filepath()const{
		return m_base_path+m_file_data.cFileName;
	}

	//ファイルのサイズを取得
	inline long long getSize()const{
		return (long long)MAKEQWORD(m_file_data.nFileSizeHigh,m_file_data.nFileSizeLow);
	}

	inline bool isDirectory()const{return (m_file_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;}
	inline bool isHiddenFile()const{return (m_file_data.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)!=0;}
	inline bool isSystemFile()const{return (m_file_data.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM)!=0;}
	inline bool hasAttribute(DWORD attr)const{return (m_file_data.dwFileAttributes&attr)!=0;}
};

//namespace sslib
}

#endif //_FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
