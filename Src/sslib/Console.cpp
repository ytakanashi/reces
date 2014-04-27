//Console.cpp
//コンソール操作クラス

#include"../StdAfx.h"
#include"Console.h"
#include"VariableArgument.h"
#include<locale.h>

namespace sslib{

Console::Console(DWORD handle_type):
	m_handle(INVALID_HANDLE_VALUE),
	m_is_redirected(false),
	m_orig_colors(0),
	write_buffer_size(2048){
	m_handle=::GetStdHandle(handle_type);
	_tsetlocale(LC_ALL,_tsetlocale(LC_CTYPE,_T("")));
	{
		//リダイレクトされているかどうか
		DWORD mode=0;
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

		m_is_redirected=(handle_type==STD_INPUT_HANDLE)?
							!::GetConsoleMode(m_handle,&mode)
							:!::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info);
		m_orig_colors=getColors();
	}
}

Console::~Console(){
	setColors(m_orig_colors);
}

//カーソルのサイズと可視性を取得
bool Console::getCursorInfo(DWORD* size,bool* visible){
	CONSOLE_CURSOR_INFO cursor_info;
	bool result=false;

	result=::GetConsoleCursorInfo(m_handle,&cursor_info)!=0;
	if(size)*size=cursor_info.dwSize;
	if(visible)*visible=cursor_info.bVisible!=0;
	return result;
}

//カーソルのサイズと可視性を設定
bool Console::setCursorInfo(DWORD size,bool visible){
	CONSOLE_CURSOR_INFO cursor_info;

	cursor_info.dwSize=size;
	cursor_info.bVisible=visible;
	return ::SetConsoleCursorInfo(m_handle,&cursor_info)!=0;
}

//カーソルの表示/非表示の切り替え
bool Console::showCursor(bool visible){
	return setCursorInfo(25,visible);
}

//カーソルの位置を取得
bool Console::getPosition(short* x,short* y){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0)return false;
	if(x)*x=screen_buffer_info.dwCursorPosition.X;
	if(y)*y=screen_buffer_info.dwCursorPosition.Y;
	return true;
}

//カーソルの位置を取得
bool Console::getPosition(COORD* pos){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0){
		return false;
	}
	if(pos){
		pos->X=screen_buffer_info.dwCursorPosition.X;
		pos->Y=screen_buffer_info.dwCursorPosition.Y;
	}
	return true;
}

//カーソルの位置を設定
bool Console::setPosition(short x,short y){
	COORD pos;

	pos.X=x;
	pos.Y=y;
	return ::SetConsoleCursorPosition(m_handle,pos)!=0;
}

//カーソルの位置を設定
bool Console::setPosition(COORD pos){
	return ::SetConsoleCursorPosition(m_handle,pos)!=0;
}

//文字の前景(テキスト)色属性と背景色属性を取得
int Console::getColors(){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info);
	return screen_buffer_info.wAttributes;
}

//文字の前景(テキスト)色属性と背景色属性を設定
bool Console::setColors(int colors){
	return ::SetConsoleTextAttribute(m_handle,colors)!=0;
}

//文字の前景(テキスト)色属性を設定
bool Console::setFGColor(int colors){
	int current_colors=getColors();

	return setColors((current_colors&0xf0)|(colors&0x0f));
}

//文字の背景色属性を設定
bool Console::setBGColor(int colors){
	int current_colors=getColors();

	return setColors(((colors&0x0f)<<4)|(current_colors&0x0f));
}

//スクリーンバッファ座標を取得
bool Console::getScreenBufferRect(RECT* rect){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	bool result=false;

	result=::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)!=0;
	if(rect){
		rect->left=screen_buffer_info.srWindow.Left;
		rect->top=screen_buffer_info.srWindow.Top;
		rect->right=screen_buffer_info.srWindow.Right;
		rect->bottom=screen_buffer_info.srWindow.Bottom;
	}
	return result;
}

//スクリーンバッファの最大サイズを取得
bool Console::getMaximumWindowSize(COORD* pos){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	bool result=false;

	result=::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)!=0;
	if(pos)*pos=screen_buffer_info.dwMaximumWindowSize;
	return result;
}

//コンソール入力バッファの現在の入力モード、またはコンソールスクリーンバッファの現在の出力モードを取得
bool Console::getConsoleMode(LPDWORD mode){
	return ::GetConsoleMode(m_handle,mode)!=0;
}

//コンソール入力バッファの入力モード、またはコンソールスクリーンバッファの出力モードを設定
bool Console::setConsoleMode(DWORD mode){
	return ::SetConsoleMode(m_handle,mode)!=0;
}

//文字を出力
bool Console::write(const VOID *buffer,DWORD buffer_size,LPDWORD written_chars){
	bool result=false;
	DWORD written=0;

	if(isRedirected()){
		result=::WriteFile(m_handle,buffer,buffer_size*sizeof(TCHAR),&written,NULL)!=0;
		if(written_chars)*written_chars=written/sizeof(TCHAR);
	}else{
		//バッファサイズが大き過ぎる場合出力されない
//		result=::WriteConsole(m_handle,buffer,buffer_size,&written,NULL);

		//分割して出力
		const TCHAR* ptr=static_cast<const TCHAR*>(buffer);
		const TCHAR* end=ptr+lstrlen(static_cast<const TCHAR*>(buffer));

		for(DWORD write_size=write_buffer_size;ptr<end;ptr+=written){
			if(write_size>static_cast<DWORD>(end-ptr))write_size=end-ptr;

			result=::WriteConsole(m_handle,ptr,write_size,&written,NULL)!=0;
		}

		if(written_chars)*written_chars=ptr-(const TCHAR*)buffer;
	}
	return result;
}

