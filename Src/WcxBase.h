//WcxBase.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _WCXBASE_H_519C1AD6_FA14_49da_A03A_12B538B15665
#define _WCXBASE_H_519C1AD6_FA14_49da_A03A_12B538B15665

#include"Archiver.h"


#define E_SUCCESS          0
#define E_END_ARCHIVE     10
#define E_NO_MEMORY       11
#define E_BAD_DATA        12
#define E_BAD_ARCHIVE     13
#define E_UNKNOWN_FORMAT  14
#define E_EOPEN           15
#define E_ECREATE         16
#define E_ECLOSE          17
#define E_EREAD           18
#define E_EWRITE          19
#define E_SMALL_BUF       20
#define E_EABORTED        21
#define E_NO_FILES        22
#define E_TOO_MANY_FILES  23
#define E_NOT_SUPPORTED   24

#define PK_OM_LIST          0
#define PK_OM_EXTRACT       1

#define PK_SKIP             0
#define PK_TEST             1
#define PK_EXTRACT          2

#define PK_VOL_ASK          0
#define PK_VOL_NOTIFY       1

#define PK_PACK_MOVE_FILES  1
#define PK_PACK_SAVE_PATHS  2
#define PK_PACK_ENCRYPT     4

#define PK_CAPS_NEW         1
#define PK_CAPS_MODIFY      2
#define PK_CAPS_MULTIPLE    4
#define PK_CAPS_DELETE      8
#define PK_CAPS_OPTIONS    16
#define PK_CAPS_MEMPACK    32
#define PK_CAPS_BY_CONTENT 64
#define PK_CAPS_SEARCHTEXT 128
#define PK_CAPS_HIDE       256
#define PK_CAPS_ENCRYPT    512
#define BACKGROUND_UNPACK   1
#define BACKGROUND_PACK     2
#define BACKGROUND_MEMPACK  4

#define MEM_OPTIONS_WANTHEADERS 1

#define MEMPACK_OK          0
#define MEMPACK_DONE        1

#define PK_CRYPT_SAVE_PASSWORD 1
#define PK_CRYPT_LOAD_PASSWORD 2
#define PK_CRYPT_LOAD_PASSWORD_NO_UI 3
#define PK_CRYPT_COPY_PASSWORD 4
#define PK_CRYPT_MOVE_PASSWORD 5
#define PK_CRYPT_DELETE_PASSWORD 6

#define PK_CRYPTOPT_MASTERPASS_SET 1

typedef struct{
	char ArcName[260];
	char FileName[260];
	int Flags;
	int PackSize;
	int UnpSize;
	int HostOS;
	int FileCRC;
	int FileTime;
	int UnpVer;
	int Format;
	int FileAttr;
	char* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
}tHeaderData;

typedef struct{
	char ArcName[1024];
	char FileName[1024];
	int Flags;
	unsigned int PackSize;
	unsigned int PackSizeHigh;
	unsigned int UnpSize;
	unsigned int UnpSizeHigh;
	int HostOS;
	int FileCRC;
	int FileTime;
	int UnpVer;
	int Format;
	int FileAttr;
	char* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
	char Reserved[1024];
}tHeaderDataEx;

typedef struct{
	WCHAR ArcName[1024];
	WCHAR FileName[1024];
	int Flags;
	unsigned int PackSize;
	unsigned int PackSizeHigh;
	unsigned int UnpSize;
	unsigned int UnpSizeHigh;
	int HostOS;
	int FileCRC;
	int FileTime;
	int UnpVer;
	int Format;
	int FileAttr;
	char* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
	char Reserved[1024];
}tHeaderDataExW;

typedef struct{
	char* ArcName;
	int OpenMode;
	int OpenResult;
	char* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
}tOpenArchiveData;

typedef struct{
	WCHAR* ArcName;
	int OpenMode;
	int OpenResult;
	WCHAR* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
}tOpenArchiveDataW;

