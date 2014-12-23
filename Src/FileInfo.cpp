//FileInfo.cpp
//ファイル情報の操作

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"FileInfo.h"


using namespace sslib;
namespace fileinfo{

namespace{
	//デリミタで区切られた要素から、順序を変えずに一つ以上の要素を持つ組み合わせを作成
	//例
	//foo/bar/baz
	//なら
	//foo
	//foo/bar
	//foo/bar/baz
	//bar
	//bar/baz
	//baz
	template<class T>void makePossibilityPattern(T* pattern_list,const TCHAR* sz,int delimiter){
		if(pattern_list==NULL)return;

		tstring::size_type pos=0;
		tstring::size_type head=0;
		tstring str(sz);

		str+=delimiter;

		for(;;){
			pos=str.find(delimiter,pos);

			#ifndef UNICODE
				if(pos!=0&&IsDBCSLeadByte(str[pos-1])){
					++pos;
					continue;
				}
			#endif

			if(pos==tstring::npos){
				head=str.find(delimiter,0);

				#ifndef UNICODE
					while(head!=0&&IsDBCSLeadByte(str[head-1])){
						head=str.find(delimiter,++head);
					}
				#endif

				if(head==tstring::npos)break;
				str=str.substr(++head);
				pos=0;
				continue;
			}else{
				pattern_list->push_back(str.substr(0,pos));
			}
			pos++;
		}
		return;
	}
	template void makePossibilityPattern(std::list<tstring>* pattern_list,const TCHAR* sz,int delimiter);
	template void makePossibilityPattern(std::vector<tstring>* pattern_list,const TCHAR* sz,int delimiter);
}

//ファイルの情報をFILEINFO構造体で取得
bool getFileInfo(FILEINFO* fileinfo,const TCHAR* file_path_orig){
	bool result=false;
	HANDLE handle;
	WIN32_FIND_DATA file_data;
	tstring file_path=path::removeTailSlash(file_path_orig);

	if((handle=::FindFirstFileEx(path::addLongPathPrefix(file_path).c_str(),FindExInfoStandard,&file_data,FindExSearchNameMatch,NULL,0))!=INVALID_HANDLE_VALUE){
		fileinfo->size=(long long)MAKEQWORD(file_data.nFileSizeHigh,file_data.nFileSizeLow);
		fileinfo->attr=file_data.dwFileAttributes;

		FILETIME ft={0};
		SYSTEMTIME st={0};

		::LocalFileTimeToFileTime(&(file_data.ftLastWriteTime),&ft);
		::FileTimeToSystemTime(&ft,&st);
		fileinfo->date_time=str::SYSTEMTIME2longlong(st);

		fileinfo->name=path::removeLongPathPrefix(file_path);

		::FindClose(handle);
		result=true;
	}
	return result;
}

//ファイルの情報をFILEINFO構造体で取得
bool getFileInfo(FILEINFO* fileinfo,const WIN32_FIND_DATA& file_data,const TCHAR* file_path_orig){
	bool result=false;
	tstring file_path=path::removeTailSlash(file_path_orig);

	if(strvalid(file_data.cFileName)){
		fileinfo->size=(long long)MAKEQWORD(file_data.nFileSizeHigh,file_data.nFileSizeLow);
		fileinfo->attr=file_data.dwFileAttributes;

		FILETIME ft={0};
		SYSTEMTIME st={0};

		::LocalFileTimeToFileTime(&(file_data.ftLastWriteTime),&ft);
		::FileTimeToSystemTime(&ft,&st);
		fileinfo->date_time=str::SYSTEMTIME2longlong(st);

		fileinfo->name=file_path;

		result=true;
	}

	return result;
}

bool matchPattern(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const TCHAR* base_dir){
	bool matched=true;
	tstring file_name(fileinfo.name);

	if(base_dir){
		if(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY){
			//ディレクトリの場合付加される区切り文字を削除
			file_name=path::removeTailSlash(path::makeRelativePath(base_dir,
																   FILE_ATTRIBUTE_DIRECTORY,
																   fileinfo.name.c_str(),
																   ::GetFileAttributes(fileinfo.name.c_str())));
		}
	}

	//パスの区切り文字を'/'に統一する
	str::replaceCharacter(file_name,'\\','/');

	//ファイル名
	if(!filefilter.pattern_list.empty()){
		std::list<tstring> pattern_list=filefilter.pattern_list;

		for(std::list<tstring>::iterator ite=pattern_list.begin(),
			end=pattern_list.end();
			ite!=end;
			++ite){
			//パスの区切り文字を'/'に統一する
			str::replaceCharacter(*ite,'\\','/');
		}

		std::list<tstring> file_name_list;

		if(filefilter.recursive){
			//デリミタで区切られた要素から、順序を変えずに一つ以上の要素を持つ組み合わせを作成
			makePossibilityPattern(&file_name_list,file_name.c_str(),'/');
		}else{
			str::splitString(&file_name_list,file_name.c_str(),'/');
		}

		for(std::list<tstring>::iterator ite_pattern=pattern_list.begin(),
			end=pattern_list.end();
			ite_pattern!=end;
			++ite_pattern){
			if(filefilter.recursive){
				for(std::list<tstring>::iterator ite_file_name=file_name_list.begin(),
					file_name_end=file_name_list.end();
					ite_file_name!=file_name_end;
					++ite_file_name){
					if(str::matchWildcards(ite_file_name->c_str(),ite_pattern->c_str())){
						matched=true;
						break;
					}else{
						matched=false;
					}
				}
			}else{
				std::list<tstring> pattern_list;

				str::splitString(&pattern_list,ite_pattern->c_str(),'/');

				//フィルタが対象パスより深い階層を指定している場合
				if(file_name_list.size()<pattern_list.size()){
					matched=false;
					break;
				}

				//先頭からパス区切り文字毎に比較
				for(std::list<tstring>::iterator ite_file_name=file_name_list.begin(),
					ite_pattern=pattern_list.begin(),
					file_name_end=file_name_list.end(),
					pattern_end=pattern_list.end();
					ite_file_name!=file_name_end&&ite_pattern!=pattern_end;
					++ite_file_name,++ite_pattern){
					if(str::matchWildcards(ite_file_name->c_str(),ite_pattern->c_str())){
						matched=true;
					}else{
						matched=false;
						break;
					}
				}
			}
			if(matched)break;
		}
	}
	return matched;
}

bool matchExPattern(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir){
	bool matched=true;

	//ファイル名
	if(!file_ex_filter.pattern_list.empty()){
		matched=!matchPattern(fileinfo,file_ex_filter,base_dir);
	}
	return matched;
}

bool matchSize(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	//サイズ
	if(filefilter.min_size>0&&
	   fileinfo.size<=filefilter.min_size){
		matched=false;
	}

	if(filefilter.max_size!=0&&
	   fileinfo.size>=filefilter.max_size){
		matched=false;
	}

	if(file_ex_filter.min_size>0&&
	   fileinfo.size>=file_ex_filter.min_size){
		matched=false;
	}

	if(file_ex_filter.max_size!=0&&
	   fileinfo.size<=file_ex_filter.max_size){
		matched=false;
	}
	return matched;
}

bool matchDateTime(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	//日付
	if(filefilter.oldest_date!=-1&&
	   fileinfo.date_time<=filefilter.oldest_date){
		matched=false;
	}

	if(filefilter.newest_date!=-1&&
	   fileinfo.date_time>=filefilter.newest_date){
		matched=false;
	}

	if(file_ex_filter.oldest_date!=-1&&
	   fileinfo.date_time>=file_ex_filter.oldest_date){
		matched=false;
	}

	if(file_ex_filter.newest_date!=-1&&
	   fileinfo.date_time<=file_ex_filter.newest_date){
		matched=false;
	}
	return matched;
}

bool matchAttribute(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	//FILE_ATTRIBUTE_DIRECTORYを取り除く('FILE'INFOなのでディレクトリは無い筈)
	DWORD filefilter_attr=filefilter.attr&~FILE_ATTRIBUTE_DIRECTORY;
	DWORD file_ex_filter_attr=file_ex_filter.attr&~FILE_ATTRIBUTE_DIRECTORY;

	//属性
	if(filefilter_attr!=0&&
	   !(fileinfo.attr&filefilter_attr)){
		matched=false;
	}

	if(file_ex_filter_attr!=0&&
	   fileinfo.attr&file_ex_filter_attr){
		matched=false;
	}
	return matched;
}

//フィルタ(FILEFILTER構造体)にマッチするか
bool matchFilters(const FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir){
	bool matched=true;

	//処理対象
	matched=matchPattern(fileinfo,filefilter,base_dir);
	if(!matched)return matched;

	//処理対象除外
	matched=matchExPattern(fileinfo,file_ex_filter,base_dir);
	if(!matched)return matched;

	//サイズ
	matched=matchSize(fileinfo,filefilter,file_ex_filter);
	if(!matched)return matched;

	//日付
	matched=matchDateTime(fileinfo,filefilter,file_ex_filter);
	if(!matched)return matched;

	//属性
	matched=matchAttribute(fileinfo,filefilter,file_ex_filter);
	if(!matched)return matched;

	return matched;
}

//フィルタ(fileinfo::FILEFILTER構造体)にマッチするか
//注意:file_pathではなくフルパスでマッチ判断
bool matchFilters(const TCHAR* file_path,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,const TCHAR* base_dir){
	bool result=false;
	fileinfo::FILEINFO fileinfo=fileinfo::FILEINFO();

	if(getFileInfo(&fileinfo,file_path)){
		result=matchFilters(fileinfo,filefilter,file_ex_filter,base_dir);
	}

	return result;
}

//namespace str
}