//文字を出力
bool Console::write(const TCHAR* format,const va_list argp,LPDWORD written_chars){
	VariableArgument va(format,argp);
	const TCHAR* buffer=va.get();

	if(!buffer)return false;

	return write(buffer,lstrlen(buffer),written_chars);
}

//文字を出力
DWORD Console::outputString(const TCHAR* format,...){
	DWORD written_chars=0;

	if(format==NULL)return written_chars;

	va_list argp;
	va_start(argp,format);

	write(format,argp,&written_chars);

	va_end(argp);

	return written_chars;
}

//色付文字を出力
DWORD Console::outputString(int foreground,int background,const TCHAR* format,...){
	DWORD written_chars=0;

	if(format==NULL)return written_chars;

	int orig_colors=getColors();

	if(!isRedirected()){
		//色設定を変更する
		if(foreground!=NONE)setFGColor(foreground);
		if(background!=NONE)setBGColor(background);
	}

	va_list argp;
	va_start(argp,format);

	write(format,argp,&written_chars);

	va_end(argp);

	if(!isRedirected()){
		//色設定を元に戻す
		setColors(orig_colors);
	}
	return written_chars;
}

//指定した範囲をクリア
bool Console::clear(DWORD fill_length,COORD begin_pos){
	DWORD written_chars=0;

	::FillConsoleOutputCharacter(m_handle,
								 ' ',
								   fill_length,
								   begin_pos,
								   &written_chars);

/*
	TCHAR t[]=_T("  ");

	while(fill_length--){
		::WriteConsoleOutputCharacter(m_handle,t,ARRAY_SIZEOF(t),begin_pos,&written_chars);
		begin_pos.X+=2;
	}
*/
	return true;
}

bool Console::clear(DWORD fill_length,short x,short y){
	COORD begin_pos={x,y};

	return clear(fill_length,begin_pos);
}

bool Console::clear(DWORD fill_length){
	COORD begin_pos;

	getPosition(&begin_pos);

	return clear(fill_length,begin_pos);
}

//コンソール上の内容をすべてクリア
bool Console::clearScreen(){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0)return false;

	clear(screen_buffer_info.dwSize.X*screen_buffer_info.dwSize.Y,0,0);
	return true;
}

//行のカーソルから行末までをクリア
bool Console::killLine(){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0)return false;

	clear(screen_buffer_info.dwSize.X-screen_buffer_info.dwCursorPosition.X,
		  screen_buffer_info.dwCursorPosition.X,
		  screen_buffer_info.dwCursorPosition.Y
	);

	return true;
}

//行のカーソルから文末までをクリア
bool Console::killSentence(){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0)return false;

	clear(screen_buffer_info.dwSize.X*screen_buffer_info.dwSize.Y,
		  screen_buffer_info.dwCursorPosition.X,
		  screen_buffer_info.dwCursorPosition.Y
	);

	return true;
}

//カーソルがある行をクリア
bool Console::clearCurrentLine(){
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

	if(::GetConsoleScreenBufferInfo(m_handle,&screen_buffer_info)==0)return false;

	clear(screen_buffer_info.dwSize.X,
		  0,
		  screen_buffer_info.dwCursorPosition.Y);

	return true;
}

//コンソール上での文字列の幅を取得
int Console::getStringWidth(const TCHAR*str){
	int result=0;
	int length=lstrlen(str);
	WORD* char_type=new WORD[length];

	if(::GetStringTypeEx(0,CT_CTYPE3,str,length,char_type)){
		for(int i=0;i<length;i++){
#ifndef UNICODE
			if(::IsDBCSLeadByte(str[i])){
				++i;
				continue;
			}
#endif

			//半角であるかどうか
			bool half_width=((char_type[i]&C3_HALFWIDTH)||(char_type[i]&C3_ALPHA)||(char_type[i]&C3_DIACRITIC))&&
							!(char_type[i]&C3_HIRAGANA)&&!(char_type[i]&C3_IDEOGRAPH)&&!(char_type[i]&C3_FULLWIDTH)&&
								//'Ω'などはC3_ALPHAと等しい
								char_type[i]!=C3_ALPHA&&
								//濁点半濁点付全角カタカナと半角カタカナの濁点半濁点判定
								!((char_type[i]&C3_DIACRITIC)&&(char_type[i]&C3_KATAKANA)&&(char_type[i]&C3_ALPHA)&&!(char_type[i]&C3_HALFWIDTH));

			if(IS_HIGH_SURROGATE(str[i])&&IS_LOW_SURROGATE(str[i+1])){
//			wchar_t wch[]={data[i],data[++i],'\0'};
				++i;
			}
			result+=(half_width)?1:2;
		}
	}

	delete[] char_type;

	return result;
}


//namespace sslib
}
