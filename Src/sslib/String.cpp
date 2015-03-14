//String.cpp
//基本的な文字列操作関数

#include"../StdAfx.h"
#include"sslib.h"
#include<ctype.h>
#include<vector>
#include<algorithm>
#include<objbase.h>

namespace sslib{
namespace str{


//文字列から文字を検索し、その最初の位置を返す
int locateFirstCharacter(const TCHAR* str,int search_char,int begin_index){
	int result=-1;

	for(int i=begin_index;str[i]!='\0';++i){
		#ifndef UNICODE
			if(IsDBCSLeadByte(str[i])){
				++i;
				continue;
			}
		#endif
		if(str[i]==search_char){result=i;break;}
	}

	return result;
}

//文字列から文字を検索し、その最後の位置を返す
int locateLastCharacter(const TCHAR* str,int search_char){
	int result=-1;

	for(int i=0;str[i]!='\0';++i){
		#ifndef UNICODE
			if(IsDBCSLeadByte(str[i])){
				++i;
				continue;
			}
		#endif
		if(str[i]==search_char)result=i;
	}

	return result;
}

//文字列から文字を検索し、その個数を返す
int countCharacter(const TCHAR* str,int search_char){
	int result=0;

	for(int i=0;str[i]!='\0';++i){
		#ifndef UNICODE
			if(IsDBCSLeadByte(str[i])){
				++i;
				continue;
			}
		#endif
		if(str[i]==search_char)result++;
	}
	return result;
}

int countCharacter(const tstring& str,const tstring& search_str){
	int result=0;

	for(tstring::const_iterator ite=str.begin(),end=str.end();ite!=end;++ite){
		for(tstring::const_iterator ite_search_str=search_str.begin(),
			end_search_str=search_str.end();
			ite_search_str!=end_search_str;++ite_search_str){
			if(*ite==*ite_search_str){
				++result;
			}
		}
	}
	return result;
}

//大文字小文字であるかを無視して文字を比較する
bool isEqualCharacterIgnoreCase(const int char1,const int char2){
	return (::CharLower(reinterpret_cast<LPTSTR>(MAKELONG(char1,0)))==CharLower(reinterpret_cast<LPTSTR>(MAKELONG(char2,0))));
}


//文字列中の小文字を大文字に変換
tstring toUpper(const tstring& str){
	tstring upper_str(str);

	std::transform(upper_str.begin(),upper_str.end(),upper_str.begin(),toupper);
	return upper_str;
}

//文字列中の大文字を小文字に変換
tstring toLower(const tstring& str){
	tstring lower_str(str);

	transform(lower_str.begin(),lower_str.end(),lower_str.begin(),tolower);
	return lower_str;
}

//大文字小文字無視比較
bool isEqualStringIgnoreCase(const tstring& lhs,const tstring& rhs){
	return toLower(lhs)==toLower(rhs);
}

//記号である
int isSymbol(int c){
	return c>' '&&!(isalpha(c)||isdigit(c));
}

//文字を置換する
void replaceCharacter(TCHAR* str,int search_char,int replace_char){
	for(int i=0;;i++){

#ifndef UNICODE
		if(IsDBCSLeadByte(str[i])){
			++i;
			continue;
		}
#endif

		if(str[i]==search_char)str[i]=replace_char;
		if(str[i]=='\0')break;
	}
	return;
}

//文字を置換する
void replaceCharacter(tstring& str,int search_char,int replace_char){
	tstring::iterator ite=str.begin();

	while(ite!=str.end()){
#ifndef UNICODE
			if(IsDBCSLeadByte(*ite)){
				++ite;
				continue;
			}
#endif
		if(*ite==search_char)*ite=replace_char;
		++ite;
	}
	return;
}

//文字列を置換する
void replaceString(tstring& str,const tstring& search_str,const tstring& replace_str){
	tstring::size_type pos=0;

	while(pos=str.find(search_str,pos),pos!=tstring::npos){
		str.replace(pos,search_str.length(),replace_str);
		pos+=replace_str.length();
	}
}

//ワイルドカードを含む文字列を比較
bool matchWildcards(const TCHAR* str,const TCHAR* pattern_string){
	for(;;){
		int c=*(pattern_string++);

		switch(c){
			case '\0':
				return !(*str);

			case '*':
				if(!(*pattern_string))return true;

				do{
					if(matchWildcards(str,pattern_string)){
						return true;
					}
				}while(*(str++));
				return false;

			case '?':
				if(!(*str))return false;

				str++;
				break;

			default:
				if(!isEqualCharacterIgnoreCase(*(str++),c))return false;
				break;
		}
	}
}

//文字列を分割してリスト化
void splitString(std::list<tstring>* string_list,const TCHAR* sz,const TCHAR* delimiter){
	if(string_list==NULL)return;

	tstring::size_type pos;
	tstring str(sz);

	while((pos=str.find_first_of(delimiter))!=tstring::npos){
		if(pos>0){
			string_list->push_back(str.substr(0,pos));
		}
		str=str.substr(pos+1);
	}
	if(str.length()>0){
		string_list->push_back(str);
	}
	return;
}

void splitString(std::list<tstring>* string_list,const TCHAR* sz,TCHAR delimiter){
	TCHAR dlmtr[2]={};

	*dlmtr=delimiter;
	*(dlmtr+1)='\0';
	splitString(string_list,sz,dlmtr);
}

//SJISをUTF16に変換
std::wstring sjis2utf16(const std::string& sjis){
	int utf16_length=::MultiByteToWideChar(CP_ACP,0,sjis.c_str(),-1,NULL,0);

	if(utf16_length>0){
		std::vector<wchar_t> utf16_v(utf16_length);
		if(::MultiByteToWideChar(CP_ACP,0,sjis.c_str(),-1,&utf16_v[0],utf16_length)){
			return &utf16_v[0];
		}
	}
	return L"";
}

bool sjis2utf16(std::wstring* utf16,const std::string& sjis){
	int utf16_length=::MultiByteToWideChar(CP_ACP,0,sjis.c_str(),-1,NULL,0);

	if(utf16&&utf16_length>0){
		std::vector<wchar_t> utf16_v(utf16_length);
		if(::MultiByteToWideChar(CP_ACP,0,sjis.c_str(),-1,&utf16_v[0],utf16_length)){
			utf16->assign(&utf16_v[0]);
			return true;
		}
	}
	return false;
}

//UTF16をSJISに変換
std::string utf162sjis(const std::wstring& utf16){
	int sjis_length=::WideCharToMultiByte(CP_ACP,0,utf16.c_str(),-1,NULL,0,NULL,NULL);

	if(sjis_length>0){
		std::vector<char> sjis_v(sjis_length);
		if(::WideCharToMultiByte(CP_ACP,0,utf16.c_str(),-1,&sjis_v[0],sjis_length,NULL,NULL)){
			return &sjis_v[0];
		}
	}
	return "";
}

bool utf162sjis(std::string* sjis,const std::wstring& utf16){
	int sjis_length=::WideCharToMultiByte(CP_ACP,0,utf16.c_str(),-1,NULL,0,NULL,NULL);

	if(sjis&&sjis_length>0){
		std::vector<char> sjis_v(sjis_length);
		if(::WideCharToMultiByte(CP_ACP,0,utf16.c_str(),-1,&sjis_v[0],sjis_length,NULL,NULL)){
			sjis->assign(&sjis_v[0]);
			return true;
		}
	}
	return false;
}

//UTF16をUTF8に変換
std::string utf162utf8(const std::wstring& utf16){
	int utf8_length=::WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,NULL,0,NULL,NULL);