bool FileTree::addV(const TCHAR* name,const fileinfo::FILEINFO* fileinfo){
	bool result=false;
	FILEINFONODE**pp_new=&m_tree;

	while(*pp_new)pp_new=&(*pp_new)->next;

	*pp_new=new FILEINFONODE();

	if(*pp_new){
		if(fileinfo==NULL){
			fileinfo::FILEINFO* fileinfo=new fileinfo::FILEINFO();

			if(fileinfo::getFileInfo(fileinfo,name)){
				(*pp_new)->fileinfo=*fileinfo;
				result=true;
			}
			SAFE_DELETE(fileinfo);
		}else{
			(*pp_new)->fileinfo=*fileinfo;
			result=true;
		}
	}
	return result;
}

bool FileTree::addV(FILEINFONODE**pp,const TCHAR* parent_dir,const TCHAR* name,const fileinfo::FILEINFO* fileinfo){
	if(!*pp)return false;

	bool result=false;

	if(lstrcmp((*pp)->fileinfo.name.c_str(),parent_dir)==0){
		FILEINFONODE**pp_new=&(*pp)->child;

		while(*pp_new)pp_new=&(*pp_new)->next;

		*pp_new=new FILEINFONODE();

		if(*pp_new){
			if(fileinfo==NULL){
				fileinfo::FILEINFO* fileinfo=new fileinfo::FILEINFO();
				if(fileinfo::getFileInfo(fileinfo,name)){
					(*pp_new)->fileinfo=*fileinfo;
					(*pp_new)->parent_dir=*pp;
					result=true;
				}
				SAFE_DELETE(fileinfo);
			}else{
				(*pp_new)->fileinfo=*fileinfo;
				(*pp_new)->parent_dir=*pp;
				result=true;
			}
		}
		return result;
	}else{
		if(addV(&(*pp)->child,parent_dir,name,fileinfo))return true;
		if(addV(&(*pp)->next,parent_dir,name,fileinfo))return true;
	}

	return result;
}

