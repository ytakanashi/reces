//ArcLMZip32.cpp
//LMZIP32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcLMZip32.h"
#include"ArcCfg.h"
#include"FileInfo.h"

using namespace sslib;


namespace{
namespace hook{
	//ArcLMZip32のコンストラクタで代入すること
	ArcLMZip32* this_ptr=NULL;
	bool no_information=false;

	typedef BOOL(WINAPI*pShowWindow)(HWND,int);
	typedef HWND(WINAPI*pCreateDialogIndirectParamW)(HINSTANCE,LPCDLGTEMPLATE,HWND,DLGPROC,LPARAM);
	typedef BOOL(WINAPI*pSetDlgItemTextW)(HWND,int,LPCWSTR);
	typedef int(WINAPI*pMessageBoxW)(HWND,LPCWSTR,LPCWSTR,UINT);

	pShowWindow org_pShowWindow;
	pCreateDialogIndirectParamW org_pCreateDialogIndirectParamW;
	pSetDlgItemTextW org_pSetDlgItemTextW;
	pMessageBoxW org_pMessageBoxW;
	HWND extracting_wnd_handle=NULL;

	BOOL WINAPI showWindow(HWND hWnd,int nCmdShow){
		int state=nCmdShow;

		if(hWnd==extracting_wnd_handle)state=SW_HIDE;
		return org_pShowWindow(hWnd,state);
	}

	HWND WINAPI createDialogIndirectParamW(HINSTANCE hInstance,LPCDLGTEMPLATE lpTemplate,HWND hWndParent,DLGPROC lpDialogFunc,LPARAM lParamInit){
		HWND cur_wnd=::GetForegroundWindow();
		HWND result=org_pCreateDialogIndirectParamW(hInstance,lpTemplate,hWndParent,lpDialogFunc,lParamInit);

		if(::GetDlgItem(result,1005)){
			extracting_wnd_handle=result;
			::SetForegroundWindow(cur_wnd);
		}
		return result;
	}

	BOOL WINAPI setDlgItemTextW(HWND hWnd,int nIDDlgItem,LPCWSTR lpString){
		if(hWnd==extracting_wnd_handle&&nIDDlgItem==1013){
			TCHAR buffer[MAX_PATH]={};

			::GetWindowText(::GetDlgItem(extracting_wnd_handle,1005),buffer,MAX_PATH);
			this_ptr->preCallback(buffer,lpString);
		}
		return org_pSetDlgItemTextW(hWnd,nIDDlgItem,lpString);
	}

	int WINAPI messageBoxW(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType){
		if(hWnd==extracting_wnd_handle&&
		   (lstrcmp(lpText,L"解凍を中断しますか？")==0/*&&
			lstrcmpi(lpCaption,L"LMZIP32")==0)*/)){
				return IDYES;
		}
		return org_pMessageBoxW(hWnd,lpText,lpCaption,uType);
	}


	void init(){
		extracting_wnd_handle=NULL;

		//ShowWindow
		org_pShowWindow=(pShowWindow)GetProcAddress(GetModuleHandleA("user32"),"ShowWindow");
		//CreateDialogIndirectParamW
		org_pCreateDialogIndirectParamW=(pCreateDialogIndirectParamW)GetProcAddress(GetModuleHandleA("user32"),"CreateDialogIndirectParamW");
		//SetDlgItemTextW
		if(!no_information)org_pSetDlgItemTextW=(pSetDlgItemTextW)GetProcAddress(GetModuleHandleA("user32"),"SetDlgItemTextW");
		//MessageBoxW
		org_pMessageBoxW=(pMessageBoxW)GetProcAddress(GetModuleHandleA("user32"),"MessageBoxW");
	}

	void install(bool no_information_=false){
		no_information=no_information_;
		init();

		//ShowWindow
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pShowWindow),
						 reinterpret_cast<PROC>(showWindow),
						 this_ptr->module());

