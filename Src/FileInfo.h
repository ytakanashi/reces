//FileInfo.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r28 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _FILEINFO_H_65AAF62F_5C38_4C38_9994_5310C9D26263
#define _FILEINFO_H_65AAF62F_5C38_4C38_9994_5310C9D26263


namespace fileinfo{
struct FILEINFO{
	tstring name;
	long long size;
	DWORD attr;
	long long date_time;

	FILEINFO():name(),size(0),attr(0),date_time(-1){}
	FILEINFO(const tstring& name_):name(name_),size(0),attr(0),date_time(0){}
	FILEINFO(const tstring& name_,long long size_,DWORD attr_,long long date_time_):name(name_),size(size_),attr(attr_),date_time(date_time_){}
	bool operator()(const FILEINFO& fileinfo)const{
		return name==fileinfo.name;
	}
	bool operator<(const FILEINFO& fileinfo)const{
		return name<fileinfo.name;
	}
	//ディレクトリ若しくはサブ以下のファイル
	bool isDirectory(bool delimiter_check=true)const{
		return ((delimiter_check)?
				 (name.find_last_of(_T("\\/"))==name.length()-1):
				false)||
				 attr&FILE_ATTRIBUTE_DIRECTORY;
	}
};

struct FILEFILTER{
	long long min_size;
	long long max_size;
	DWORD attr;
	//除外フィルタ時のみ操作
	bool include_empty_dir;
	long long oldest_date;
	long long newest_date;
	std::list<tstring> pattern_list;
	bool recursive;
	bool regex;

	FILEFILTER():min_size(0),max_size(0),attr(0),include_empty_dir(true),oldest_date(-1),newest_date(-1),pattern_list(),recursive(false),regex(false){}
	bool empty()const{return min_size==0&&
							 max_size==0&&
							 attr==0&&
							 include_empty_dir&&
							 oldest_date==-1&&
							 newest_date==-1&&
							 pattern_list.empty();}
};

//ファイルの情報をFILEINFO構造体で取得
bool getFileInfo(FILEINFO* fileinfo,const TCHAR* file_path_orig);
//ファイルの情報をFILEINFO構造体で取得
bool getFileInfo(FILEINFO* fileinfo,const WIN32_FIND_DATA& file_data,const TCHAR* file_path_orig);

//フィルタ(FILEFILTER構造体)にマッチするか
bool matchPattern(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const TCHAR* base_dir=NULL);
bool matchExPattern(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir=NULL);
bool matchRegex(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter);
bool matchExRegex(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& file_ex_filter);
bool matchSize(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);
bool matchDateTime(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);
bool matchAttribute(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);

//注意:file_pathではなくフルパスでマッチ判断
bool matchFilters(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir=NULL);
bool matchFilters(const TCHAR* file_path,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir=NULL);


//namespace fileinfo
}


class FileTree{
struct FILEINFONODE{
	fileinfo::FILEINFO fileinfo;

	FILEINFONODE* parent_dir;
	FILEINFONODE* child;
	FILEINFONODE* next;

	bool disable;
	//disableExternalParent()で使用
	bool disable_checked;
	FILEINFONODE():fileinfo(),parent_dir(NULL),child(NULL),next(NULL),disable(false),disable_checked(false){}
};

public:
	enum TREE_OPTION{
		TO_NONE=0,
		//ディレクトリなら除外
		TO_EXCLUDE_DIR=1<<0,
		TO_ALL=TO_EXCLUDE_DIR
	};
	FileTree():m_tree(NULL){}
	FileTree(const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter):
		m_tree(NULL),
		m_filefilter(filefilter),
		m_file_ex_filter(file_ex_filter){}
	virtual ~FileTree(){destroy(&m_tree);m_tree=NULL;}

private:
	FILEINFONODE* m_tree;
	fileinfo::FILEFILTER m_filefilter;
	fileinfo::FILEFILTER m_file_ex_filter;

private:
	bool addV(const TCHAR* name,const fileinfo::FILEINFO* fileinfo=NULL);
	bool addV(FILEINFONODE**pp,const TCHAR* parent,const TCHAR* name,const fileinfo::FILEINFO* fileinfo=NULL);

	bool delNode(FILEINFONODE**pp);
	bool delV(FILEINFONODE**pp,const TCHAR* name);
#if 0
	int countSibling(FILEINFONODE**pp);
#endif
	bool isEmptyDirectory(FILEINFONODE**pp);

	void disableLine(FILEINFONODE**pp);
	void makeIncludeTree(FILEINFONODE** pp,DWORD options=TO_NONE,const TCHAR* base_dir=NULL);
	void makeExcludeTree(FILEINFONODE** pp,DWORD options=TO_NONE,const TCHAR* base_dir=NULL);

	void disableExternalParent(FILEINFONODE** pp,bool reverse=false);

	void tree2list(FILEINFONODE**pp,std::vector<fileinfo::FILEINFO>& list);

#ifdef _DEBUG
	void msgTree(FILEINFONODE** pp);
#endif

public:
	bool add(const TCHAR* parent,const TCHAR* name,const fileinfo::FILEINFO* fileinfo=NULL);
	bool add(const TCHAR* name,const fileinfo::FILEINFO* fileinfo=NULL);
	bool del(const TCHAR* name);
	void destroy(FILEINFONODE**pp);
#if 0
	int countContents(int depth);
#endif
	bool createFileTree(const TCHAR* search_dir_orig,const TCHAR* wildcard=_T("*"),bool include_dir=false);
	void makeIncludeTree(DWORD options=TO_NONE,const TCHAR* base_dir=NULL);
	void makeExcludeTree(DWORD options=TO_NONE,const TCHAR* base_dir=NULL);
	bool tree2list(std::vector<fileinfo::FILEINFO>& list);

#ifdef _DEBUG
	void msgTree();
#endif

};

#endif //_FILEINFO_H_65AAF62F_5C38_4C38_9994_5310C9D26263
