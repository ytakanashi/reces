//Path.cpp
//パス操作関数

#include"../StdAfx.h"
#include"sslib.h"
#include<mbstring.h>
#include<vector>
#include<algorithm>

namespace sslib{
namespace path{

namespace{
	int isNumberOrSymbol(int c){
		return str::isSymbol(c)||isdigit(c);
	}

	static const TCHAR long_path_prefix[]=_T("\\\\?\\");
	bool hasLongPathPrefix(const tstring& file_path){
		return file_path.substr(0,ARRAY_SIZEOF(long_path_prefix)-1)==long_path_prefix;
	}
}

//MAX_PATHを超えるパスの為の接頭辞'\\?\'を追加
tstring addLongPathPrefix(const tstring& file_path){
	if(file_path.size()>MAX_PATH&&
	   !hasLongPathPrefix(file_path)){
		return long_path_prefix+file_path;
	}else{
		return file_path;
	}
}

//MAX_PATHを超えるパスの為の接頭辞'\\?\'を削除
tstring removeLongPathPrefix(const tstring& file_path){
	if(hasLongPathPrefix(file_path)){
		return file_path.substr(ARRAY_SIZEOF(long_path_prefix)-1);
	}else{
		return file_path;
	}
}

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//ディレクトリを再帰的に検索してlistに追加
template<class T>void recursiveSearch(T* path_list,const TCHAR* search_dir,const TCHAR* wildcard,bool include_dir){
	if(path_list==NULL)return;

	FileSearch fs;

	for(fs.first(addLongPathPrefix(search_dir).c_str(),wildcard);fs.next();){
		if(!fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
			//ファイル
			path_list->push_back(removeLongPathPrefix(fs.filepath()));
		}else{
			if(include_dir)path_list->push_back(removeLongPathPrefix(fs.filepath()));
			//サブディレクトリを検索
			recursiveSearch(path_list,fs.filepath().c_str(),wildcard,include_dir);
		}
	}
	return;
}
template void recursiveSearch(std::list<tstring>* path_list,const TCHAR* search_dir,const TCHAR* wildcard,bool include_dir);
template void recursiveSearch(std::vector<tstring>* path_list,const TCHAR* search_dir,const TCHAR* wildcard,bool include_dir);

#endif

//ファイル名に使えない文字が含まれるかどうか
bool isBadName(const TCHAR* file_name){
	if(str::locateLastCharacter(file_name,'\\')!=-1||
	   str::locateLastCharacter(file_name,'/')!=-1||
	   str::locateLastCharacter(file_name,':')!=-1||
//		str::locateLastCharacter(file_name,',')!=-1||
//		str::locateLastCharacter(file_name,';')!=-1||
	   str::locateLastCharacter(file_name,'*')!=-1||
	   str::locateLastCharacter(file_name,'?')!=-1||
	   str::locateLastCharacter(file_name,'\"')!=-1||
	   str::locateLastCharacter(file_name,'<')!=-1||
	   str::locateLastCharacter(file_name,'>')!=-1||
	   str::locateLastCharacter(file_name,'|')!=-1)return true;
	else return false;
}

//末尾にスラッシュ/バックスラッシュを追加
tstring addTailSlash(const tstring& file_path){
	int slash_pos=-1;
	bool is_slash=false;

	if(file_path.empty())return file_path;

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1){
		if((slash_pos=str::locateLastCharacter(file_path.c_str(),'/'))!=-1){
			is_slash=true;
		}
	}
	if(slash_pos==2&&file_path[2]==':'&&file_path[3]=='\0'){//file_path==ルート
		return file_path;
	}else{
		int last_slash_pos=file_path.size()-1;
		if(slash_pos!=last_slash_pos){
			return file_path+((is_slash)?_T("/"):_T("\\"));
		}
	}
	return file_path;
}

//末尾のスラッシュ/バックスラッシュを削除
tstring removeTailSlash(const tstring& file_path){
	int slash_pos=-1;

	if(file_path.empty())return file_path;

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1){
		slash_pos=str::locateLastCharacter(file_path.c_str(),'/');
	}
	if(slash_pos==2&&file_path[2]==':'&&file_path[4]=='\0'){//file_path==ルート
		return file_path;
	}else{
		int last_slash_pos=file_path.size()-1;

		if(slash_pos==last_slash_pos){
			return file_path.substr(0,slash_pos);
		}
	}
	return file_path;
}

//カレントディレクトリを取得
tstring getCurrentDirectory(){
	std::vector<TCHAR> buffer(::GetCurrentDirectory(0,NULL));

	::GetCurrentDirectory(buffer.size(),&buffer[0]);
	return &buffer[0];
}