	if(utf8_length>0){
		std::vector<char> utf8_v(utf8_length);
		if(::WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,&utf8_v[0],utf8_length,NULL,NULL)){
			return &utf8_v[0];
		}
	}
	return "";
}

bool utf162utf8(std::string* utf8,const std::wstring& utf16){
	int utf8_length=::WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,NULL,0,NULL,NULL);

	if(utf8&&utf8_length>0){
		std::vector<char> utf8_v(utf8_length);
		if(::WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,&utf8_v[0],utf8_length,NULL,NULL)){
			utf8->assign(&utf8_v[0]);
			return true;
		}
	}
	return false;
}

//UTF8をUTF16に変換
std::wstring utf82utf16(const std::string& utf8){
	int utf16_length=::MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,NULL,0);

	if(utf16_length>0){
		std::vector<wchar_t> utf16_v(utf16_length);

		if(::MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,&utf16_v[0],utf16_length)){
			return &utf16_v[0];
		}
	}
	return L"";
}

bool utf82utf16(std::wstring* utf16,const std::string& utf8){
	int utf16_length=::MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,NULL,0);

	if(utf16&&utf16_length>0){
		std::vector<wchar_t> utf16_v(utf16_length);

		if(::MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,&utf16_v[0],utf16_length)){
			utf16->assign(&utf16_v[0]);
			return true;
		}
	}
	return false;
}

