//String.h

#ifndef _STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46
#define _STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46



namespace sslib{
namespace str{

//datetime2longlong()
//longlong2datetime()
//FILETIME構造体は1601年1月1日以降
const long long min_date=1601;
//yyyymmddhhmmss
const long long max_date=99999999999999;

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
template<class T>void splitString(T* string_list,const TCHAR* sz,const TCHAR* delimiter);
template<class T>void splitString(T* string_list,const TCHAR* sz,TCHAR delimiter);

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

//Unicodeエスケープをデコード(\uxxxx(16bit),\Uxxxxxxxx(32bit)に対応)
bool decodeUnicodeEscape(std::wstring& result,const TCHAR*src,bool support_32bit=true,unsigned int c='\\');

//ファイルサイズを表す文字列をlong longに変換
long long filesize2longlong(const TCHAR* size_str);

//long long(バイト単位)をファイルサイズを表す文字列に変換
tstring longlong2filesize(const long long size_ll);

//FILETIME構造体からlong longへ変換
long long fttoll(FILETIME ft);

//long longからFILETIME構造体へ変換
FILETIME lltoft(long long ll);

//属性を表す文字をDWORDに変換
DWORD attr2DWORD(int c);

//yyyymmddhhmmss(long long型)をSYSTEMTIMEに変換
bool longlong2SYSTEMTIME(SYSTEMTIME* result_st,const long long date_time);
//SYSTEMTIMEをyyyymmddhhmmss(long long型)に変換
long long SYSTEMTIME2longlong(const SYSTEMTIME& st);

//yyyymmddhhmmss(文字列)をyyyymmddhhmmss(long long型)に変換
long long datetime2longlong(const TCHAR* date_time_str,bool max_time=false);
//yyyymmddhhmmss(long long型)をyyyymmddhhmmss(文字列)に変換
const tstring longlong2datetime(long long date_time);


//GUIDを取得
//bool getGUID(TCHAR*str,bool need_brace=true);

//CRC32を算出
//DWORD crc32(DWORD crc,const BYTE *buf,size_t length);

//namespace str
}
//namespace sslib
}

#endif //_STRING_H_CAD325EE_F515_4AD2_9587_0B82251DBB46
