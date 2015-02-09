//File.cpp
//ファイル操作クラス

#include"../StdAfx.h"
#include"sslib.h"
#include<vector>

namespace sslib{


namespace{
	const BYTE utf8_bom[]={0xef,0xbb,0xbf};
	const BYTE utf16le_bom[]={0xff,0xfe};
	const BYTE utf16be_bom[]={0xfe,0xff};
}

File::File():
	m_file_handle(INVALID_HANDLE_VALUE),
	m_file_path(),
	m_file_codepage(SJIS){
}

File::File(const TCHAR* file_path,DWORD creation_disposition,DWORD desired_access,DWORD share_mode,CODEPAGE codepage):m_file_handle(INVALID_HANDLE_VALUE){
	File::open(file_path,
		 creation_disposition,
		 desired_access,
		 share_mode,
		 codepage);
}

File::File(HANDLE handle,CODEPAGE codepage):
	m_file_handle(INVALID_HANDLE_VALUE){
	File::open(handle,codepage);
}

File::~File(){
	if(isOpened()){
		close();
	}
}

//ファイルを開く
bool File::open(const TCHAR* file_path,DWORD creation_disposition,DWORD desired_access,DWORD share_mode,CODEPAGE codepage){
	if(file_path==NULL)return false;

	if(isOpened()){
		close();
	}

	//標準入力、標準出力ならばFILE_SHARE_*の指定が必須
	if(lstrcmpi(file_path,_T("CONIN$"))==0&&share_mode==0)share_mode=FILE_SHARE_READ;
	if(lstrcmpi(file_path,_T("CONOUT$"))==0&&share_mode==0)share_mode=FILE_SHARE_WRITE;

	m_file_handle=::CreateFile(path::addLongPathPrefix(file_path).c_str(),
							   desired_access,
							   share_mode,
							   NULL,
							   creation_disposition,
							   //ディレクトリであればFILE_FLAG_BACKUP_SEMANTICSを追加
							   (!path::isDirectory(file_path))?FILE_ATTRIBUTE_NORMAL:FILE_ATTRIBUTE_NORMAL|FILE_FLAG_BACKUP_SEMANTICS,
							   NULL);
	if(m_file_handle!=INVALID_HANDLE_VALUE){
		m_file_path=file_path;
		m_file_codepage=codepage;
	}

	return isOpened();
}

//ファイルを開く
//注意:リダイレクトの有無にかかわらず標準入力/出力を開く場合open(_T("CONIN$/CONOUT$"))すること
bool File::open(HANDLE handle,CODEPAGE codepage){
	if(isOpened()){
		close();
	}

	m_file_handle=handle;
	if(m_file_handle!=INVALID_HANDLE_VALUE){
		m_file_codepage=codepage;
	}

	return isOpened();
}

//ファイルを閉じる
bool File::close(){
	if(handle()!=::GetStdHandle(STD_INPUT_HANDLE)&&
	   handle()!=::GetStdHandle(STD_OUTPUT_HANDLE)&&
	   handle()!=::GetStdHandle(STD_ERROR_HANDLE)){
		SAFE_CLOSE_EX(m_file_handle,INVALID_HANDLE_VALUE);
	}
	m_file_handle=INVALID_HANDLE_VALUE;
	m_file_path.clear();
	return true;
}

//ファイルからデータを読み込む
DWORD File::read(void* buffer,DWORD buffer_size){
	DWORD read_chars=0;

	if(isOpened()&&buffer!=NULL){
		::ReadFile(handle(),buffer,buffer_size,&read_chars,NULL);
		return read_chars;
	}

	return read_chars;
}

//ファイルの中身をリストとして取得
template<class T>bool File::readList(T* file_list,DWORD opt){
	if(!isOpened()||file_list==NULL){
		return false;
	}
	long long file_size=getSize();

	//ファイルポインタを先頭へ移動
	seek(0);
	if(opt&RL_SKIP_BOM){
		bool has_bom=false;
		BYTE file_bom[3]={0};

		read(file_bom,sizeof(file_bom));

		for(int i=0;i<(int)sizeof(utf8_bom);i++){
			if(file_bom[i]!=utf8_bom[i]){
				has_bom=false;
				break;
			}else{
				has_bom=true;
				m_file_codepage=UTF8;
			}
		}

		if(!has_bom){
			for(int i=0;i<(int)sizeof(utf16le_bom);i++){
				if(file_bom[i]!=utf16le_bom[i]){
					has_bom=false;
					break;
				}else{
					has_bom=true;
					m_file_codepage=UTF16LE;
				}
			}
		}

		if(!has_bom){
			for(int i=0;i<(int)sizeof(utf16be_bom);i++){
				if(file_bom[i]!=utf16be_bom[i]){
					has_bom=false;
					break;
				}else{
					has_bom=true;
					m_file_codepage=UTF16BE;
				}
			}
		}

		//BOMをスキップする
		if(has_bom){
			if(m_file_codepage!=SJIS){
				long bom_size=0;

				switch(m_file_codepage){
					case UTF8:
						bom_size=sizeof(utf8_bom);
						break;

					case UTF16LE:
						bom_size=sizeof(utf16le_bom);
						break;

					case UTF16BE:
						bom_size=sizeof(utf16be_bom);
						break;

					case SJIS:
					default:
						break;
				}
				seek(bom_size);
			}
		}else{
			//ファイルポインタを先頭へ移動
			seek(0);
		}

		if(!has_bom)m_file_codepage=static_cast<CODEPAGE>(m_file_codepage|NO_BOM);
	}

	std::vector<BYTE> file_buffer(file_size+1);
	if(!read(&file_buffer[0],file_size)){
		return false;
	}

	tstring file_buffer_tmp;

	switch(codepage()){
		case SJIS:{
#ifdef UNICODE
			str::sjis2utf16(&file_buffer_tmp,(char*)&file_buffer[0]);
#else
			file_buffer_tmp=(char*)&file_buffer[0];
#endif
			break;
		}

		case UTF8:{
#ifdef UNICODE
			str::utf82utf16(&file_buffer_tmp,(char*)&file_buffer[0]);
#else
			str::utf82sjis(&file_buffer_tmp,(char*)&file_buffer[0]);
#endif
			break;
		}

		case UTF16LE:{
#ifdef UNICODE
			file_buffer_tmp=(wchar_t*)&file_buffer[0];
#else
			str::utf162sjis(&file_buffer_tmp,(char*)&file_buffer[0]);
#endif
			break;
		}

		case UTF16BE:{
			std::vector<BYTE> dest_buffer(file_buffer.size());
			_swab((char*)&file_buffer[0],(char*)&dest_buffer[0],file_buffer.size());
#ifdef UNICODE
			file_buffer_tmp=(wchar_t*)&dest_buffer[0];
#else
			str::utf162sjis(&file_buffer_tmp,(wchar_t*)&dest_buffer[0]);
#endif
			break;
		}

		default:
			break;
	}

	tstring line_buffer;

	for(const TCHAR* buffer_ptr=file_buffer_tmp.c_str();/**/;buffer_ptr++){
		if(*buffer_ptr=='\n'||*buffer_ptr=='\r'||*buffer_ptr=='\0'){
			if(!line_buffer.empty()){
				if(opt&RL_REMOVE_QUOTES){
					//二重引用符を取り除く
					line_buffer=path::removeQuotation(line_buffer);
					if(!line_buffer.empty()){
						file_list->push_back(line_buffer);
					}
				}else{
					file_list->push_back(line_buffer);
				}
			}
			line_buffer.clear();

			//ループ終了
			if(*buffer_ptr=='\0')break;
		}else{
			//改行文字やNULL文字でなければ追加
			line_buffer+=*buffer_ptr;
		}
	}
	return !file_list->empty();
}
template bool File::readList(std::list<tstring>* file_list,DWORD opt);
template bool File::readList(std::vector<tstring>* file_list,DWORD opt);

//ファイルに書き出す
DWORD File::write(const void* file_buffer,DWORD file_size)const{
	DWORD written_chars=0;

	if(isOpened()&&file_buffer!=NULL){
		return ::WriteFile(handle(),file_buffer,file_size,&written_chars,NULL);
	}

	return written_chars;
}

//BOMを書き込む
void File::writeBom()const{
	if(getSize()==0&&!(m_file_codepage&NO_BOM)){
		//BOM書き込み
		if(codepage()!=SJIS){
			switch(codepage()){
				case UTF8:
					write((void*)utf8_bom,sizeof(utf8_bom));
					break;

				case UTF16LE:
					write((void*)utf16le_bom,sizeof(utf16le_bom));
					break;

				case UTF16BE:
					write((void*)utf16be_bom,sizeof(utf16be_bom));
					break;

				default:
					break;
			}
		}
	}
	return;
}

//ファイルに書き出す(可変長引数)
DWORD File::writeEx(const TCHAR* fmt,...)const{
	if(!isOpened()||fmt==NULL)return false;

	DWORD written_chars=0;
	tstring buffer;

	va_list argp;
	va_start(argp,fmt);
	buffer=format(fmt,argp).c_str();
	va_end(argp);

	if(buffer.empty())return written_chars;
#ifdef UNICODE

	//BOMを書き込む
	writeBom();

	switch(codepage()){
		case UTF8:
		case SJIS:{
			std::string multibyte_str((codepage()==SJIS)?str::utf162sjis(buffer):str::utf162utf8(buffer));
			written_chars=write(multibyte_str.c_str(),multibyte_str.length());
			break;
		}

		//TODO:UTF16BE,UTF16LEのテストをあまり行っていない
		case UTF16LE:
			written_chars=write(buffer.c_str(),buffer.length()*sizeof(TCHAR));
			break;

		case UTF16BE:{
			std::vector<BYTE> src_buffer(buffer.length()*sizeof(TCHAR));

			lstrcpy((wchar_t*)&src_buffer[0],buffer.c_str());

			std::vector<BYTE> dest_buffer(buffer.length());

			_swab((char*)&src_buffer[0],(char*)&dest_buffer[0],src_buffer.size());
			written_chars=write((wchar_t*)&dest_buffer[0],dest_buffer.size()*sizeof(TCHAR));
			break;
		}

		default:
			break;
	}

#else
	written_chars=write(buffer.c_str(),buffer.length());
#endif
	return written_chars;
}

//ファイルポインタの位置を設定
bool File::seek(long long offset,UINT from,long long* cur){
	if(!isOpened()){
		return false;
	}

	LARGE_INTEGER li,li2;

	li.QuadPart=offset;
	li2.QuadPart=0;
	if(::SetFilePointerEx(handle(),li,&li2,from)){
		if(cur)*cur=li2.QuadPart;
		return true;
	}else{
		return false;
	}
}

//ファイルポインタの位置を取得
long long File::tell(){
	if(!isOpened()){
		return -1;
	}

	LARGE_INTEGER cur;
	cur.QuadPart=0;

	return (::SetFilePointerEx(handle(),cur,&cur,FILE_CURRENT))?
			cur.QuadPart:
			0;
}

//ファイルを空にする
bool File::clear(){
	if(!isOpened())return false;

	::SetFilePointer(handle(),0,NULL,FILE_BEGIN);
	return ::SetEndOfFile(handle())!=0;
}

//ファイルサイズを取得
long long File::getSize()const{
	LARGE_INTEGER size;

	if(!isOpened()){
		return -1;
	}

	if(!::GetFileSizeEx(handle(),&size)){
		return -1;
	}

	return size.QuadPart;
}

//ファイルの更新日時/作成日時/アクセス日時を取得
bool File::getFileTime(FILETIME* last_write_time,FILETIME* creation_time,FILETIME* last_access_time){
	if(!isOpened()){
		return false;
	}

	return ::GetFileTime(handle(),creation_time,last_access_time,last_write_time)!=0;
}

//ファイルの更新日時/作成日時/アクセス日時を変更
bool File::setFileTime(const FILETIME* last_write_time,const FILETIME* creation_time,const FILETIME* last_access_time){
	if(!isOpened()){
		return false;
	}

	return ::SetFileTime(handle(),creation_time,last_access_time,last_write_time)!=0;
}

//namespace sslib
}
