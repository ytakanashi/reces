//Path.h

#ifndef _PATH_H_CF78ED0E_4800_4E6F_903D_7DDF669BD13F
#define _PATH_H_CF78ED0E_4800_4E6F_903D_7DDF669BD13F

namespace sslib{
namespace path{


//スペースが含まれていれば'"'で囲む
tstring quote(const tstring& file_path,bool required=false);

//MAX_PATHを超えるパスの為の接頭辞'\\?\'を追加
tstring addLongPathPrefix(const tstring& file_path);

//MAX_PATHを超えるパスの為の接頭辞'\\?\'を削除
tstring removeLongPathPrefix(const tstring& file_path);

//ディレクトリを再帰的に検索してlistに追加
void recursiveSearch(std::list<tstring>* path_list,const TCHAR* search_dir,const TCHAR* wildcard=_T("*"),bool include_dir=false);

//ファイル名に使えない文字が含まれるかどうか
bool isBadName(const TCHAR* file_name);


//末尾にスラッシュ/バックスラッシュを追加
tstring addTailSlash(const tstring& file_path);
//末尾のスラッシュ/バックスラッシュを削除
tstring removeTailSlash(const tstring& file_path);

//区切り文字を推測、含まれなければ'\'を返す
inline int guessDelimiter(const TCHAR* file_path){
	return (str::locateLastCharacter(file_path,'\\')!=-1)?
		'\\':
		(str::locateLastCharacter(file_path,'/')!=-1)?
			'/':
			'\\';
}

//カレントディレクトリを取得
tstring getCurrentDirectory();

//親ディレクトリを取得
tstring getParentDirectory(const tstring& file_path_orig);
//ルートディレクトリを取得
tstring getRootDirectory(const tstring& file_path);
//ルートディレクトリを削除
tstring removeRootDirectory(const tstring& file_path);
//実行ファイル名を取得
tstring getExeName();
//実行ファイルのディレクトリを取得
tstring getExeDirectory();
//ファイル名を取得
tstring getFileName(const tstring& file_path_orig);
//拡張子を削除
tstring removeExtension(const tstring& file_path);
//拡張子を取得
tstring getExtension(const tstring& file_path);

//短いファイル名を取得
tstring getShortPathName(const TCHAR* file_path);

//長いファイル名を取得
tstring getLongPathName(const TCHAR* file_path);

//二重引用符を取り除く
tstring removeQuotation(const tstring& file_path);
//末尾の条件に合致する文字を削除
tstring removeLastCharacters(const tstring& file_path,int(*check)(int c));
//末尾の記号を削除
tstring removeLastSymbols(const tstring& file_path);
//末尾の数字を削除
tstring removeLastNumbers(const tstring& file_path);
//末尾の数字と記号を削除
tstring removeLastNumbersAndSymbols(const tstring& file_path);
//末尾のスペースとドットを削除
tstring removeSpacesAndDots(const tstring& file_path);

//一時ディレクトリのパスを取得(末尾に\有り)
tstring getTempDirPath();

//相対パスを取得する
//注意:ディレクトリの場合末尾に区切り文字が追加される
tstring makeRelativePath(const TCHAR* base_path_orig,DWORD base_path_attr,const TCHAR* file_path_orig,DWORD file_path_attr);

//ルートであるか否か
bool isRoot(const TCHAR* file_path);
//絶対パスであるか否か
bool isAbsolutePath(const TCHAR* file_path);
//相対パスであるか否か
bool isRelativePath(const TCHAR* file_path);
//ディレクトリであるかどうか
bool isDirectory(const TCHAR* file_path);
//ファイルが存在するか否か
bool fileExists(const TCHAR* file_path);
//空のディレクトリである
bool isEmptyDirectory(const TCHAR* dir_path);

//フルパスを取得
bool getFullPath(TCHAR* full_path,unsigned int buffer_size,const TCHAR*relative_path,const TCHAR*base_dir=NULL);

//ワイルドカードを含むパスである
bool containsWildcard(const TCHAR* file_path);

//分割ファイル名の拡張子を作成
tstring createPartExtension(long count,int digit=3);

//namespace path
}
//namespace sslib
}

#endif //_PATH_H_CF78ED0E_4800_4E6F_903D_7DDF669BD13F