//UTF8をSJISに変換
std::string utf82sjis(const std::string& utf8){
	std::wstring utf16=utf82utf16(utf8);
	if(!utf16.empty()){
		return utf162sjis(utf16);
	}
	return "";
}

bool utf82sjis(std::string* sjis,const std::string& utf8){
	std::wstring utf16;

	if(sjis&&utf82utf16(&utf16,utf8)){
		return utf162sjis(sjis,utf16);
	}
	return false;
}

//Unicodeエスケープをデコード(\uxxxx(16bit),\Uxxxxxxxx(32bit)に対応)
bool decodeUnicodeEscape(std::wstring& result,const TCHAR*src,bool support_32bit,unsigned int c){
	bool decoded=false;
	size_t i=0;
	size_t orig_str_length=lstrlen(src);
	TCHAR* decoded_str=new TCHAR[orig_str_length+1];

	while(*src&&i<orig_str_length){
		if(src[0]==c&&(src[1]=='u'||src[1]=='U')){
			size_t digit=(src[1]=='u'||!support_32bit)?4:8;
			bool esc=true;

			for(size_t ii=2;ii<digit&&esc;ii++){if(src[ii]!='\0'&&(isalpha(src[ii])||isdigit(src[ii]))){continue;}esc=false;}
			if(esc){
				TCHAR tmp[9]={};

				lstrcpyn(tmp,src+2,digit+1);
				decoded_str[i++]=static_cast<TCHAR>(_tcstoul(tmp,NULL,16));
				src+=digit+2;
				decoded=true;
			}
		}else{
			decoded_str[i++]=*src++;
		}
	}

	decoded_str[i]='\0';

	if(decoded){
		result.assign(decoded_str);
		SAFE_DELETE_ARRAY(decoded_str);
		return true;
	}else{
		SAFE_DELETE_ARRAY(decoded_str);
		return false;
	}
	return true;
}


//ファイルサイズを表す文字列をlong longに変換
long long filesize2longlong(const TCHAR* size_str){
	long long result=-1;

#ifdef _MSC_VER
	long double file_size=0;

	_stscanf(size_str,_T("%Lf"),&file_size);
#else
	long double file_size=_tcstold(size_str,NULL);
#endif

	//'k'or'kb'
	if(isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-1],'k')||
	   isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-2],'k')){
		result=file_size*1024;
	}else if(isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-1],'m')||
			 isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-2],'m')){
		result=file_size*(1024<<10);
	}else if(isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-1],'g')||
			 isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-2],'g')){
		result=file_size*(1024<<20);
	}else if(isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-1],'t')||
			 isEqualCharacterIgnoreCase(size_str[lstrlen(size_str)-2],'t')){
		result=file_size*(1024<<30);
	}else{
		result=file_size;
	}

	return result;
}

//long long(バイト単位)をファイルサイズを表す文字列に変換
tstring longlong2filesize(const long long size_ll){
	tstring str;

	if(size_ll<1024)str=format(_T("%I64db"),size_ll);
	else if(size_ll<(1024<<10))str=format(_T("%.2fkb"),size_ll/1024.0);
	else if(size_ll<(1024LL<<20))str=format(_T("%.2fmb"),size_ll/((1024<<10)*1.0));
	else if(size_ll<(1024LL<<30))str=format(_T("%.2fgb"),size_ll/((1024<<20)*1.0));
	else if(size_ll<(1024LL<<40))str=format(_T("%.2ftb"),size_ll/((1024LL<<30)*1.0));
	else str=format(_T("%I64db"),size_ll);

	return str;
}

