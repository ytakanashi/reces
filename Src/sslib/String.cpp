//String.cpp
//基本的な文字列操作関数

#include"../StdAfx.h"
#include"sslib.h"

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

//namespace str
}
//namespace sslib
}
