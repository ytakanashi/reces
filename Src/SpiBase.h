//SpiBase.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SPIBASE_H_D89DCC07_6B0E_47f3_A2DB_FA78710F5EC3
#define _SPIBASE_H_D89DCC07_6B0E_47f3_A2DB_FA78710F5EC3


#include"Archiver.h"

class SpiBase:public Archiver{
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

	enum SPI_INPUT_TYPE{
		SPI_INPUT_FILE=0,
		SPI_INPUT_MEMORY=1,
	};
	enum SPI_OUTPUT_TYPE{
		SPI_OUTPUT_FILE=0,
		SPI_OUTPUT_MEMORY=0x100,
	};

	typedef int(CALLBACK*SPI_CALLBACK)(int,int,long);

typedef struct fileInfo{
	unsigned char method[8];						//圧縮法の種類
	unsigned long position;							//ファイル上での位置
	unsigned long compsize;							//圧縮されたサイズ
	unsigned long filesize;							//元のファイルサイズ
	long timestamp;									//ファイルの更新日時
	char path[200];									//相対パス
	char filename[200];								//ファイルネーム
	unsigned long crc;								//CRC
}fileInfo;

public:
	SpiBase(){}
	SpiBase(const TCHAR* library_name):Archiver(library_name){}
	virtual ~SpiBase(){}

public:
	//Plug-inに関する情報を得る
	int getPluginInfo(int info_no,tstring* result_buffer,int buffer_length);
	//展開可能な(対応している)ファイル形式か調べる。
	int isSupported(const TCHAR* file_path,DWORD param);
	//アーカイブ内のすべてのファイルの情報を取得する(ファイル入力)
	SPI_ERROR getArchiveInfo(const TCHAR* file_path,long buffer_length,unsigned int flag,HLOCAL* info);
#if 0
	//アーカイブ内の指定したファイルの情報を取得する
	SPI_ERROR getFileInfo(const TCHAR* file_path,long buffer_length,char* dir_path,unsigned int flag,fileInfo* file_info);
#endif
	//アーカイブ内のファイルを取得する
	SPI_ERROR getFile(const TCHAR* file_path,long buffer_length,void* dest,unsigned int flag,SPI_CALLBACK callback,long data);

	//Plug-in設定ダイアログの表示 
	int ConfigurationDlg(HWND wnd_handle=NULL,int fnc=1);
};

#endif //_SPIBASE_H_D89DCC07_6B0E_47f3_A2DB_FA78710F5EC3
