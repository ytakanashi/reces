//recesBase.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r33 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _RECESBASE_H_814BA2FA_E363_4fd0_97FC_32334E1D1C87
#define _RECESBASE_H_814BA2FA_E363_4fd0_97FC_32334E1D1C87

#include"ArcCfg.h"
#include"Msg.h"
#include"ArcDll.h"
#include"ArcB2e.h"
#include"Spi.h"
#include"Wcx.h"
#include"PrivateProfile.h"

class RecesBase{
public:
	RecesBase(){}
	virtual ~RecesBase(){}

protected:
	enum ARC_RESULT{
		//処理成功
		ARC_SUCCESS,

		//対応していない書庫形式
		ARC_NOT_SUPPORTED_FORMAT,
		//SFX+Sは選択できない
		ARC_INCORRECT_OPTION_SFX_S,
		//ライブラリの読み込みに失敗
		ARC_CANNOT_LOAD_LIBRARY,
		//他のプロセスで使用中
		ARC_LIBRARY_IS_BUSY,
		//ディレクトリの作成に失敗
		ARC_CANNOT_CREATE_DIRECTORY,
		//作成する書庫と同名のディレクトリが存在する
		ARC_DUPLICATE_NAME,

		//パスワードの入力がキャンセルされた
		ARC_INPUT_PASSWORD_CANCEL,

		//C-cによりキャンセルされた
		ARC_USER_CANCEL,

		//処理失敗
		ARC_FAILURE,
	};

	static std::vector<ArcDll*> m_arcdll_list;

	//B2e.dllは特別扱い
	static ArcB2e* m_b2e_dll;
	//b2eのあるディレクトリ(フルパス)
	static tstring m_b2e_dir;

	//対応外のライブラリ(ms/Sでのみ対応)
	static ArcDll* m_cal_dll;

	static std::vector<Spi*> m_spi_list;

	static std::vector<Wcx*> m_wcx_list;

	//分割/結合用一時ディレクトリ
	static tstring m_split_temp_dir;

	//処理前のカレントディレクトリ
	static tstring m_original_cur_dir;

	//処理中ファイルの諸々の情報
	struct CUR_FILE{
		tstring arc_path;
		bool auto_renamed;
		tstring recompress_mhd;

		CUR_FILE():arc_path(),auto_renamed(false),recompress_mhd(){}
		CUR_FILE(const tstring& path,bool renamed=false,const tstring& mhd=_T("")):arc_path(path),auto_renamed(renamed),recompress_mhd(mhd){}
	};

	static CUR_FILE m_cur_file;

	//動作中ライブラリ
	static Archiver* m_arc_dll;

	static sslib::misc::thread::INFO m_progressbar_thread;

protected:
	//m_arcdll_list読み込み
	void loadArcLib();
	//m_arcdll_list解放
	void freeArcLib();
	//読み込みと対応チェック
	template<typename I>Archiver* loadAndCheck(I ite,I end,const TCHAR* arc_path=NULL,bool* loaded_library=NULL,const TCHAR* ext=NULL,const TCHAR* libname=NULL,const TCHAR* full_libname=NULL){
		for(;!IS_TERMINATED&&ite!=end;++ite){
			if(!(ext&&!(*ite)->isSupportedExtension(ext))&&
			   !(!ext&&libname!=NULL&&!sslib::str::isEqualStringIgnoreCase((*ite)->name(),libname))){
				if(!(!(*ite)->isLoaded()&&!(*ite)->load((full_libname!=NULL)?full_libname:libname,NULL))){
					if(loaded_library!=NULL)*loaded_library=true;
					if(!(arc_path!=NULL&&!(*ite)->isSupportedArchive(arc_path))){
						return *ite;
					}else{
						if(libname!=NULL)return NULL;
					}
				}
			}
		}
		return NULL;
	}
	//spiやwcxなどプラグインの読み込みと対応チェック
	template<typename T>Archiver* loadAndCheckPlugin(std::vector<T*>* plugin_list,const TCHAR* arc_path,bool* loaded_library,const tstring& plugin_dir,const TCHAR* libname,Archiver::ARC_TYPE type){
		//フルパスを取得
		std::vector<TCHAR> full_path(MAX_PATH);

		if(sslib::path::getFullPath(&full_path[0],full_path.size(),libname)&&
		   sslib::path::fileExists(&full_path[0])){
			T* plugin=new T(&full_path[0]);

			if(plugin->type()==type){
				if(plugin_list!=NULL)plugin_list->insert(plugin_list->begin(),plugin);
			}else{
				SAFE_DELETE(plugin);
				return NULL;
			}
		}else{
			//プラグインディレクトリ以下にあると仮定
			if(!plugin_dir.empty()&&
				sslib::path::getFullPath(&full_path[0],full_path.size(),libname,plugin_dir.c_str())){
			}
		}
		return loadAndCheck(plugin_list->begin(),
							plugin_list->end(),
							arc_path,
							loaded_library,
							NULL,
							&full_path[0]);
	}
	//'od'と'of'を反映した作成する書庫のパスを作成
	ARC_RESULT updateArcFileName(CUR_FILE* new_cur_file,const tstring& arc_path,tstring& err_msg);
	//ファイルのフルパスリストを作成
	bool fullPathList(std::list<tstring>& list,std::vector<tstring>& filepaths,bool received=true);
};

extern const UINT WM_HOOKDIALOG;
extern const UINT WM_CREATE_PROGRESSBAR;
extern const UINT WM_UPDATE_PROGRESSBAR_MAIN;
extern const UINT WM_DESTROY_PROGRESSBAR;

tstring removeTailCharacter(const tstring& str,TCHAR c);
//拡張子を取得(tar系考慮)、含まれなければ""を返す
tstring getExtensionEx(const tstring& file_path);
//拡張子を削除(tar系考慮)
tstring removeExtensionEx(const tstring& file_path);

#endif //_RECESBASE_H_814BA2FA_E363_4fd0_97FC_32334E1D1C87
