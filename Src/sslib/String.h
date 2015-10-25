//String.h

#ifndef _STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46
#define _STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46



namespace sslib{
namespace str{

//文字列から文字を検索し、その最初の位置を返す
int locateFirstCharacter(const TCHAR* str,int search_char,int begin_index=0);
//文字列から文字を検索し、その最後の位置を返す
int locateLastCharacter(const TCHAR* str,int search_char);
//文字列から文字を検索し、その個数を返す
int countCharacter(const TCHAR* str,int search_char);
int countCharacter(const tstring& str,const tstring& search_str);

//大文字小文字であるかを無視して文字を比較する
bool isEqualCharacterIgnoreCase(const int char1,const int char2);

//空白スペースを含む
inline bool containsWhiteSpace(const TCHAR* str){
	return _tcschr(str,' ')!=NULL;
}
inline bool containsWhiteSpace(const tstring& str){
	return str.find(_T(" "))!=tstring::npos;
}

//文字列末尾の改行文字を削除
bool chomp(tstring& str);

//文字列中の小文字を大文字に変換
tstring toUpper(const tstring& str);
//文字列中の大文字を小文字に変換
tstring toLower(const tstring& str);

//大文字小文字無視比較
bool isEqualStringIgnoreCase(const tstring& lhs,const tstring& rhs);

//記号である
int isSymbol(int c);

//文字を置換する
void replaceCharacter(TCHAR* str,int search_char,int replace_char);
void replaceCharacter(tstring& str,int search_char,int replace_char);

//文字列を置換する
void replaceString(tstring& str,const tstring& search_str,const tstring& replace_str);

//ワイルドカードを含む文字列を比較
bool matchWildcards(const TCHAR* str,const TCHAR* pattern_string);

//文字列を分割してリスト化
void splitString(std::list<tstring>* string_list,const TCHAR* sz,const TCHAR* delimiter);
void splitString(std::list<tstring>* string_list,const TCHAR* sz,TCHAR delimiter);

//指定したコードページからUTF16に変換
std::wstring toUtf16(UINT codepage,const std::string& ansi);
bool toUtf16(UINT codepage,std::wstring* utf16,const std::string& ansi);
//UTF16から指定したコードページに変換
std::string fromUtf16(UINT codepage,const std::wstring& utf16);
bool fromUtf16(UINT codepage,std::string* ansi,const std::wstring& utf16);
//SJISをUTF16に変換
std::wstring sjis2utf16(const std::string& sjis);
bool sjis2utf16(std::wstring* utf16,const std::string& sjis);
//UTF16をSJISに変換
std::string utf162sjis(const std::wstring& utf16);
bool utf162sjis(std::string* sjis,const std::wstring& utf16);
//UTF16をUTF8に変換
std::string utf162utf8(const std::wstring& utf16);
bool utf162utf8(std::string* utf8,const std::wstring& utf16);
//UTF8をUTF16に変換
std::wstring utf82utf16(const std::string& utf8);
bool utf82utf16(std::wstring* utf16,const std::string& utf8);
//UTF8をSJISに変換
std::string utf82sjis(const std::string& utf8);
bool utf82sjis(std::string* sjis,const std::string& utf8);

//namespace str
}
//namespace sslib
}

#endif //_STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46