//FILETIME構造体からlong longへ変換
long long fttoll(FILETIME ft){
	return static_cast<long long>(ft.dwHighDateTime)<<32|ft.dwLowDateTime;
}

//long longからFILETIME構造体へ変換
FILETIME lltoft(long long ll){
	FILETIME ft={};

	ft.dwHighDateTime=static_cast<DWORD>(ll>>32);
	ft.dwLowDateTime=static_cast<DWORD>(ll);
	return ft;
}

//属性を表す文字をDWORDに変換
DWORD attr2DWORD(int c){
	switch(c){
/*
		case 'a':
		case 'A':
			return FILE_ATTRIBUTE_ARCHIVE;
*/
/*
		case 'c':
		case 'C':
			return FILE_ATTRIBUTE_COMPRESSED;
*/
		case 'd':
		case 'D':
			return FILE_ATTRIBUTE_DIRECTORY;
/*
		case 'e':
		case 'E':
			return FILE_ATTRIBUTE_ENCRYPTED;
*/
		case 'h':
		case 'H':
			return FILE_ATTRIBUTE_HIDDEN;
/*
		case 'n':
		case 'N':
			return FILE_ATTRIBUTE_NORMAL;
*/
		case 'r':
		case 'R':
			return FILE_ATTRIBUTE_READONLY;

		case 's':
		case 'S':
			return FILE_ATTRIBUTE_SYSTEM;

		default:
			return 0;
	}
}

//yyyymmddhhmmss(long long型)をSYSTEMTIMEに変換
bool longlong2SYSTEMTIME(SYSTEMTIME* result_st,const long long date_time){
	if(result_st&&date_time>min_date&&date_time<max_date){
		result_st->wYear=static_cast<WORD>(date_time/10000000000);
		result_st->wMonth=static_cast<WORD>((date_time/100000000)%100);
		result_st->wDay=static_cast<WORD>((date_time/1000000)%100);
		result_st->wHour=static_cast<WORD>((date_time/10000)%100);
		result_st->wMinute=static_cast<WORD>((date_time/100)%100);
		result_st->wSecond=static_cast<WORD>(date_time%100);

		if(result_st->wYear<min_date)result_st->wYear=min_date;
		if(result_st->wYear>30827)result_st->wYear=30827;
		if(!result_st->wMonth)result_st->wMonth=1;
		if(!result_st->wDay)result_st->wDay=1;
		return true;
	}
	return false;
}

//SYSTEMTIMEをyyyymmddhhmmss(long long型)に変換
long long SYSTEMTIME2longlong(const SYSTEMTIME& st){
	long long result=0;

	result=st.wYear*10000000000;
	result+=st.wMonth*100000000;
	result+=st.wDay*1000000;
	result+=st.wHour*10000;
	result+=st.wMinute*100;
	result+=st.wSecond;
	return result;
}

//yyyymmddhhmmss(文字列)をyyyymmddhhmmss(long long型)に変換
long long datetime2longlong(const TCHAR* date_time_str,bool max_time){
	long long date_time;

	date_time=_tcstoll(date_time_str,NULL,10);

	if(date_time<min_date)return -1;

	size_t skip_digit=0;

	//不足している桁を補う
	for(;date_time<max_date;){date_time*=10;skip_digit++;}
	//余分な桁を落とす
	for(;date_time>max_date;)date_time/=10;

	if(!((date_time/100000000)%100)&&
	   !max_time){
		//月
		date_time+=100000000;
	}

	if(!((date_time/1000000)%100)&&
	   !max_time){
		//日
		date_time+=1000000;
	}

	if(max_time){
		for(size_t i=0;i<skip_digit;i++){
			switch(i){
				case 1:
					//秒
					date_time+=59;
					break;

				case 3:
					//分
					date_time+=59*100;
					break;

				case 5:
					//時間
					date_time+=23*10000;
					break;

				case 7:
					//日
					date_time+=31*1000000;
					break;

				case 9:
					//月
					date_time+=12*100000000;
					break;

				default:
					break;
			}
		}
	}

	return date_time;
}