bool FileTree::add(const TCHAR* parent_dir,const TCHAR* name,const fileinfo::FILEINFO* fileinfo){
	return (!addV(&m_tree,parent_dir,name,fileinfo))?
			((addV(name,fileinfo))?true:false):
			true;
}

bool FileTree::delNode(FILEINFONODE**pp){
	if(!*pp)return false;

	if(!(*pp)->child){
		FILEINFONODE* del_data=*pp;
		*pp=(*pp)->next;
		SAFE_DELETE(del_data);
	}else{
		delNode(&(*pp)->child);
		delNode(pp);
	}
	return true;
}

bool FileTree::delV(FILEINFONODE**pp,const TCHAR* name){
	if(!*pp)return false;

	if(lstrcmp((*pp)->fileinfo.name.c_str(),name)==0){
		return delNode(pp);
	}

	if(delV(&(*pp)->child,name))return true;
	if(delV(&(*pp)->next,name))return true;
	return false;
}

bool FileTree::del(const TCHAR* name){
	return delV(&m_tree,name);
}

void FileTree::destroy(FILEINFONODE**pp){
	if(!*pp)return;

	destroy(&(*pp)->child);
	destroy(&(*pp)->next);

	FILEINFONODE*del_node=*pp;
	SAFE_DELETE(del_node);
}

