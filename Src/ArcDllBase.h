//ArcDll.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCDLLBASE_H_AAC042AD_D5D0_4E78_9923_8A687752895E
#define _ARCDLLBASE_H_AAC042AD_D5D0_4E78_9923_8A687752895E

#include"Archiver.h"

#ifndef FNAME_MAX32
	#define FNAME_MAX32 512
	#define FNAME_MAX FNAME_MAX32
#else
	#ifndef FNAME_MAX
		#define	FNAME_MAX 128
	#endif
#endif //FNAME_MAX32

#ifndef CHECKARCHIVE_RAPID
	#define CHECKARCHIVE_RAPID				0
	#define CHECKARCHIVE_BASIC				1
	#define CHECKARCHIVE_FULLCRC			2

	#define CHECKARCHIVE_RECOVERY			4
	#define CHECKARCHIVE_SFX				8
	#define CHECKARCHIVE_ALL				16
	#define CHECKARCHIVE_ENDDATA			32

	#define CHECKARCHIVE_NOT_ASK_PASSWORD	64//パスワードを聞かない
#endif //CHECKARCHIVE_RAPID

#ifndef ISARC_FUNCTION_START
	#define ISARC_FUNCTION_START			0
	#define ISARC							0
	#define ISARC_GET_VERSION				1
	#define ISARC_GET_CURSOR_INTERVAL		2
	#define ISARC_SET_CURSOR_INTERVAL		3
	#define ISARC_GET_BACK_GROUND_MODE		4
	#define ISARC_SET_BACK_GROUND_MODE		5
	#define ISARC_GET_CURSOR_MODE			6
	#define ISARC_SET_CURSOR_MODE			7
	#define ISARC_GET_RUNNING				8

	#define ISARC_CHECK_ARCHIVE				16
	#define ISARC_CONFIG_DIALOG				17
	#define ISARC_GET_FILE_COUNT			18
	#define ISARC_QUERY_FUNCTION_LIST		19
	#define ISARC_HOUT						20
	#define ISARC_STRUCTOUT					21
	#define ISARC_GET_ARC_FILE_INFO			22

	#define ISARC_OPEN_ARCHIVE				23
	#define ISARC_CLOSE_ARCHIVE				24
	#define ISARC_FIND_FIRST				25
	#define ISARC_FIND_NEXT					26
	#define ISARC_EXTRACT					27
	#define ISARC_ADD						28
	#define ISARC_MOVE						29
	#define ISARC_DELETE					30
	#define ISARC_SETOWNERWINDOW			31
	#define ISARC_CLEAROWNERWINDOW			32
	#define ISARC_SETOWNERWINDOWEX			33
	#define ISARC_KILLOWNERWINDOWEX			34

	#define ISARC_GET_ARC_FILE_NAME			40
	#define ISARC_GET_ARC_FILE_SIZE			41
	#define ISARC_GET_ARC_ORIGINAL_SIZE		42
	#define ISARC_GET_ARC_COMPRESSED_SIZE	43
	#define ISARC_GET_ARC_RATIO				44
	#define ISARC_GET_ARC_DATE				45
	#define ISARC_GET_ARC_TIME				46
	#define ISARC_GET_ARC_OS_TYPE			47
	#define ISARC_GET_ARC_IS_SFX_FILE		48
	#define ISARC_GET_ARC_WRITE_TIME_EX		49
	#define ISARC_GET_ARC_CREATE_TIME_EX	50
	#define	ISARC_GET_ARC_ACCESS_TIME_EX	51
	#define	ISARC_GET_ARC_CREATE_TIME_EX2	52
	#define ISARC_GET_ARC_WRITE_TIME_EX2	53
	#define ISARC_GET_FILE_NAME				57
	#define ISARC_GET_ORIGINAL_SIZE			58
	#define ISARC_GET_COMPRESSED_SIZE		59
	#define ISARC_GET_RATIO					60
	#define ISARC_GET_DATE					61
	#define ISARC_GET_TIME					62
	#define ISARC_GET_CRC					63
	#define ISARC_GET_ATTRIBUTE				64
	#define ISARC_GET_OS_TYPE				65
	#define ISARC_GET_METHOD				66
	#define ISARC_GET_WRITE_TIME			67
	#define ISARC_GET_CREATE_TIME			68
	#define ISARC_GET_ACCESS_TIME			69
	#define ISARC_GET_WRITE_TIME_EX			70
	#define ISARC_GET_CREATE_TIME_EX		71
	#define ISARC_GET_ACCESS_TIME_EX		72
	#define ISARC_SET_ENUM_MEMBERS_PROC		80
	#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81
	#define ISARC_GET_ARC_FILE_SIZE_EX		82
	#define ISARC_GET_ARC_ORIGINAL_SIZE_EX	83
	#define ISARC_GET_ARC_COMPRESSED_SIZE_EX	84
	#define ISARC_GET_ORIGINAL_SIZE_EX		85
	#define ISARC_GET_COMPRESSED_SIZE_EX	86
	#define ISARC_SETOWNERWINDOWEX64		87
	#define ISARC_KILLOWNERWINDOWEX64		88
	#define ISARC_SET_ENUM_MEMBERS_PROC64	89
	#define ISARC_CLEAR_ENUM_MEMBERS_PROC64	90
	#define ISARC_OPEN_ARCHIVE2				91
	#define ISARC_GET_ARC_READ_SIZE			92
	#define ISARC_GET_ARC_READ_SIZE_EX		93
	#define SET_LANGUE_JAPANESE				94
	#define SET_LANGUE_ENGLISH				95
	#define SET_LANGUE_SPECIFIED			96
	#define ISARC_SET_LANGUE_SPECIFIED		97
	#define ISARC_SET_LANGUE_JAPANESE		98
	#define ISARC_SET_LANGUE_ENGLISH		99
	#define ISARC_SET_PRIORITY				100
	#define ISARC_GET_ATTRIBUTES			101
	#define ISARC_SET_CP					102
	#define ISARC_GET_CP					103
	#define ISARC_GET_LAST_ERROR			104
	#define ISARC_GET_ARC_WRITE_TIME		105
	#define ISARC_GET_ARC_CREATE_TIME		106
	#define ISARC_GET_ARC_ACCESS_TIME		107
	#define ISARC_GET_ARC_WRITE_TIME_64		108
	#define ISARC_GET_ARC_CREATE_TIME_64	109
	#define ISARC_GET_ARC_ACCESS_TIME_64	110
	#define ISARC_GET_WRITE_TIME_64			111
	#define ISARC_GET_CREATE_TIME_64		112
	#define ISARC_GET_ACCESS_TIME_64		113
	#define ISARC_SET_UNICODE_MODE			114

	#define ISARC_FUNCTION_END				ISARC_SET_UNICODE_MODE
