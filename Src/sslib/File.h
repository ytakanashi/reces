//File.h

#ifndef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
#define _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869



namespace sslib{
class File{
public:
	enum CODEPAGE{
		SJIS=1<<0,
		UTF8=1<<1,
		UTF16LE=1<<2,
		UTF16BE=1<<3,

		//for writeEx()[writeBom()]
		NO_BOM=1<<4
	};

	//readList
	enum{
		RL_SKIP_BOM=1<<0,
		RL_REMOVE_QUOTES=1<<1
	};

public:
	File();
	File(HANDLE handle,CODEPAGE codepage=SJIS);
	File(const TCHAR* file_path,
			  DWORD creation_disposition=OPEN_EXISTING,
			  DWORD desired_access=GENERIC_READ|GENERIC_WRITE,
			  DWORD share_mode=0,
			  CODEPAGE codepage=SJIS);
	virtual ~File();

protected:
	HANDLE m_file_handle;
	tstring m_file_path;
	CODEPAGE m_file_codepage;

public:
	//ファイルを開く
	bool open(const TCHAR* file_path,
			  DWORD creation_disposition=OPEN_EXISTING,
			  DWORD desired_access=GENERIC_READ|GENERIC_WRITE,
			  DWORD share_mode=0,
			  CODEPAGE codepage=SJIS);

	bool open(HANDLE handle,CODEPAGE codepage=SJIS);

	//ファイルを閉じる
	bool close();

	//ファイルからデータを読み込む
	DWORD read(void* buffer,DWORD buffer_size);

	//ファイルの中身をリストとして取得
	template<class T>bool readList(T* file_list,DWORD opt=0);

	//ファイルに書き出す
	DWORD write(const void* file_buffer,DWORD file_size)const;
	//BOMを書き込む
	void writeBom()const;
	//ファイルに書き出す(可変長引数)
	DWORD writeEx(const TCHAR* format,...)const;

	//ファイルポインタの位置を設定
	bool seek(long long offset,UINT from=FILE_BEGIN,long long* cur=NULL);
	//ファイルポインタの位置を取得
	long long tell();
	//ファイルを空にする
	bool clear();

	//ファイルサイズを取得
	long long getSize()const;

	//ファイルの更新日時/作成日時/アクセス日時を取得
	bool getFileTime(FILETIME* last_write_time,FILETIME* creation_time=NULL,FILETIME* last_access_time=NULL);

	//ファイルの更新日時/作成日時/アクセス日時を変更
	bool setFileTime(const FILETIME* last_write_time,const FILETIME* creation_time=NULL,const FILETIME* last_access_time=NULL);

	//既にファイルを開いているかどうか
	inline bool isOpened()const{return m_file_handle!=INVALID_HANDLE_VALUE;}
	inline HANDLE handle()const{return m_file_handle;}
	inline const tstring& filepath()const{return m_file_path;}
	inline CODEPAGE codepage()const{return static_cast<CODEPAGE>(m_file_codepage&~NO_BOM);}
};

//namespace sslib
}

#endif //_FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