//yyyymmddhhmmss(long long型)をyyyymmddhhmmss(文字列)に変換
const tstring longlong2datetime(long long date_time){
	if(date_time<min_date||date_time>max_date){
		return _T("");
	}

	return format(_T("%I64d"),date_time);
}

#if 0
//日付と時刻を表す文字列yyyymmddhhmmssをlong longに変換
long long datetime2longlong(const TCHAR* date_time_str,bool max_time){
	FILETIME result_ft={};
	long long date_time;

	date_time=_tcstoll(date_time_str,NULL,10);

	if(date_time<min_date)return -1;

	size_t skip_digit=0;

	//不足している桁を補う
	for(;date_time<max_date;){date_time*=10;skip_digit++;}
	//余分な桁を落とす
	for(;date_time>max_date;)date_time/=10;

	SYSTEMTIME st={0};

	//各数値を抽出
	st.wYear=static_cast<WORD>(date_time/10000000000);
	st.wMonth=static_cast<WORD>((date_time/100000000)%100);
	st.wDay=static_cast<WORD>((date_time/1000000)%100);
	st.wHour=static_cast<WORD>((date_time/10000)%100);
	st.wMinute=static_cast<WORD>((date_time/100)%100);
	st.wSecond=static_cast<WORD>(date_time%100);

	if(!st.wMonth)st.wMonth=1;
	if(!st.wDay)st.wDay=1;

	if(max_time){
		//'D'指定の場合、指定されていない数値を最大にする
		//例えば、'20120101'であれば、時間分秒が指定されていないので、
		//23時59分59秒とする

		//ミリ秒は元から指定できない
		st.wMilliseconds=999;

		for(size_t i=0;i<skip_digit;i++){
			switch(i){
				case 1:
					//秒
					st.wSecond=59;
					break;

				case 3:
					//分
					st.wMinute=59;
					break;

				case 5:
					//時間
					st.wHour=23;
					break;

				case 7:
					//日
					st.wDay=31;
					break;

				case 9:
					//月
					st.wMonth=12;
					break;

				default:
					break;
			}
		}
	}

	FILETIME tmp={};

	if(!::SystemTimeToFileTime(&st,&tmp)){
		return -1;
	}

	if(!::LocalFileTimeToFileTime(&tmp,&result_ft)){
		return -1;
	}

	return fttoll(result_ft);
}
#endif

#if 0
//long longから日付と時刻を表す文字列yyyymmddhhmmssに変換
const tstring longlong2datetime(long long date_time){
	FILETIME ft={},tmp={};
	SYSTEMTIME st={};
	long long result_ll;

	if(date_time<min_date||date_time>max_date){
		return _T("");
	}

	ft=lltoft(date_time);

	if(!::FileTimeToLocalFileTime(&ft,&tmp)){
		return _T("");
	}
	if(!::FileTimeToSystemTime(&tmp,&st)){
		return _T("");
	}

	result_ll=st.wYear*10000000000;
	result_ll+=st.wMonth*100000000;
	result_ll+=st.wDay*1000000;
	result_ll+=st.wHour*10000;
	result_ll+=st.wMinute*100;
	result_ll+=st.wSecond;


	return format(_T("%I64d"),result_ll);
}
#endif

/*
//GUIDを取得
bool getGUID(TCHAR*str,bool need_brace){
	GUID guid;
	HRESULT result;

	result=::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	if(FAILED(result)){
		return false;
	}

	::CoCreateGuid(&guid);

	if(need_brace){
		wsprintf(str,_T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
				 guid.Data1,
				 guid.Data2,
				 guid.Data3,
				 guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],
				 guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	}else{
		wsprintf(str,_T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
				 guid.Data1,
				 guid.Data2,
				 guid.Data3,
				 guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],
				 guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	}

	::CoUninitialize();

	return true;
}
*/

/*
static DWORD crc_table[256]={
	0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
};

//CRC32を算出
DWORD crc32(DWORD crc,const BYTE *buf,size_t length){
	crc^=0xffffffffL;

	while(length--){
		crc=crc_table[(crc^*buf++)&0xff]^(crc>>8);
	}
	return crc^0xffffffffL;
}
*/
//namespace str
}
//namespace sslib
}