//親ディレクトリを取得
//foo\bar\baz->foo\bar
//C:\foo\bar\baz->C:\foo\bar
//foo\->foo
tstring getParentDirectory(const tstring& file_path_orig){
	int slash_pos=-1;

	if(file_path_orig.empty())return file_path_orig;

	tstring file_path(removeTailSlash(file_path_orig));

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1)slash_pos=str::locateLastCharacter(file_path.c_str(),'/');
	if(slash_pos==-1||
	   (file_path[1]==':'&&slash_pos==2&&file_path[3]=='\0')){//区切り文字が含まれないかルート
		return file_path;
	}else{
		if(file_path[1]==':'&&slash_pos==2&&file_path[3]!='\0'){//C:\foo
			++slash_pos;
		}
		return file_path.substr(0,slash_pos);
	}
}

//ルートディレクトリを取得
//foo\bar\baz->foo
/*C:\foo\bar->C:\*/
//foo\->foo
tstring getRootDirectory(const tstring& file_path){
	int slash_pos=-1;

	if(file_path.empty())return file_path;

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1)slash_pos=str::locateLastCharacter(file_path.c_str(),'/');
	if(slash_pos==-1||
	   (file_path[1]==':'&&slash_pos==2&&file_path[3]=='\0')){
		//区切り文字が含まれないかルート
		return file_path;
	}else{
		slash_pos=str::locateFirstCharacter(file_path.c_str(),'\\');
		if(slash_pos==-1)slash_pos=str::locateFirstCharacter(file_path.c_str(),'/');
		if(slash_pos==2&&file_path[2]==':'&&file_path[3]!='\0'){//C:\foo\*
			++slash_pos;
		}
		return file_path.substr(0,slash_pos);
	}
}

//ルートディレクトリを削除
tstring removeRootDirectory(const tstring& file_path){
	int slash_pos=-1;

	if(file_path.empty())return file_path;

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1)slash_pos=str::locateLastCharacter(file_path.c_str(),'/');
	if(slash_pos==-1||
	   (file_path[1]==':'&&slash_pos==2&&file_path[3]=='\0')){
		//区切り文字が含まれないかルート
		return _T("");
	}else{
		slash_pos=str::locateFirstCharacter(file_path.c_str(),'\\');
		if(slash_pos==-1)slash_pos=str::locateFirstCharacter(file_path.c_str(),'/');
		if(slash_pos==2&&file_path[2]==':'&&file_path[3]!='\0'){//C:\foo\*
			++slash_pos;
		}
		return file_path.substr(slash_pos+1);
	}
}

//実行ファイル名を取得
tstring getExeName(){
	std::vector<TCHAR> buffer(MAX_PATH);

	if(::GetModuleFileName(NULL,&buffer[0],buffer.size())){
		return &buffer[0];
	}
	return _T("");
}

//実行ファイルのディレクトリを取得
//C:\foo\bar\baz.exe->C:\foo\bar
tstring getExeDirectory(){
	std::vector<TCHAR> buffer(MAX_PATH);

	if(::GetModuleFileName(NULL,&buffer[0],buffer.size())){
		return getParentDirectory(&buffer[0]);
	}
	return _T("");
}

//ファイル名を取得
tstring getFileName(const tstring& file_path_orig){
	int slash_pos=-1;

	if(file_path_orig.empty())return file_path_orig;

	tstring file_path(removeTailSlash(file_path_orig));

	slash_pos=str::locateLastCharacter(file_path.c_str(),'\\');
	if(slash_pos==-1)slash_pos=str::locateLastCharacter(file_path.c_str(),'/');
	if(slash_pos==-1||
	   (file_path[1]==':'&&slash_pos==2&&file_path[3]=='\0')){
		//区切り文字が含まれないかルート
		return file_path;
	}else{
		return file_path.substr(slash_pos+1);
	}
}

//拡張子を削除
tstring removeExtension(const tstring& file_path){
	int dot_pos=-1;

	if(file_path.empty())return file_path;

	dot_pos=str::locateLastCharacter(file_path.c_str(),'.');
	if(dot_pos!=-1){
		return file_path.substr(0,dot_pos);
	}
	return file_path;
}

//拡張子を取得
tstring getExtension(const tstring& file_path){
	int dot_pos=-1;

	if(file_path.empty())return file_path;

	tstring file_name=getFileName(file_path);

	dot_pos=str::locateLastCharacter(file_name.c_str(),'.');
	if(dot_pos!=-1){
		return file_name.substr(dot_pos+1);
	}
	return file_path;
}