#endif //ISARC_FUNCTION_START

#ifndef ERROR_START
	#define ERROR_START				0x8000
	#define ERROR_DISK_SPACE		0x8005
	#define ERROR_READ_ONLY			0x8006
	#define ERROR_USER_SKIP			0x8007
	#define ERROR_UNKNOWN_TYPE		0x8008
	#define ERROR_METHOD			0x8009
	#define ERROR_PASSWORD_FILE		0x800A
	#define ERROR_VERSION			0x800B
	#define ERROR_FILE_CRC			0x800C
	#define ERROR_FILE_OPEN			0x800D
	#define ERROR_MORE_FRESH		0x800E
	#define ERROR_NOT_EXIST			0x800F
	#define ERROR_ALREADY_EXIST		0x8010
	#define ERROR_TOO_MANY_FILES	0x8011
	#define ERROR_MAKEDIRECTORY		0x8012
	#define ERROR_CANNOT_WRITE		0x8013
	#define ERROR_HUFFMAN_CODE		0x8014
	#define ERROR_COMMENT_HEADER	0x8015
	#define ERROR_HEADER_CRC		0x8016
	#define ERROR_HEADER_BROKEN		0x8017
	#define ERROR_ARC_FILE_OPEN		0x8018
	#define ERROR_NOT_ARC_FILE		0x8019
	#define ERROR_CANNOT_READ		0x801A
	#define ERROR_FILE_STYLE		0x801B
	#define ERROR_COMMAND_NAME		0x801C
	#define ERROR_MORE_HEAP_MEMORY	0x801D
	#define ERROR_ENOUGH_MEMORY		0x801E
	#ifndef ERROR_ALREADY_RUNNING
		#define ERROR_ALREADY_RUNNING	0x801F
	#endif //ERROR_ALREADY_RUNNING
	#define ERROR_USER_CANCEL		0x8020
	#define ERROR_HARC_ISNOT_OPENED	0x8021
	#define ERROR_NOT_SEARCH_MODE	0x8022
	#define ERROR_NOT_SUPPORT		0x8023
	#define ERROR_TIME_STAMP		0x8024
	#define ERROR_TMP_OPEN			0x8025
	#define ERROR_LONG_FILE_NAME	0x8026
	#define ERROR_ARC_READ_ONLY		0x8027
	#define ERROR_SAME_NAME_FILE	0x8028
	#define ERROR_NOT_FIND_ARC_FILE 0x8029
	#define ERROR_RESPONSE_READ		0x802A
	#define ERROR_NOT_FILENAME		0x802B
	#define ERROR_TMP_COPY			0x802C
	#define ERROR_EOF				0x802D
	#define ERROR_ADD_TO_LARC		0x802E
	#define ERROR_TMP_BACK_SPACE	0x802F
	#define ERROR_SHARING			0x8030
	#define ERROR_NOT_FIND_FILE		0x8031
	#define ERROR_LOG_FILE			0x8032
	#define	ERROR_NO_DEVICE			0x8033
	#define ERROR_GET_ATTRIBUTES	0x8034
	#define ERROR_SET_ATTRIBUTES	0x8035
	#define ERROR_GET_INFORMATION	0x8036
	#define ERROR_GET_POINT			0x8037
	#define ERROR_SET_POINT			0x8038
	#define ERROR_CONVERT_TIME		0x8039
	#define ERROR_GET_TIME			0x803a
	#define ERROR_SET_TIME			0x803b
	#define ERROR_CLOSE_FILE		0x803c
	#define ERROR_HEAP_MEMORY		0x803d
	#define ERROR_HANDLE			0x803e
	#define ERROR_TIME_STAMP_RANGE	0x803f
	#define ERROR_MAKE_ARCHIVE		0x8040
	#define ERROR_NOT_CONFIRM_NAME	0x8041
	#define ERROR_UNEXPECTED_EOF	0x8042
	#define ERROR_INVALID_END_MARK	0x8043
	#define ERROR_INVOLVED_LZH		0x8044
	#define ERROR_NO_END_MARK		0x8045
	#define ERROR_HDR_INVALID_SIZE	0x8046
	#define ERROR_UNKNOWN_LEVEL		0x8047
	#define ERROR_BROKEN_DATA		0x8048
	#define ERROR_INVALID_PATH		0x8049
	#define ERROR_TOO_BIG			0x804A
	#define ERROR_EXECUTABLE_FILE	0x804B
	#define ERROR_INVALID_VALUE		0x804C
	#define ERROR_HDR_EXPLOIT		0x804D
	#define ERROR_HDR_NO_CRC		0X804E
	#define ERROR_HDR_NO_NAME		0X804F

	#define ERROR_END	ERROR_END