typedef struct{
	int size;
	DWORD PluginInterfaceVersionLow;
	DWORD PluginInterfaceVersionHi;
	char DefaultIniName[MAX_PATH];
}PackDefaultParamStruct;

typedef int(__stdcall*tChangeVolProc)(char *ArcName,int Mode);
typedef int(__stdcall*tChangeVolProcW)(WCHAR *ArcName,int Mode);
typedef int(__stdcall*tProcessDataProc)(char *FileName,int Size);
typedef int(__stdcall*tProcessDataProcW)(WCHAR *FileName,int Size);
typedef int(__stdcall*tPkCryptProc)(int CryptoNr,int Mode,char* ArchiveName,char* Password,int maxlen);
typedef int(__stdcall*tPkCryptProcW)(int CryptoNr,int Mode,WCHAR* ArchiveName,WCHAR* Password,int maxlen);



class WcxBase:public Archiver{
public:
	enum SPI_ERROR{
		SPI_ERROR_NOT_IMPLEMENT=-1,//その機能はインプリメントされていない
		SPI_ERROR_SUCCESS,//正常終了
		SPI_ERROR_USER_CANCEL,//コールバック関数が非0を返したので展開を中止した
		SPI_ERROR_UNKNOWN_FORMAT,//未知のフォーマット
		SPI_ERROR_BROKEN_DATA,//データが壊れている
		SPI_ERROR_MALLOC,//メモリーが確保出来ない
		SPI_ERROR_MEMORY,//メモリーエラー（Lock出来ない、等）
		SPI_ERROR_FILE_READ,//ファイルリードエラー
		SPI_ERROR_WINDOW,//窓が開けない （非公開）
		SPI_ERROR_INTERNAL,//内部エラー
		SPI_ERROR_FILE_WRITE,//ファイル書き込みエラー (非公開)
		SPI_ERROR_EOF,//ファイル終端を検出 (非公開)
	};

	enum SPI_INFO_NO{
		SPI_GET_VERSION=0,
		SPI_GET_ABOUT,
		SPI_GET_EXT,
		SPI_GET_TYPE,
	};

	enum SPI_TYPE{
		SPI_NOT=0,
		SPI_IN,
		SPI_AM,
	};

	enum SPI_INPUT_TYPE{
		SPI_INPUT_FILE=0,
		SPI_INPUT_MEMORY=1,
	};
	enum SPI_OUTPUT_TYPE{
		SPI_OUTPUT_FILE=0,
		SPI_OUTPUT_MEMORY=0x100,
	};

	typedef int(CALLBACK*SPI_CALLBACK)(int,int,long);

struct OPEN_ARC_DATA{
	OPEN_ARC_DATA():
		open_dataA(),
		open_dataW(),
		is_unicode_mode(false),
		open_mode(PK_OM_LIST){
	}
	OPEN_ARC_DATA(const TCHAR* file_name,bool unicode_mode,int open_mode=PK_OM_LIST):
		open_dataA(),
		open_dataW(),
		is_unicode_mode(unicode_mode),
		open_mode(open_mode){
			if(is_unicode_mode){
				lstrcpy(file_nameW,file_name);
				open_dataW.ArcName=file_nameW;
				open_dataW.OpenMode=open_mode;
			}else{
				lstrcpyA(file_nameA,sslib::str::utf162sjis(file_name).c_str());
				open_dataA.ArcName=file_nameA;
				open_dataA.OpenMode=open_mode;
			}
	}

	int result()const{return (is_unicode_mode)?open_dataW.OpenResult:open_dataA.OpenResult;}


	tOpenArchiveData open_dataA;
	tOpenArchiveDataW open_dataW;
	bool is_unicode_mode;
	int open_mode;
	char file_nameA[MAX_PATH];
	wchar_t file_nameW[1024];
};

struct HEADER_DATA{
	HEADER_DATA():
		hd_mode(HDA),
		header_dataA(),
		header_data_exA(),
		header_data_exW(){}