//短いファイル名を取得
tstring getShortPathName(const TCHAR* file_path){
	if(!fileExists(file_path))return _T("");

	std::vector<TCHAR> buffer(::GetShortPathName(path::addLongPathPrefix(file_path).c_str(),NULL,0));

	::GetShortPathName(path::addLongPathPrefix(file_path).c_str(),&buffer[0],buffer.size());
	return path::removeLongPathPrefix(&buffer[0]);
}

//長いファイル名を取得
tstring getLongPathName(const TCHAR* file_path){
	if(!fileExists(file_path))return _T("");

	std::vector<TCHAR> buffer(::GetLongPathName(path::addLongPathPrefix(file_path).c_str(),NULL,0));

	::GetLongPathName(path::addLongPathPrefix(file_path).c_str(),&buffer[0],buffer.size());
	return path::removeLongPathPrefix(&buffer[0]);
}

//二重引用符を取り除く
tstring removeQuotation(const tstring& file_path){
	if(file_path.empty())return file_path;

	return file_path.substr((file_path.find_first_of('\"')==0)?1:0,
							(file_path.find_last_of('\"')==file_path.length()-1)?file_path.length()-2:tstring::npos);
}

//末尾の条件に合致する文字を削除
tstring removeLastCharacter(const tstring& file_path,int(*check)(int c)){
	for(int i=file_path.length()-1;i!=-1;--i){
#ifndef UNICODE
		if(::IsDBCSLeadByte(file_path.c_str()[i])){
			--i;
			continue;
		}
#endif

		if(!check(file_path.c_str()[i])){
#ifndef UNICODE
			if(::IsDBCSLeadByte(file_path.c_str()[i])){
				++i;
			}
#endif
			return file_path.substr(0,++i);
		}
	}
	return file_path;
}

//末尾の記号を削除
tstring removeLastSymbol(const tstring& file_path){
	return removeLastCharacter(file_path,str::isSymbol);
}

//末尾の数字を削除
tstring removeLastNumber(const tstring& file_path){
	return removeLastCharacter(file_path,isdigit);
}

//末尾の数字と記号を削除
tstring removeLastNumberAndSymbol(const tstring& file_path){
	return removeLastCharacter(file_path,isNumberOrSymbol);
}

//一時ディレクトリのパスを取得(末尾に\有り)
tstring getTempDirPath(){
	std::vector<TCHAR> buffer(::GetTempPath(0,NULL));

	::GetTempPath(buffer.size(),&buffer[0]);
	return getLongPathName(&buffer[0]);
}

//相対パスを取得する
//注意:ディレクトリの場合末尾に区切り文字が追加される
tstring makeRelativePath(const TCHAR* base_path_orig,DWORD base_path_attr,const TCHAR* file_path_orig,DWORD file_path_attr){
	tstring relative_path;
	tstring	base_path;
	tstring	file_path;
	tstring::size_type delimiter_pos;

	if(base_path_orig==NULL||file_path_orig==NULL)return _T("");

	tstring base_path_bak(base_path_orig);
	tstring file_path_bak(file_path_orig);

	//ディレクトリのパスの末尾に区切り文字を追加
	if(base_path_attr&FILE_ATTRIBUTE_DIRECTORY){
		base_path_bak=addTailSlash(base_path_bak);
	}

	if(file_path_attr&FILE_ATTRIBUTE_DIRECTORY){
		file_path_bak=addTailSlash(file_path_bak);
	}

	base_path=base_path_bak;
	file_path=file_path_bak;
	if(base_path.length()==0||file_path.length()==0)return file_path;

	int base_path_delimiter=guessDelimiter(base_path.c_str());

	delimiter_pos=base_path.rfind(base_path_delimiter);
	if(delimiter_pos==tstring::npos){
		return _T("");
	}

	if(!(base_path_attr&FILE_ATTRIBUTE_DIRECTORY)){
		base_path=base_path.substr(0,delimiter_pos);
	}

	int file_path_delimiter=guessDelimiter(file_path.c_str());;

	delimiter_pos=file_path.rfind(file_path_delimiter);
	if(delimiter_pos==tstring::npos){
		return _T("");
	}

	if(!(file_path_attr&FILE_ATTRIBUTE_DIRECTORY)){
		file_path=file_path.substr(0,delimiter_pos);
	}

	if(base_path_delimiter!=file_path_delimiter){
		//双方の区切り文字が異なる場合、base_path_delimiterに統一する
		str::replaceCharacter(file_path,file_path_delimiter,base_path_delimiter);
	}

	if(base_path.length()==0||file_path.length()==0)return _T("");

	if(file_path.find(base_path,0)==0){
		return file_path_bak.substr(base_path.length());
	}

	//以下、'..'を用いてディレクトリの移動が必要なパス
	while(1){
		if(base_path.find(file_path)==0){
			relative_path=file_path_bak.substr(file_path.length()+1);

			base_path=base_path_bak.substr(file_path.length()+1);
			int delimiter_count=str::countCharacter(base_path.c_str(),base_path_delimiter);

			for(int i=0;i<delimiter_count;i++){
				relative_path=((base_path_delimiter=='\\')?_T("..\\"):_T("../"))+relative_path;
			}
			return relative_path;
		}

		delimiter_pos=file_path.rfind(base_path_delimiter);
		if(delimiter_pos==tstring::npos)break;
		file_path=file_path.substr(0,delimiter_pos);
	}
	return _T("");
}

