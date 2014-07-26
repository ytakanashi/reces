//CfgFile.h

#ifndef _CFGFILE_H_741B8F63_0A1A_4051_9F45_C99B6BF7A95D
#define _CFGFILE_H_741B8F63_0A1A_4051_9F45_C99B6BF7A95D



namespace sslib{
class CfgFile{
public:
	CfgFile();
	CfgFile(const TCHAR* cfg_path);
	virtual ~CfgFile();

private:
	bool m_empty;

protected:
	tstring m_cfg_path;

public:
	//cfgファイル名を設定
	void setFileName(const TCHAR* cfg_name);

	//キーが存在するか
	bool keyExists(const TCHAR* section,const TCHAR* key);

	//GetPrivateProfileInt()でデータを取得
	template<typename T>
	bool getData(const TCHAR* section,const TCHAR* key,T* result,const T default_value=(T)0){
		*result=static_cast<T>(::GetPrivateProfileInt(section,key,default_value,m_cfg_path.c_str()));
		return keyExists(section,key);
	}

	//GetPrivateProfileString()でデータを取得
	bool getStringData(const TCHAR* section,const TCHAR* key,TCHAR* result,int result_buffer,const TCHAR* default_value=NULL);
	bool getStringData(const TCHAR* section,const TCHAR* key,tstring* result,const TCHAR* default_value=NULL);

	//存在するならGetPrivateProfileInt()でデータを取得
	template<typename T>
	bool getDataEx(const TCHAR* section,const TCHAR* key,T* result,const T default_value=(T)0){
		if(keyExists(section,key)){
			*result=static_cast<T>(::GetPrivateProfileInt(section,key,default_value,m_cfg_path.c_str()));
		}
		return keyExists(section,key);
	}

	//存在するならGetPrivateProfileString()でデータを取得
	bool getStringDataEx(const TCHAR* section,const TCHAR* key,TCHAR* result,int result_buffer,const TCHAR* default_value=NULL);
	bool getStringDataEx(const TCHAR* section,const TCHAR* key,tstring* result,const TCHAR* default_value=NULL);

	//データを設定
	bool setData(const TCHAR* section,const TCHAR* key,int value);

	//データ(文字列)を設定
	bool setData(const TCHAR* section,const TCHAR* key,const TCHAR* value);

	//キーを削除
	inline bool removeKey(const TCHAR* section,const TCHAR* key){
		return (path::fileExists(filepath().c_str()))?
			(::WritePrivateProfileString(section,key,NULL,filepath().c_str())!=0):true;
	}

	//セクションを削除
	inline bool removeSection(const TCHAR* section){
		return (path::fileExists(filepath().c_str()))?
			(::WritePrivateProfileString(section,NULL,NULL,filepath().c_str())!=0):true;
	}

	//セクション内にキーがない
	bool isEmptySection(const TCHAR* section){
		std::vector<TCHAR> buffer(128);

		::GetPrivateProfileString(section,NULL,NULL,&buffer[0],buffer.size(),filepath().c_str());
		return !strvalid(&buffer[0]);
	}

	inline const tstring filepath()const{return m_cfg_path;}
};

//namespace sslib
}

#endif //_CFGFILE_H_741B8F63_0A1A_4051_9F45_C99B6BF7A95D
