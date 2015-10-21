//Settings.cpp
//設定

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Settings.h"
#include"ArcCAL.h"


using namespace sslib;


Settings::ARC_RESULT Settings::operator()(HWND wnd,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	m_arc_dll=NULL;
	m_cal_dll=NULL;

	if(CFG.general.selected_library_name.empty()){
		err_msg=(_T("ライブラリが指定されていません。\n"));
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	msg::info(_T("ライブラリを読み込んでいます..."));

	m_arc_dll=loadAndCheckPlugin(&m_spi_list,
								 NULL,
								 NULL,
								 CFG.general.spi_dir,
								 CFG.general.selected_library_name.c_str(),
								 Archiver::SPI_AM);

	if(!m_arc_dll){
		m_arc_dll=loadAndCheckPlugin(&m_wcx_list,
									 NULL,
									 NULL,
									 CFG.general.wcx_dir,
									 CFG.general.selected_library_name.c_str(),
									 Archiver::WCX);
	}

	if(!m_arc_dll){
		if(!CFG.general.selected_library_prefix.empty()){
			//対応外の統合アーカイバライブラリ
			m_cal_dll=new ArcCAL(CFG.general.selected_library_name.c_str(),
										CFG.general.selected_library_prefix.c_str());
			if(m_cal_dll){
				if(m_cal_dll->load()){
					m_arc_dll=m_cal_dll;
				}else{
					SAFE_DELETE(m_cal_dll);
				}
			}
		}

		if(!m_arc_dll){
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   NULL,
								   NULL,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}

		if(!m_arc_dll&&m_b2e_dll){
			std::vector<Archiver*> v;

			v.push_back(m_b2e_dll);

			if(!m_b2e_dir.empty()){
				//b2eスクリプトのあるディレクトリを指定
				m_b2e_dll->setScriptDirectory(m_b2e_dir.c_str());
			}

			m_arc_dll=loadAndCheck(v.begin(),
								   v.end(),
								   NULL,
								   NULL,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}
	}

	if(!m_arc_dll){
		STDOUT.outputString(_T("\n"));
		err_msg=format(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	msg::info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			msg::info(_T("バックグラウンドモードに設定しました。\n"));
		}

		//ライブラリの設定ダイアログ呼び出し
		if(m_arc_dll->configurationDialog(wnd)){
			return ARC_SUCCESS;
		}else{
			err_msg=_T("設定に失敗したか、設定ダイアログが用意されていません。\n");
			return ARC_FAILURE;
		}
	}
	return ARC_FAILURE;
}
