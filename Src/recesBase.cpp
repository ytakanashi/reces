//reces.cpp
//recesベース

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"reces.h"
#include"ArcTar32.h"
#include"Arc7-zip32.h"
#include"ArcUnrar32.h"
#include"ArcUnlha32.h"
#ifndef _WIN64
#include"ArcUniso32.h"
#include"ArcXacrett.h"
#endif
#include"Spi.h"
#include"Wcx.h"
#include"ParseOptions.h"


using namespace sslib;


//あかんやつ
std::vector<ArcDll*> RecesBase::m_arcdll_list;
ArcB2e* RecesBase::m_b2e_dll;
tstring RecesBase::m_b2e_dir;
ArcDll* RecesBase::m_cal_dll;
std::vector<Spi*> RecesBase::m_spi_list;
std::vector<Wcx*> RecesBase::m_wcx_list;
tstring RecesBase::m_split_temp_dir;
tstring RecesBase::m_original_cur_dir;
RecesBase::CUR_FILE RecesBase::m_cur_file;
Archiver* RecesBase::m_arc_dll;
misc::thread::INFO RecesBase::m_progressbar_thread;


const UINT WM_HOOKDIALOG=::RegisterWindowMessage(_T("_WM_HOOKDIALOG_"));
const UINT WM_CREATE_PROGRESSBAR=::RegisterWindowMessage(_T("WM_CREATE_PROGRESSBAR"));
const UINT WM_UPDATE_PROGRESSBAR_MAIN=::RegisterWindowMessage(_T("WM_UPDATE_PROGRESSBAR_MAIN"));
const UINT WM_DESTROY_PROGRESSBAR=::RegisterWindowMessage(_T("WM_DESTROY_PROGRESSBAR"));

tstring removeTailCharacter(const tstring& str,TCHAR c){
	if(str.empty())return str;

	tstring result(str);

	if(result.find_last_of(c)==result.length()-1){
		result=result.substr(0,result.length()-1);
	}
	return result;
}

//拡張子を取得(tar系考慮)、含まれなければ""を返す
tstring getExtensionEx(const tstring& file_path,const tstring& ext){
	if(file_path.empty())return file_path;

	tstring modified_path(path::removeExtension(file_path));

	if(modified_path==file_path)return _T("");

	//拡張子が'.tar.xz'等の場合、二度removeExtension()する
	if(path::getExtension(modified_path)==ext){
		modified_path=path::removeExtension(modified_path);
	}
	return file_path.substr(modified_path.length()+1);
}

//拡張子を削除(tar系考慮)
tstring removeExtensionEx(const tstring& file_path,const tstring& ext){
	if(file_path.empty())return file_path;

	tstring modified_path(path::removeExtension(file_path));

	//拡張子が'.tar.xz'等の場合、二度removeExtension()する
	if(path::getExtension(modified_path)==ext){
		return path::removeExtension(modified_path);
	}
	return modified_path;
}


