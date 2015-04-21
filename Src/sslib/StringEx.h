//StringEx.h

#ifndef _STRINGEX_H_6B6A2CF8_09C6_4c05_A8DD_697F57761F23
#define _STRINGEX_H_6B6A2CF8_09C6_4c05_A8DD_697F57761F23



namespace sslib{
namespace strex{

//datetime2longlong()
//longlong2datetime()
//FILETIME構造体は1601年1月1日以降
const long long min_date=1601;
//yyyymmddhhmmss
const long long max_date=99999999999999;

//Unicodeエスケープをデコード(\uxxxx(16bit),\Uxxxxxxxx(32bit)に対応)
//bool decodeUnicodeEscape(std::wstring& result,const TCHAR*src,bool support_32bit=true,unsigned int c='\\');

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

//namespace strex
}
//namespace sslib
}

#endif //_STRINGEX_H_6B6A2CF8_09C6_4c05_A8DD_697F57761F23