int FileTree::countSibling(FILEINFONODE**pp){
	if(!*pp)return -1;

	int count=0;

	while(*pp)if(*(pp=&(*pp)->next))++count;
	return count;
}

int FileTree::countContents(int depth){
	FILEINFONODE**pp=&m_tree;

	for(int i=0;i<depth;++i){
		pp=&(*pp)->child;
		if(!*pp)return -1;
	}
	return countSibling(pp)+1;
}

void FileTree::disableLine(FILEINFONODE**pp){
	if(!*pp)return;

	FILEINFONODE**node=&(*pp)->parent_dir;

	while(*node){
		if((*node)->disable)break;
		(*node)->disable=true;
		node=&(*node)->parent_dir;
	}
	return;
}

bool FileTree::createFileTree(const TCHAR* search_dir,const TCHAR* wildcard,bool include_dir){
	FileSearch fs;

	std::list<tstring> file_list;

	path::recursiveSearch(&file_list,search_dir,wildcard,include_dir);

	file_list.sort();

	for(std::list<tstring>::iterator ite=file_list.begin(),end=file_list.end();
		ite!=end;
		++ite){
		add(path::getParentDirectory(*ite).c_str(),ite->c_str());
	}

	return m_tree&&(m_tree->child||m_tree->next);
}

void FileTree::makeIncludeTree(FILEINFONODE**pp,DWORD options,const TCHAR* base_dir){
	if(!*pp)return;

	//マッチしないものをIncludeTreeから除外
	bool exclude=false;

	if((*pp)->fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY||
	   (!path::isAbsolutePath(base_dir)&&
	   (str::countCharacter((*pp)->fileinfo.name.c_str(),'/')||
	   str::countCharacter((*pp)->fileinfo.name.c_str(),'\\')))){
		if(options&TO_EXCLUDE_DIR&&(*pp)->fileinfo.isDirectory()){
			//TO_EXCLUDE_DIRが有効
			exclude=true;
		}else if(m_file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//x:adが有効
			exclude=true;
		}
	}else{
		if(m_filefilter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//i:ad
			//親がないファイルは除外
			if(!((*pp)->parent_dir)){
				exclude=true;
			}
		}

		if(m_file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//x:ad
			//親がいるファイルは除外
			if((*pp)->parent_dir){
				exclude=true;
			}
		}
	}

	if(!exclude){
		if(!path::isEmptyDirectory((*pp)->fileinfo.name.c_str())){
			//空ディレクトリでなければマッチしない場合
			//IncludeTreeから除外
			exclude=!fileinfo::matchFilters((*pp)->fileinfo,m_filefilter,m_file_ex_filter,base_dir);
		}else{
			//空ディレクトリならx:aeが無効でマッチしないかx:aeが有効である場合
			//IncludeTreeから除外
			if(m_file_ex_filter.include_empty_dir){
				exclude=!fileinfo::matchFilters((*pp)->fileinfo,m_filefilter,m_file_ex_filter,base_dir);
			}else{
				exclude=true;
			}
		}
	}

	if(exclude){
		(*pp)->disable=true;
		//代々のディレクトリを全てIncludeTreeから除外
		disableLine(pp);
	}


	makeIncludeTree(&(*pp)->child,options,base_dir);
	makeIncludeTree(&(*pp)->next,options,base_dir);
}

