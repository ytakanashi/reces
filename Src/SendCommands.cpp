//SendCommands.cpp
//直接操作

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"SendCommands.h"
#include"ArcCAL.h"


using namespace sslib;


SendCommands::ARC_RESULT SendCommands::operator()(std::list<tstring>& commands_list,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	m_arc_dll=NULL;
	m_cal_dll=NULL;

	msg::info(_T("ライブラリを読み込んでいます..."));

	{

		if(!CFG.general.selected_library_prefix.empty()){
			//対応外の統合アーカイバライブラリ
			m_cal_dll=new ArcCAL(CFG.general.selected_library_name.c_str(),
								 CFG.general.selected_library_prefix.c_str());
			if(m_cal_dll){
				if(m_cal_dll->load()){
					m_arc_dll=m_cal_dll;
				}else{
					SAFE_DELETE(m_cal_dll)
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

#ifndef _WIN64
		if(!m_arc_dll&&m_b2e_dll){
			std::vector<Archiver*> v;

			v.push_back(m_b2e_dll);
			m_arc_dll=loadAndCheck(v.begin(),
								   v.end(),
								   NULL,
								   NULL,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
			if(m_arc_dll!=NULL){
				if(!CFG.general.b2e_dir.empty()){
					//b2eスクリプトのあるディレクトリを指定
					m_b2e_dll->setScriptDirectory(CFG.general.b2e_dir.c_str());
				}
			}
		}
#endif

		if(!m_arc_dll){
			STDOUT.outputString(_T("\n"));
			err_msg=format(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
			return ARC_CANNOT_LOAD_LIBRARY;
		}
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

		tstring cmd;

		if(!CFG.general.custom_param.empty()){
			//ユーザ独自のパラメータ
			cmd+=CFG.general.custom_param;
			cmd+=_T(" ");
		}

		for(std::list<tstring>::iterator ite=commands_list.begin(),
			end=commands_list.end();
			ite!=end;
			/*++ite*/){
			if(ite->find(_T(" "))!=tstring::npos){
				cmd+=_T("\"");
				cmd+=*ite;
				cmd+=_T("\"");
			}else{
				cmd+=*ite;
			}
			if(++ite!=end){
				cmd+=_T(" ");
			}
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_CREATE_PROGRESSBAR);
		}
		//コールバック関数を登録
		m_arc_dll->setCallback(m_progressbar_thread.id);

		//ライブラリにコマンドを直接渡す
		int result=-1;
		tstring log;

		if(!CFG.no_display.no_log){
			result=static_cast<ArcDll*>(m_arc_dll)->sendCommands(cmd.c_str(),&log);
		}else{
			result=static_cast<ArcDll*>(m_arc_dll)->sendCommands(cmd.c_str());
		}

		if(!CFG.no_display.no_information){
			STDOUT.outputString(_T("\n   => return code %d[%#x]\n"),result,result);
		}

		if(!log.empty())STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
		}
		//コールバック関数の登録を解除
		m_arc_dll->clearCallback();
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}
