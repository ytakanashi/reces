//Console.h

#ifndef _CONSOLE_H_112DA473_5860_4F65_879F_E1C937920096
#define _CONSOLE_H_112DA473_5860_4F65_879F_E1C937920096



namespace sslib{
class Console{
private:
	Console(const Console&);
	Console& operator=(const Console&);
public:
	Console(DWORD handle_type=STD_OUTPUT_HANDLE);
	virtual ~Console();

private:
	HANDLE m_handle;
	bool m_is_redirected;
	int m_orig_colors;
	const DWORD io_buffer_size;
#ifdef UNICODE
	bool m_ansi_mode;
#endif

public:
	enum{
		NONE=-1,
		LOW_BLACK=0,
		LOW_BLUE,
		LOW_GREEN,
		LOW_CYAN,
		LOW_RED,
		LOW_PURPLE,
		LOW_YELLOW,
		LOW_WHITE,

		HIGH_BLACK,
		HIGH_BLUE,
		HIGH_GREEN,
		HIGH_CYAN,
		HIGH_RED,
		HIGH_PURPLE,
		HIGH_YELLOW,
		HIGH_WHITE,
	};

public:
	//カーソルのサイズと可視性を取得
	bool getCursorInfo(DWORD* size,bool* visible=NULL);

	//カーソルのサイズと可視性を設定
	bool setCursorInfo(DWORD size,bool visible);

	//カーソルの表示/非表示の切り替え
	bool showCursor(bool visible);

	//カーソルの位置を取得
	bool getPosition(short* x,short* y);
	bool getPosition(COORD* pos);

	//カーソルの位置を設定
	bool setPosition(short x,short y);
	bool setPosition(COORD pos);

	//文字の前景(テキスト)色属性と背景色属性を取得
	int getColors();

	//文字の前景(テキスト)色属性と背景色属性を設定
	bool setColors(int colors);

	//文字の前景(テキスト)色属性を設定
	bool setFGColor(int colors);

	//文字の背景色属性を設定
	bool setBGColor(int colors);

	//スクリーンバッファ座標を取得
	bool getScreenBufferRect(RECT* rect);

	//スクリーンバッファの最大サイズを取得
	bool getMaximumWindowSize(COORD* pos);

	//コンソール入力バッファの現在の入力モード、またはコンソールスクリーンバッファの現在の出力モードを取得
	bool getConsoleMode(LPDWORD mode);

	//コンソール入力バッファの入力モード、またはコンソールスクリーンバッファの出力モードを設定
	bool setConsoleMode(DWORD mode);

	//文字を読み込む
	bool read(VOID* buffer,DWORD buffer_size,LPDWORD read_chars=NULL);

	//文字を書き込む
	bool write(const VOID* buffer,DWORD buffer_size,LPDWORD written_chars=NULL);
	bool write(const TCHAR* fmt,const va_list argp,LPDWORD written_chars=NULL);

	//文字を出力
	DWORD outputString(const TCHAR* fmt,...);
	//色付文字を出力
	DWORD outputString(int foreground,int background,const TCHAR* fmt,...);

	//指定した範囲をクリア
	bool clear(DWORD fill_length,COORD begin_pos);
	bool clear(DWORD fill_length,short shX,short shY);
	bool clear(DWORD fill_length);

	//コンソール上の内容をすべてクリア
	bool clearScreen();

	//行のカーソルから行末までをクリア
	bool killLine();

	//行のカーソルから文末までをクリア
	bool killSentence();

	//カーソルがある行をクリア
	bool clearCurrentLine();

	//ハンドルを取得
	inline HANDLE handle()const{return m_handle;}

	//標準出力がリダイレクトされている
	inline bool isRedirected()const{return m_is_redirected;}

	//文字の前景(テキスト)色属性と背景色属性を元に戻す
	inline bool resetColors(){
		return setColors(m_orig_colors);
	}

#ifdef UNICODE
	//ANSI(sjis)に変換して文字を出力するモード
	inline void setAnsiMode(bool ansi_mode){m_ansi_mode=ansi_mode;}
	inline bool isAnsiMode()const{return m_ansi_mode;}
#endif
};

//namespace sslib
}

#endif //_CONSOLE_H_112DA473_5860_4F65_879F_E1C937920096