void FileTree::makeIncludeTree(DWORD options,const TCHAR* base_dir){
	makeIncludeTree(&m_tree,options,base_dir);
}

void FileTree::makeExcludeTree(FILEINFONODE**pp,DWORD options,const TCHAR* base_dir){
	if(!*pp)return;

	bool include=true;

	if((*pp)->fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY||
	   (!path::isAbsolutePath(base_dir)&&
	   (str::countCharacter((*pp)->fileinfo.name.c_str(),'/')||
	   str::countCharacter((*pp)->fileinfo.name.c_str(),'\\')))){
		if(options&TO_EXCLUDE_DIR&&(*pp)->fileinfo.isDirectory()){
			//TO_EXCLUDE_DIRが有効
			include=false;
		}else if(m_file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//x:adが有効
			include=false;
		}
	}else{
		if(m_filefilter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//i:ad
			//親がいないファイル
			if(!(*pp)->parent_dir){
				include=false;
			}
		}

		if(m_file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
			//x:ad
			//親がいるファイル
			if((*pp)->parent_dir){
				include=false;
			}
		}
	}

	if(include){
		if(!path::isEmptyDirectory((*pp)->fileinfo.name.c_str())){
			//空ディレクトリでなければマッチする場合
			include=fileinfo::matchFilters((*pp)->fileinfo,m_filefilter,m_file_ex_filter,base_dir);
		}else{
			//空ディレクトリならx:aeが無効でマッチする場合
			if(m_file_ex_filter.include_empty_dir){
				include=fileinfo::matchFilters((*pp)->fileinfo,m_filefilter,m_file_ex_filter,base_dir);
			}
		}
	}

	if(include){
		(*pp)->disable=true;
		//代々のディレクトリを全てExcludeTreeから除外
		disableLine(pp);
	}

	makeExcludeTree(&(*pp)->child,options,base_dir);
	makeExcludeTree(&(*pp)->next,options,base_dir);
}

void FileTree::makeExcludeTree(DWORD options,const TCHAR* base_dir){
	makeExcludeTree(&m_tree,options,base_dir);
}

void FileTree::tree2list(FILEINFONODE**pp,std::vector<fileinfo::FILEINFO>& list){
	if(!*pp)return;

	if(!(*pp)->disable){
		list.push_back((*pp)->fileinfo);
	}
	tree2list(&(*pp)->child,list);
	tree2list(&(*pp)->next,list);
}

bool FileTree::tree2list(std::vector<fileinfo::FILEINFO>& list){
	tree2list(&m_tree,list);
	return !list.empty();
}

#ifdef _DEBUG
void FileTree::msgTree(FILEINFONODE** pp){
	if(*pp==NULL)return;
	msg(_T("[%s]"),(*pp)->fileinfo.name.c_str());
	msgTree(&(*pp)->child);
	msgTree(&(*pp)->next);
}

void FileTree::msgTree(){
	msgTree(&m_tree);
}
#endif