#endif //ERROR_START

#ifndef UNPACK_CONFIG_MODE
	#define	UNPACK_CONFIG_MODE	1
	#define	PACK_CONFIG_MODE	2
#endif

#ifndef EXTRACT_FOUND_FILE
	#define M_INIT_FILE_USE			0x00000001L
	#define M_REGARDLESS_INIT_FILE	0x00000002L
	#define M_NO_BACKGROUND_MODE	0x00000004L
	#define M_NOT_USE_TIME_STAMP	0x00000008L
	#define M_EXTRACT_REPLACE_FILE	0x00000010L
	#define M_EXTRACT_NEW_FILE		0x00000020L
	#define M_EXTRACT_UPDATE_FILE	0x00000040L
	#define M_CHECK_ALL_PATH		0x00000100L
	#define M_CHECK_FILENAME_ONLY	0x00000200L
	#define M_CHECK_DISK_SIZE		0x00000400L
	#define M_REGARDLESS_DISK_SIZE	0x00000800L
	#define M_USE_DRIVE_LETTER		0x00001000L
	#define M_NOT_USE_DRIVE_LETTER	0x00002000L
	#define M_INQUIRE_DIRECTORY		0x00004000L
	#define M_NOT_INQUIRE_DIRECTORY 0x00008000L
	#define M_INQUIRE_WRITE			0x00010000L
	#define M_NOT_INQUIRE_WRITE		0x00020000L
	#define M_CHECK_READONLY		0x00040000L
	#define M_REGARDLESS_READONLY	0x00080000L
	#define M_REGARD_E_COMMAND		0x00100000L
	#define M_REGARD_X_COMMAND		0x00200000L
	#define M_ERROR_MESSAGE_ON		0x00400000L
	#define M_ERROR_MESSAGE_OFF		0x00800000L
	#define M_BAR_WINDOW_ON			0x01000000L
	#define M_BAR_WINDOW_OFF		0x02000000L
	#define M_CHECK_PATH			0x04000000L
	#define M_RECOVERY_ON			0x08000000L
	#define M_MAKE_INDEX_FILE		0x10000000L
	#define M_NOT_MAKE_INDEX_FILE	0x20000000L
	#define EXTRACT_FOUND_FILE		0x40000000L
	#define EXTRACT_NAMED_FILE		0x80000000L