	HEADER_DATA(int hd_mode):
		hd_mode(hd_mode),
		header_dataA(),
		header_data_exA(),
		header_data_exW(){}

	enum HD_MODE{
		HDA=0,
		HDEXA,
		HDEXW,
	};

	DWORD attr(){
		int attr=0;
		DWORD result=0;

		if(hd_mode==HDA){
			attr=header_dataA.FileAttr;
		}else if(hd_mode==HDEXA){
			attr=header_data_exA.FileAttr;
		}else if(hd_mode==HDEXW){
			attr=header_data_exW.FileAttr;
		}

		if(attr&0x1)result|=FILE_ATTRIBUTE_READONLY;
		if(attr&0x2)result|=FILE_ATTRIBUTE_HIDDEN;
		if(attr&0x4)result|=FILE_ATTRIBUTE_SYSTEM;
		if(attr&0x10)result|=FILE_ATTRIBUTE_DIRECTORY;
		if(attr&0x20)result|=FILE_ATTRIBUTE_ARCHIVE;
		if(attr&0x3f)result|=FILE_ATTRIBUTE_NORMAL;

		return result;
	}

	tstring filename()const{
		if(hd_mode==HDA)return sslib::str::sjis2utf16(header_dataA.FileName);
		else if(hd_mode==HDEXA)return sslib::str::sjis2utf16(header_data_exA.FileName);
		else return tstring(header_data_exW.FileName);
	}

	long long filesize()const{
		if(hd_mode==HDA)return header_dataA.UnpSize;
		else if(hd_mode==HDEXA)return MAKEQWORD(header_data_exA.UnpSizeHigh,header_data_exA.UnpSize);
		else return MAKEQWORD(header_data_exW.UnpSizeHigh,header_data_exW.UnpSize);
	}

	int timestamp()const{
		if(hd_mode==HDA)return header_dataA.FileTime;
		else if(hd_mode==HDEXA)return header_data_exA.FileTime;
		else return header_data_exW.FileTime;
	}

	unsigned int hd_mode;
	tHeaderData header_dataA;
	tHeaderDataEx header_data_exA;
	tHeaderDataExW header_data_exW;
};

public:
	WcxBase():
		m_arc_handle(NULL),
		m_unicode_mode(false),
		m_open_arc_data(NULL),
		m_header_data(NULL){}
	WcxBase(const TCHAR* library_name):Archiver(library_name),
		m_arc_handle(NULL),
		m_unicode_mode(false),
		m_open_arc_data(NULL),
		m_header_data(NULL){}
	virtual ~WcxBase(){SAFE_DELETE(m_open_arc_data);}

private:
	HANDLE m_arc_handle;
	bool m_unicode_mode;

protected:
	inline HANDLE handle()const{return m_arc_handle;}
	OPEN_ARC_DATA* m_open_arc_data;
	HEADER_DATA* m_header_data;

public:
	HANDLE openArchiveA(tOpenArchiveData* arc_data);
	HANDLE openArchiveW(tOpenArchiveDataW* arc_data);
	HANDLE openArchive();
	int readHeader(tHeaderData* header_data);
	int readHeaderEx(tHeaderDataEx* header_data);
	int readHeaderExW(tHeaderDataExW* header_data);
	int readHeader();
	int processFileA(int operation,char*dest_path,char*dest_name);
	int processFileW(int operation,TCHAR*dest_path,TCHAR*dest_name);
	int processFile(int operation,const TCHAR*dest_path=NULL,const TCHAR*dest_name=NULL);
	bool closeArchive();
	bool configurePacker(HWND wnd_handle,HINSTANCE inst);
#if 0
	void setProcessDataProc(tProcessDataProc process_data_proc);
	void setProcessDataProcW(tProcessDataProcW process_data_proc);
#endif
	bool isUnicodeMode();
	int getHeaderDataMode();
	//既に書庫を開いているかどうか
	inline bool isOpened()const{return handle()!=NULL;}
};

#endif //_WCXBASE_H_519C1AD6_FA14_49da_A03A_12B538B15665