//ルートであるか否か
bool isRoot(const TCHAR* file_path){
	switch(lstrlen(file_path)){
		case 1:
			//区切り文字単体
			if((file_path[0]=='\\'||file_path[0]=='/')&&file_path[1]=='\0')return true;
			else return false;
			break;

		case 3:
			//ルート(X:\)
			if(file_path[1]==':'&&(file_path[2]=='\\'||file_path[2]=='/')&&file_path[3]=='\0')return true;
			else return false;
			break;

		default:
			if(str::countCharacter(file_path,'\\')<3){
				if(str::countCharacter(file_path,'/')<3)return false;
			}
			//UNCパス
			if((file_path[0]=='\\'&&file_path[1]=='\\'&&file_path[2]!='\\')||
			   (file_path[0]=='/'&&file_path[1]=='/'&&file_path[2]!='/'))return true;
			else return false;
			break;
	}
}

//絶対パスであるか否か
bool isAbsolutePath(const TCHAR* file_path){
	return static_cast<bool>(file_path[0]&&file_path[1]&&file_path[2]&&file_path[1]==':'&&(file_path[2]=='\\'||file_path[2]=='/'));
}

//相対パスであるか否か
bool isRelativePath(const TCHAR* file_path){
	if(file_path[0]=='.'){
		//'.\\'or'..\\'
		if(file_path[1]=='\\'||
		   (file_path[1]=='.'&&file_path[2]=='\\'))return true;
	}
	return false;
}

//ディレクトリであるか否か
bool isDirectory(const TCHAR* file_path){
	return file_path&&
		fileExists(file_path)&&
			(::GetFileAttributes(addLongPathPrefix(file_path).c_str())&FILE_ATTRIBUTE_DIRECTORY)!=0;
}

//ファイルが存在するか否か
bool fileExists(const TCHAR* file_path){
	return file_path&&(::GetFileAttributes(addLongPathPrefix(file_path).c_str())!=0xffffffff);
}

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//空のディレクトリである
bool isEmptyDirectory(const TCHAR* dir_path){

	if(dir_path==NULL)return false;
	if(!isDirectory(dir_path))return false;

	FileSearch fs;
	fs.first(dir_path);

	return fs.next()==NULL;
}
#endif

//フルパスを取得(存在する保証は無し)
bool getFullPath(TCHAR* full_path,unsigned int buffer_size,const TCHAR*relative_path,const TCHAR*base_dir){
	bool result=false;
	tstring cur_dir;

	if(base_dir){
		cur_dir=getCurrentDirectory();
		//勿論base_dirが存在することが条件
		::SetCurrentDirectory(base_dir);
	}
	result=::GetFullPathName(relative_path,buffer_size,full_path,NULL)!=0;

	if(base_dir)::SetCurrentDirectory(cur_dir.c_str());
	return result;
}

//ワイルドカードを含むパスである
bool containsWildcard(const TCHAR* file_path){
	return (str::locateLastCharacter(file_path,'*')!=-1||str::locateLastCharacter(file_path,'?')!=-1);
}

//分割ファイル名の拡張子を作成
tstring createPartExtension(long count,int digit){
	tstring ext;

	int max_number=0;
	long max_alphabet=1;

	//'999'や'99999'など数字のみで構成される拡張子のmaxを作成
	for(int i=0;i<digit;i++){
		int temp=9;
		for(int ii=0;ii<i;ii++)temp*=10;
		max_number+=temp;
	}

	//アルファベットのみで構成される拡張子の総数を計算
	for(int i=0;i<digit;i++)max_alphabet*=26;

	if(count<=max_number){
		//'*'を使用すると桁数を変数から指定できる
		ext=format(_T("%0*ld"),digit,count);
	}else if(count<=max_number+max_alphabet){
		long i=count-(max_number+1);

		ext.substr(0,digit);
		for(long ii=digit-1;;ii--){
			ext[ii]=i%26+'a';
			if(ii>0)i/=26;
			else break;
		}
	}
	return ext;
}

//namespace path
}
//namespace sslib
}