#endif

#ifndef FA_RDONLY
	#define FA_RDONLY		0x01
	#define FA_HIDDEN		0x02
	#define FA_SYSTEM		0x04
	#define FA_LABEL		0x08
	#define FA_DIREC		0x10
	#define FA_ARCH			0x20
#endif
	#ifndef FA_ENCRYPTED
	#define FA_ENCRYPTED	0x40
#endif

#if defined(__BORLANDC__)
	#pragma option -a-
#else
	#pragma pack(1)
#endif

typedef	HGLOBAL	HARCHIVE;

#ifndef ARC_DECSTRACT
	#define ARC_DECSTRACT
	typedef	HGLOBAL	HARC;

	typedef struct{
		DWORD dwOriginalSize;
		DWORD dwCompressedSize;
		DWORD dwCRC;
		UINT uFlag;
		UINT uOSType;
		WORD wRatio;
		WORD wDate;
		WORD wTime;
		char szFileName[FNAME_MAX32+1];
		char dummy1[3];
		char szAttribute[8];
		char szMode[8];
	}INDIVIDUALINFO,FAR*LPINDIVIDUALINFO;

	typedef struct{
		DWORD dwFileSize;
		DWORD dwWriteSize;
		char szSourceFileName[FNAME_MAX32+1];
		char dummy1[3];
		char szDestFileName[FNAME_MAX32+1];
		char dummy[3];
	}EXTRACTINGINFO,FAR*LPEXTRACTINGINFO;

	typedef struct{
		EXTRACTINGINFO exinfo;
		DWORD dwCompressedSize;
		DWORD dwCRC;
		UINT uOSType;
		WORD wRatio;
		WORD wDate;
		WORD wTime;
		char szAttribute[8];
		char szMode[8];
	}EXTRACTINGINFOEX,*LPEXTRACTINGINFOEX;

	typedef struct{
		DWORD dwStructSize;
		EXTRACTINGINFO exinfo;
		LONGLONG llFileSize;
		LONGLONG llCompressedSize;
		LONGLONG llWriteSize;
		DWORD dwAttributes;
		DWORD dwCRC;
		UINT uOSType;
		WORD wRatio;
		FILETIME ftCreateTime;
		FILETIME ftAccessTime;
		FILETIME ftWriteTime;
		char szMode[8];
		char szSourceFileName[FNAME_MAX32+1];
		char dummy1[3];
		char szDestFileName[FNAME_MAX32+1];
		char dummy2[3];
	}EXTRACTINGINFOEX64,*LPEXTRACTINGINFOEX64;
#endif

#if !defined(__BORLANDC__)
	#pragma pack()
#else
	#pragma option -a.
#endif

#if !defined(__BORLANDC__)
	#define	_export
#endif

#ifndef WM_ARCEXTRACT
	#define	WM_ARCEXTRACT			"wm_arcextract"
	#define	ARCEXTRACT_BEGIN		0
	#define	ARCEXTRACT_INPROCESS	1
	#define	ARCEXTRACT_END			2
	#define ARCEXTRACT_OPEN			3
	#define ARCEXTRACT_COPY			4
	typedef BOOL CALLBACK ARCHIVERPROC(HWND,UINT,UINT,LPEXTRACTINGINFOEX);
	typedef ARCHIVERPROC* LPARCHIVERPROC;
#endif