//'od'と'of'を反映した作成する書庫のパスを作成
RecesBase::ARC_RESULT RecesBase::updateArcFileName(CUR_FILE* new_cur_file,const tstring& arc_path,tstring& err_msg){
	//オプションの有無にかかわらず、出力先ディレクトリをoutput_dirに代入
	tstring output_dir(path::getParentDirectory(arc_path));
	new_cur_file->arc_path.assign(arc_path);

	if(!CFG.general.output_dir.empty()){
		//'od'
		if(path::isRelativePath(CFG.general.output_dir.c_str())||
		   !path::isAbsolutePath(CFG.compress.output_file.c_str())){
			//相対パスであれば絶対パスを作成
			std::vector<TCHAR> buffer(MAX_PATHW);

			path::getFullPath(&buffer[0],
							  buffer.size(),
							  CFG.general.output_dir.c_str(),
							  ((!CFG.general.default_base_dir)?
							   output_dir.c_str():
							   m_original_cur_dir.c_str()));
			output_dir.assign(&buffer[0]);
		}else{
			//絶対パス[\Dir含む]であればそのまま代入
			output_dir=CFG.general.output_dir;
		}

		if(!path::fileExists(output_dir.c_str())){
			if(!fileoperation::createDirectory(output_dir.c_str())){
				err_msg.assign(format(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str()));
				return ARC_CANNOT_CREATE_DIRECTORY;
			}
		}

		if(CFG.compress.output_file.empty()){
			//'of'が無効であればarc_pathを作成
			output_dir=path::addTailSlash(output_dir);
			new_cur_file->arc_path=output_dir+path::getFileName(arc_path);
		}
	}

	if(!CFG.compress.output_file.empty()){
		//'of'
		if(path::isRelativePath(CFG.compress.output_file.c_str())||
		   !path::isAbsolutePath(CFG.compress.output_file.c_str())){
			//相対パスであれば絶対パスを作成
			std::vector<TCHAR> buffer(MAX_PATHW);

			path::getFullPath(&buffer[0],
							  buffer.size(),
							  CFG.compress.output_file.c_str(),
							  ((!CFG.general.default_base_dir)?
							   output_dir.c_str():
							   m_original_cur_dir.c_str()));
			new_cur_file->arc_path.assign(&buffer[0]);
		}else{
			//絶対パス[\Dir含む]であればそのまま
			new_cur_file->arc_path=CFG.compress.output_file;
		}
		//修正したパスから親ディレクトリを取得
		output_dir=path::getParentDirectory(arc_path);

		if(!path::fileExists(output_dir.c_str())){
			if(!fileoperation::createDirectory(output_dir.c_str())){
				err_msg=format(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
				return ARC_CANNOT_CREATE_DIRECTORY;
			}
		}
	}

	bool need_ext=true;
	tstring ext;

	if(!CFG.compress.raw_file_name){

#define NEED_EXT(need_ext,arc_path)\
	if(m_arc_dll->isSupportedExtension(path::getExtension(arc_path).c_str())&&\
		!path::isDirectory(arc_path.c_str())){\
		if(m_arc_dll->type()==Archiver::CAL){\
			if(str::isEqualStringIgnoreCase(static_cast<ArcDll*>(m_arc_dll)->getFormat().ext,\
											tstring(_T("."))+path::getExtension(arc_path))){\
				need_ext=false;\
			}\
		}else{need_ext=false;}\
	}\

		//new_cur_file->arc_pathに圧縮形式の拡張子があれば追加しない
		NEED_EXT(need_ext,new_cur_file->arc_path);

		if(!CFG.compress.output_file.empty()){
			//'/of～'に圧縮形式の拡張子が含まれていれば追加しない
			NEED_EXT(need_ext,CFG.compress.output_file);
		}

#undef NEED_EXT

	}else{
		need_ext=false;
	}

	if(need_ext){
		if(m_arc_dll->type()==Archiver::CAL){
			ext=static_cast<ArcDll*>(m_arc_dll)->getFormat().ext;
		}
	}

	if(CFG.general.auto_rename){
		//既に存在する様であればリネーム(arc.zip->arc_1.zip)
		if(CFG.compress.raw_file_name){
			tstring e=getExtensionEx(CFG.compress.output_file);

			if(!e.empty()&&
			   new_cur_file->arc_path.rfind(str::toLower(e))!=tstring::npos){
				ext.assign(_T("."));
				ext+=getExtensionEx(CFG.compress.output_file);
				new_cur_file->arc_path=removeExtensionEx(new_cur_file->arc_path);
				need_ext=true;
			}
		}

		if(path::fileExists((new_cur_file->arc_path+((need_ext)?ext:_T(""))).c_str())){
			for(unsigned long long i=1;i<ULLONG_MAX;++i){
				tstring n=format(_T("_%I64u"),i);

				if(!path::fileExists((new_cur_file->arc_path+n.c_str()+((need_ext)?ext:_T(""))).c_str())){
					new_cur_file->arc_path+=n.c_str();
					new_cur_file->auto_renamed=true;
					break;
				}
			}
		}
	}

	if(need_ext&&
	   m_arc_dll->type()==Archiver::CAL){
		//拡張子を追加
		new_cur_file->arc_path+=static_cast<ArcDll*>(m_arc_dll)->getFormat().ext;
	}

	return ARC_SUCCESS;
}

//m_arcdll_list読み込み
void RecesBase::loadArcLib(){
#ifdef _WIN64
	INNER_FUNC(loadUnBypass,
		bool operator()(ArcDll* list){
			if(!list->isLoaded()){
				return list->load(_T("UNBYPASS"),list->prefix().c_str());
			}
			return false;
		}
	);
#endif

	if(m_arcdll_list.size()!=0
	   ||m_b2e_dll!=NULL
	   )freeArcLib();

	m_arcdll_list.push_back(new ArcUnlha32());
#ifdef _WIN64
	loadUnBypass(m_arcdll_list.back());
#endif

	m_arcdll_list.push_back(new ArcUnrar32());
	m_arcdll_list.push_back(new ArcTar32());

	m_arcdll_list.push_back(new Arc7zip32());

#ifndef _WIN64
	m_arcdll_list.push_back(new ArcUniso32());
#endif

#ifndef _WIN64
	m_arcdll_list.push_back(new ArcXacrett());
#endif

	//B2e.dllは特別扱い
	m_b2e_dll=new ArcB2e();
}

//m_arcdll_list解放
void RecesBase::freeArcLib(){
	for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
		SAFE_DELETE(m_arcdll_list[i]);
	}

	std::vector<ArcDll*>().swap(m_arcdll_list);

	if(m_b2e_dll!=NULL){
		SAFE_DELETE(m_b2e_dll);
	}
}

//ファイルのフルパスリストを作成
bool RecesBase::fullPathList(std::list<tstring>& list,std::vector<tstring>& filepaths,bool recursive){
	for(std::vector<tstring>::size_type i=0,size=filepaths.size();!IS_TERMINATED&&i<size;++i){
		std::vector<TCHAR> buffer(MAX_PATHW);

		//フルパスを取得
		if(path::getFullPath(&buffer[0],buffer.size(),filepaths[i].c_str())&&
		   path::fileExists(&buffer[0])){
			tstring buffer_str(&buffer[0]);

			if(recursive){
				//ディレクトリなら再帰的検索してリストに追加
				if(path::isDirectory(&buffer[0])){
					path::recursiveSearch(&list,&buffer[0]);
					continue;
				}
			}

			if(find(list.begin(),list.end(),buffer_str)==list.end()){
				//リストに追加
				list.push_back(buffer_str);
			}
		}
	}
	return !list.empty();
}