		//CreateDialogIndirectParamW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pCreateDialogIndirectParamW),
						 reinterpret_cast<PROC>(createDialogIndirectParamW),
						 this_ptr->module());

		//SetDlgItemTextW
		if(!no_information)
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pSetDlgItemTextW),
						 reinterpret_cast<PROC>(setDlgItemTextW),
						 this_ptr->module());

		//MessageBoxW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pMessageBoxW),
						 reinterpret_cast<PROC>(messageBoxW),
						 this_ptr->module());
	}

	void uninstall(){
		//ShowWindow
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(showWindow),
						 reinterpret_cast<PROC>(org_pShowWindow),
						 this_ptr->module());

		//CreateDialogIndirectParamW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(createDialogIndirectParamW),
						 reinterpret_cast<PROC>(org_pCreateDialogIndirectParamW),
						 this_ptr->module());

		//SetDlgItemTextW
		if(!no_information)
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(setDlgItemTextW),
						 reinterpret_cast<PROC>(org_pSetDlgItemTextW),
						 this_ptr->module());

		//MessageBoxW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(messageBoxW),
						 reinterpret_cast<PROC>(org_pMessageBoxW),
						 this_ptr->module());
	}
}
}


ArcLMZip32::ArcLMZip32():
	ArcDll(_T("LMZip32"),
			_T("Unzip"),
			_T("zip"),
			_T("\\")),
	m_file_size(0){
		hook::this_ptr=this;
		COMPRESSION_METHOD method[]={
#ifdef LMZIP32_CMP
			{_T("lmzip"),_T(".zip"),_T(""),_T(""),
				0,
				0,6,9},

			{_T("lmzippw"),_T(".zip"),_T("-e"),_T("-e"),
				0,
				0,6,9},
#endif
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

//書庫をテスト
ArcLMZip32::ARC_RESULT ArcLMZip32::test(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	//プレフィックス設定
	setPrefix(_T("Unzip"));

	tstring cmd_line(format(_T("%s %s%s%s"),
									  _T("-t"),
									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
									  arc_path_str.c_str(),
									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T("")));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	if(CFG.no_display.no_log){
		//実行
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		tstring log_msg;

		dll_ret=execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcLMZip32::ARC_RESULT ArcLMZip32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
//超絶つくりかけ!!!!
#ifndef LMZIP32_CMP
	return ARC_FAILURE;
#else
	if(getMethod().mhd==NULL)return ARC_FAILURE;

	tstring arc_path_str(arc_path);

	m_file_size=0;

	//プレフィックス設定
	setPrefix(_T("Zip"));

#if 0
	//階層無視圧縮ではリスト出力が必要!
	if(CFG.general.ignore_directory_structures&&
	   (CFG.general.filefilter.empty()&&CFG.general.file_ex_filter.empty())){
		CFG.general.filefilter.pattern_list.push_back(tstring(_T("*")));
	}
#endif

	tstring list_file_path;
	File list_file;

	//リストファイルを作成
	list_file_path=fileoperation::createTempFile(_T("lmz"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	//リストファイルにパスを出力
	if(CFG.mode==MODE_RECOMPRESS){
		list_file.writeEx(_T("*\r\n"));
#if 0
		//ファイルサイズ計算
		if(!CFG.no_display.no_information&&
		   !STDOUT.isRedirected()){
			m_file_size=fileoperation::getDirectorySize(path::getCurrentDirectory().c_str());
		}
#endif
	}

	if(CFG.mode==MODE_COMPRESS){
		for(std::list<tstring>::const_iterator ite=file_list->begin(),
			end=file_list->end();
			ite!=end;++ite){
			writeFormatedList(list_file,*ite);
		}
	}

	if(!list_file.getSize())return ARC_NO_MATCHES_FOUND;

	list_file.close();

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(&CFG.general.filefilter.pattern_list);
	replaceDelimiter(&CFG.general.file_ex_filter.pattern_list);
	replaceDelimiter(file_list);
	replaceDelimiter(list_file_path);

	//勝手に拡張子が付加されないように'.'をファイル名末尾に追加。
	arc_path_str+=_T(".");

	//圧縮率の処理
	tstring level_str;

	if(CFG.compress.compression_level==minimum_compressionlevel){
		//'/l'と指定された場合
		CFG.compress.compression_level=getMethod().minimum_level;
	}else if(CFG.compress.compression_level==maximum_compressionlevel){
		//'/lx'と指定された場合
		CFG.compress.compression_level=getMethod().maximum_level;
	}

	if(CFG.compress.compression_level!=-1){
		//圧縮率を範囲内に収める
		CFG.compress.compression_level=clamp(CFG.compress.compression_level,
											   getMethod().minimum_level,
											   getMethod().maximum_level);
		level_str=getMethod().level;
		level_str+=static_cast<TCHAR>(CFG.compress.compression_level+'0');
	}

	tstring cmd_line(format(_T("%s %s %s %s %s %s%s%s %s@%s%s"),
									  //-r 再帰ディレクトリ検索
									  _T("-r"),
									  //-S システム・隠し属性ファイルも対象にする
									  _T("-S"),
									  //-j パスを保存しない
									  (!CFG.general.ignore_directory_structures)?_T(""):_T("-j"),
									  //getMethod().level,compression_level
									  (CFG.compress.compression_level!=-1&&!level_str.empty())?level_str.c_str():getMethod().cmd,
									  CFG.general.custom_param.c_str(),

									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
									  arc_path_str.c_str(),
									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),

									  (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
									  list_file_path.c_str(),
									  (str::containsWhiteSpace(list_file_path))?_T("\""):_T("")));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'に圧縮しています...\n\n"),arc_path);

	//実行
	int dll_ret=-1;

	if(CFG.no_display.no_log){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.c_str(),log_msg,log_buffer_size);
	}

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
#endif
}

ArcLMZip32::ARC_RESULT ArcLMZip32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	m_file_size=0;

	//プレフィックス設定
	setPrefix(_T("Unzip"));

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	tstring list_file_path;
	File list_file;

	if(use_filter){
		//リストファイルを作成
		list_file_path=fileoperation::createTempFile(_T("lmz"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象ファイルのみ出力
		if(!outputFileListEx(arc_path_str.c_str(),
							 CFG.general.filefilter,
							 CFG.general.file_ex_filter,
							 list_file)){
			return ARC_NO_MATCHES_FOUND;
		}
		list_file.close();
	}

	tstring output_dir_bak;
	fileoperation::scheduleDelete schedule_delete;

	if(CFG.compress.exclude_base_dir!=0){
		output_dir_bak=output_dir_str;
		output_dir_str=path::addTailSlash(fileoperation::createTempDir(_T("rcs"),output_dir_str.c_str()));

		//一時ディレクトリ削除予約
		schedule_delete.set(output_dir_str.c_str());
		//削除漏れ対策
		ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(output_dir_str.c_str()));
	}

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(output_dir_str);

	tstring cmd_line(format(_T("%s %s %s %s%s%s %s%s%s"),
									  //反転注意
									  //-j パス付きでファイルを復元
									  (!CFG.general.ignore_directory_structures)?_T("-j"):_T(""),
									  //-o 既存ファイルへの上書き確認の省略
									  _T("-o"),
									  CFG.general.custom_param.c_str(),

									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
									  arc_path_str.c_str(),
									  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),

									  (str::containsWhiteSpace(output_dir_str))?_T("\""):_T(""),
									  output_dir_str.c_str(),
									  (str::containsWhiteSpace(output_dir_str))?_T("\""):_T("")));

	if(use_filter){
		cmd_line.append(format(_T(" %s@%s%s"),
										 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
										 list_file_path.c_str(),
										 (str::containsWhiteSpace(list_file_path))?_T("\""):_T("")));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を解凍しています...\n\n"),arc_path);


	hook::install(CFG.no_display.no_information||STDOUT.isRedirected());

	//実行
	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
		m_extracting_info_struct_size=sizeof(EXTRACTINGINFOEX64);
	}

	if(CFG.no_display.no_log){
		tstring dummy(1,'\0');

		dll_ret=execute(app()->wnd(),cmd_line.c_str(),&dummy,dummy.length());
	}else{
		dll_ret=execute(app()->wnd(),cmd_line.c_str(),log_msg,log_buffer_size);
	}

	hook::uninstall();

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	if(dll_ret==50/*PK_DISK*/)dll_ret=0;

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir_str,'/','\\');

	if(CFG.general.decode_uesc){
		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		decodeUnicodeEscape(arc_path,output_dir_str.c_str(),CFG.general.ignore_directory_structures);
	}

	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		recoverDirectoryTimestamp(arc_path,output_dir_str.c_str(),CFG.general.decode_uesc,true);
	}

	unload();

	if(CFG.compress.exclude_base_dir!=0){
		//共通パスを取り除く
		excludeCommonPath(output_dir_bak.c_str(),output_dir_str.c_str(),CFG.compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcLMZip32::ARC_RESULT ArcLMZip32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	//プレフィックス設定
	setPrefix(_T("Unzip"));

	if(CFG.output_file_list.api_mode){
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter,(CFG.general.decode_uesc)?DECODE_UNICODE_ESCAPE:0);
	}else{
		tstring cmd_line(format(_T("%s %s%s%s"),
										  _T("-v"),
										  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
										  arc_path_str.c_str(),
										  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T("")));

		dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

		tstring log_msg;

		execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}
	return ARC_SUCCESS;
}

int ArcLMZip32::sendCommands(const TCHAR* commands,tstring* log_msg){
	int dll_ret=-1;

	//実行
	if(log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL/*ARCCFG->wnd()*/,commands,&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL/*ARCCFG->wnd()*/,commands,log_msg,log_buffer_size);
	}

	return dll_ret;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcLMZip32::writeFormatedPath(const File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig){
	tstring file_path(file_path_orig);

	//ディレクトリ名末尾の区切り文字は不要
	file_path=path::removeTailSlash(file_path);

	return list_file.writeEx(_T("%s%s%s\r\n"),
							 (str::containsWhiteSpace(file_path))?_T("\""):_T(""),
							 file_path.c_str(),
							 (str::containsWhiteSpace(file_path))?_T("\""):_T(""));
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcLMZip32::writeFormatedList(const File& list_file,const tstring& full_path){
#ifndef LMZIP32_CMP
	return false;
#else
	//超絶作りかけ!!!



	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	tstring base_dir(path::getParentDirectory(full_path));
	tstring file_name(path::getFileName(full_path));

	if(path::isDirectory(full_path.c_str())&&
	   !path::isEmptyDirectory(full_path.c_str())){
		//full_pathはディレクトリ

		if(CFG.compress.exclude_base_dir!=0){
			//基底ディレクトリを含まない
			base_dir=full_path;
		}

		std::list<size_info> relative_path_list;

		if(use_filter){
			//フィルタ使用時
			std::vector<fileinfo::FILEINFO> exclude_list;

			FileTree file_tree(CFG.general.filefilter,CFG.general.file_ex_filter);
			file_tree.createFileTree(full_path.c_str());
			file_tree.makeExcludeTree(FileTree::TO_NONE,full_path.c_str());

			//リストに変換
			file_tree.tree2list(exclude_list);

			if(exclude_list.empty()){
				//フィルタを使用しない処理へ
				use_filter=false;
			}else{
				for(std::vector<fileinfo::FILEINFO>::iterator ite=exclude_list.begin(),
					end=exclude_list.end();
					ite!=end;
					++ite){
					relative_path_list.push_back(size_info(
						path::makeRelativePath(base_dir.c_str(),
											   FILE_ATTRIBUTE_DIRECTORY,
											   ite->name.c_str(),
											   ::GetFileAttributes(ite->name.c_str())),
						ite->size));
				}
			}
		}

		if(!use_filter){
			relative_path_list.push_back(size_info(
				path::makeRelativePath(base_dir.c_str(),
									   FILE_ATTRIBUTE_DIRECTORY,
									   full_path.c_str(),
									   ::GetFileAttributes(full_path.c_str())),
				fileoperation::getDirectorySize(full_path.c_str())));
		}

		if(!relative_path_list.empty()){
			//一つ以上追加された

			if(CFG.compress.exclude_base_dir!=0&&!use_filter){
				//'eb'が有効かつフィルタなし
				writeFormatedPath(list_file,base_dir.c_str(),_T("*"));
			}

			for(std::list<size_info>::iterator ite=relative_path_list.begin(),
				end=relative_path_list.end();
				ite!=end;
				++ite){
				if(!CFG.general.ignore_directory_structures){
					//'/mc'
					//write:
					//[親ディレクトリ ディレクトリ内ファイルの相対パス]
					writeFormatedPath(list_file,base_dir.c_str(),ite->first.c_str());
				}else{
					//'mC'
					if(str::countCharacter(ite->first.c_str(),*m_delimiter)){
						//相対パスにデリミタを含む場合

						//例:base_dir=foo
						//   ite->name.c_str()=bar/baz

						//相対パスからファイル名を除くパスの部分を取得
						//例:bar
						tstring tmp=path::getParentDirectory(ite->first);
						base_dir+=m_delimiter;
						//元の親ディレクトリのパスと取得したパスを結合
						//foo/bar
						base_dir+=tmp;
					}
					//ファイルの相対パスからファイル名を取得
					file_name=path::getFileName(ite->first);

					//write:
					//[ディレクトリパス] [区切り文字がないファイルパス]
					writeFormatedPath(list_file,base_dir.c_str(),file_name.c_str());

					//親のパスを元に戻す
					base_dir=path::getParentDirectory(full_path);
					replaceDelimiter(base_dir);
				}
				//合計サイズ
				if(!CFG.no_display.no_information&&
				   !STDOUT.isRedirected()){
					m_file_size+=ite->second;
				}
			}
		}
	}else{
		//full_pathはファイル
		if(fileinfo::matchFilters(full_path.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter,base_dir.c_str())){
			//合計サイズ
			if(!CFG.no_display.no_information&&
			   !STDOUT.isRedirected()){
				m_file_size+=fileoperation::getFileSize(full_path.c_str());
			}
			//write:
			//[親ディレクトリ ディレクトリ内ファイルの相対パス]
			writeFormatedPath(list_file,base_dir.c_str(),file_name.c_str());
		}
	}
	return true;
#endif
}

//処理を中止する
void ArcLMZip32::abort(){
	if(!isTerminated()){
		if(hook::extracting_wnd_handle!=NULL){
			//CANCEL押下
			HWND old_wnd=::GetLastActivePopup(hook::extracting_wnd_handle);
			for(HWND msg_wnd=old_wnd;old_wnd==msg_wnd;msg_wnd=::GetLastActivePopup(hook::extracting_wnd_handle)){
				//中断確認メッセージボックスが出るまでボタン押下
				::SendDlgItemMessage(hook::extracting_wnd_handle,IDCANCEL,BM_CLICK,0,0);
				::Sleep(100);
			}
		}
	}
	terminateApp();
}

//ArcDll::callbackProcV()に投げるための準備
bool ArcLMZip32::preCallback(const TCHAR* file_name,const TCHAR* progress){
	if(CFG.no_display.no_information||STDOUT.isRedirected())return false;

	EXTRACTINGINFOEX64 arc_info={};

	//ファイル数
	if(progress){
		//ダイアログ上で"done / total"の形で表示されている
		long long count=0,digit=1;
		int i=lstrlen(progress);

		if(m_processing_info.total!=0){
//			arc_info.llFileSize=m_processing_info.total;
			for(;progress[i]!='/'&&i>=0;--i);
			--i;
		}

		for(;i>=0;--i){
			if(isdigit(progress[i])){
				count+=digit*(progress[i]-'0');
				digit*=10;
			}else if(progress[i]=='/'){
//				arc_info.llFileSize=count;
				m_processing_info.total=count;
				count=0;
				digit=1;
			}
		}
//		arc_info.llWriteSize=count;
		m_processing_info.done=count;
	}

	//処理中ファイル名
	if(file_name)m_processing_info.file_name=file_name;
//	if(file_name)lstrcpynA(arc_info.szSourceFileName,str::utf162utf8(file_name).c_str(),FNAME_MAX32);

	if(!callbackProcV(NULL,0,ARCEXTRACT_INPROCESS,&arc_info)){
		return false;
	}
	return true;
}

//ファイル処理情報を格納
void ArcLMZip32::setExtractingInfo(UINT state,void* arc_info){
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):{
#if 0
			//処理中ファイル名
			m_processing_info.file_name=
				str::utf82utf16(((LPEXTRACTINGINFOEX64)arc_info)->szSourceFileName);

			//処理済みサイズ
			m_processing_info.done=((LPEXTRACTINGINFOEX64)arc_info)->llWriteSize;

			//ファイルサイズ
			m_processing_info.total=((LPEXTRACTINGINFOEX64)arc_info)->llFileSize;
#endif
			break;
		}

		case sizeof(EXTRACTINGINFOEX):
			break;
	}
}