class ArcDllBase:public Archiver{
public:
	ArcDllBase();
	ArcDllBase(const TCHAR* library_name,const TCHAR* library_prefix);
	virtual ~ArcDllBase();

public:
	//ライブラリを読み込みます
	virtual bool load();
	virtual bool load(const TCHAR* library_name,const TCHAR* library_prefix);
	//ライブラリを解放します
	virtual bool unload();

private:
	HARC m_arc_handle;
	bool m_unicode_mode;

private:
	inline HARC handle()const{return m_arc_handle;}

protected:
	//関数対応を操作
	virtual bool querySupport(int func);

public:
	//引数を渡して実行
	int execute(const HWND wnd_handle,const TCHAR* cmdline,tstring* result_buffer,const DWORD buffer_size);
	//DLLのバージョンを返します(現在のバージョンの上位2ワードに100を掛けたものが返ってきます)
	//ライブラリ本体の戻り値/引数と異なる事に注意
	bool getVersion(WORD* version);
	//DLLのバージョンを返します(現在のバージョンの下位2ワードに100を掛けたものが返ってきます)
	//ライブラリ本体の戻り値/引数と異なる事に注意
	bool getSubVersion(WORD* version);
#if 0
	//DLLの動作中にカーソルを表示するモードか否かを設定します
	bool setCursorMode(const bool cursor_info);
#endif
	//現在DLLが動作中か否かを得ます
	bool getRunning();
#if 0
	//DLLがバックグラウンドモードか否かを得ます
	bool getBackGroundMode();
	//DLLのバックグラウンドモードを設定します
	bool setBackGroundMode(const bool back_ground_mode);
#endif
	//指定ファイルがサポートしている書庫ファイルとして正しいかどうかを返します
	bool checkArchive(const TCHAR* file_name,const int mode=CHECKARCHIVE_BASIC);
	//指定された書庫ファイルに格納されているファイル数を得ます
//	int getFileCount(const TCHAR* arc_file);
	//指定したアーカイブファイルの形式を得ます
	int getArchiveType(const TCHAR* file_name);
	//指定されたAPIが使用可能かどうかを得ます
	bool queryFunctionList(const int function);
	//オプション指定ダイアログの表示
	bool configDialog(HWND wnd_handle=NULL,tstring* result_buffer=NULL,const int mode=0);
	//指定ファイルが有効な書庫ファイルかどうか調べ、有効であればオープンし、一意の数値（ハンドル）を返します
	HARC openArchive(const HWND wnd_handle,const TCHAR* file_name,const DWORD mode);
	//OpenArchive()で割り付けたハンドルを解放する
	bool closeArchive();
	//最初の格納ファイルの情報を得ます
	int findFirst(const TCHAR* wild_name,LPINDIVIDUALINFO info);
	//2番目以降の格納ファイルの情報を返す
	int findNext(LPINDIVIDUALINFO info);
#if 0
	//DLLのメッセージの送信先ウィンドウを設定します
	bool setOwnerWindow(HWND wnd_handle);
	//SetOwnerWindow()で設定したウィンドウの設定を解除します
	bool clearOwnerWindow();
#endif
	//DLLの圧縮解凍状況を受け取るためのコールバック関数の指定を行います
	bool setOwnerWindowEx(HWND wnd_handle,LPARCHIVERPROC arc_proc);
	//SetOwnerWindowEx()で設定したウィンドウの設定を解除します
	bool killOwnerWindowEx(HWND wnd_handle);
	//次の格納ファイルの情報を得ます
	int getFileName(tstring* result_buffer,const int buffer_size);
	//格納ファイルのサイズを64ビット整数で得ます
	bool getOriginalSizeEx(__int64* size);
#if 0
	//格納ファイルの圧縮サイズを得ます
	bool getCompressedSizeEx(__int64* size);
#endif
	//格納ファイルの属性を得ます
	bool getAttribute(DWORD* p_attr,int* p_orig_attr=NULL);
	//書庫の情報を得る際に使用する標準パスワードを設定します。
	int setDefaultPassword(const TCHAR*password);
	//使用する文字コードをUTF-8に設定、及び解除を行います(7-zip32)
	//ANSI 版 API を UTF-8 の文字コードで使用するかどうかを指定します(UNLHA32.DLL)
	bool setUnicodeMode(bool unicode);
	//DLLの圧縮解凍状況を受け取るためのコールバック関数の指定を行います
	bool setOwnerWindowEx64(const HWND wnd_handle,LPARCHIVERPROC arc_proc,const DWORD struct_size);
	//SetOwnerWindowEx64()で設定したウィンドウの設定を解除します
	bool killOwnerWindowEx64(HWND wnd_handle);

	//UnicodeModeが有効かどうか
	inline bool isUnicodeMode()const{return m_unicode_mode;}
	//既に書庫を開いているかどうか
	inline bool isOpened()const{return handle()!=NULL;}
};
#endif //_ARCDLLBASE_H_AAC042AD_D5D0_4E78_9923_8A687752895E
