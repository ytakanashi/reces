//Library.h

#ifndef _PROGRESSBAR_H_B4B65DF9_7CCF_40d5_A539_C01575084085
#define _PROGRESSBAR_H_B4B65DF9_7CCF_40d5_A539_C01575084085



namespace sslib{
class Library{
public:
	Library();
	Library(const TCHAR* library_name);
	Library(const TCHAR* library_name,const TCHAR* library_prefix);
	virtual ~Library();

protected:
	HMODULE m_module;
	tstring m_library_name;
	tstring m_library_prefix;

public:
	//ライブラリを読み込む
	virtual bool load();
	virtual bool load(const TCHAR* library_name,const TCHAR* library_prefix=_T(""));
	//ライブラリを解放する
	virtual bool unload();
	//プレフィックスを設定
	void setPrefix(const TCHAR* library_prefix);
	//プレフィックスをクリア
	void clearPrefix();
	//関数のアドレスを取得
	FARPROC WINAPI getAddress(const TCHAR* proc_name_format,...);
	inline HMODULE module()const{return m_module;}
	//ライブラリ名取得
	inline const tstring name()const{return m_library_name;}
	//プレフィックスを取得
	inline const tstring prefix()const{return m_library_prefix;}
	//ライブラリ読み込み済み
	inline bool isLoaded()const{return m_module!=NULL;}
};

//namespace sslib
}

#endif //_PROGRESSBAR_H_B4B65DF9_7CCF_40d5_A539_C01575084085
