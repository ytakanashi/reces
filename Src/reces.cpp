//reces.cpp
//recesメイン

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24a by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"reces.h"
#include"ArcCfg.h"
#include"ArcUnlha32.h"
#include"ArcTar32.h"
#include"Arc7-zip32.h"
#include"ArcUnrar32.h"
#include"ArcUniso32.h"
#include"ArcXacrett.h"
#include"ArcLMZip32.h"
#include"ArcCAL.h"
#include"Spi.h"
#include"Wcx.h"
#include"Hook/HookArchiverDialog.h"
#include"resources/resource.h"
#include<algorithm>
#include<shlobj.h>
#include<limits.h>


using namespace sslib;


namespace{
	const UINT WM_HOOKDIALOG=::RegisterWindowMessage(_T("_WM_HOOKDIALOG_"));
	const UINT WM_CREATE_PROGRESSBAR=::RegisterWindowMessage(_T("WM_CREATE_PROGRESSBAR"));
	const UINT WM_UPDATE_PROGRESSBAR_MAIN=::RegisterWindowMessage(_T("WM_UPDATE_PROGRESSBAR_MAIN"));
	const UINT WM_DESTROY_PROGRESSBAR=::RegisterWindowMessage(_T("WM_DESTROY_PROGRESSBAR"));

	int wait=0;
}

namespace{
	DWORD WaitForProcessExit(DWORD process_id,DWORD milliseconds=INFINITE){
		return ::WaitForSingleObject(::OpenProcess(SYNCHRONIZE,false,process_id),INFINITE);
	}

	tstring removeTailCharacter(const tstring& str,TCHAR c){
		if(str.empty())return str;

		tstring result(str);

		if(result.find_last_of(c)==result.length()-1){
			result=result.substr(0,result.length()-1);
		}
		return result;
	}

	//ウインドウは応答なしである
	bool isWindowHung(HWND wnd){
		return (!::SendMessageTimeout(wnd,WM_NULL,0,0,SMTO_ABORTIFHUNG,500,NULL))?true:false;
	}

	//ウインドウをフォアグラウンドに持ってくる
	bool setForegroundWindow(HWND wnd){
		bool result=false;
		bool hung=isWindowHung(wnd);

		int foreground_thread_id=0,target_thread_id=0;

		//最小化されていたら元に戻す
		if(::IsIconic(wnd)){
			return ::ShowWindow(wnd,SW_RESTORE)!=0;
		}

		foreground_thread_id=::GetWindowThreadProcessId(::GetForegroundWindow(),NULL);
		target_thread_id=::GetWindowThreadProcessId(wnd,NULL);

		if(foreground_thread_id==target_thread_id||
		   hung){
			result=::SetForegroundWindow(wnd)!=0;
		}else{
			::AttachThreadInput(target_thread_id,foreground_thread_id,true);
			::SetWindowPos(wnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			result=::SetForegroundWindow(wnd)!=0;
			::AttachThreadInput(target_thread_id,foreground_thread_id,false);
		}
		return result;
	}

	//拡張子を取得(tar系考慮)、含まれなければ""を返す
	tstring getExtensionEx(const tstring& file_path){
		if(file_path.empty())return file_path;

		tstring modified_path(path::removeExtension(file_path));

		if(modified_path==file_path)return _T("");

		//拡張子が'.tar.xz'等の場合、二度removeExtension()する
		if(path::getExtension(modified_path)==_T("tar")){
			modified_path=path::removeExtension(modified_path);
		}
		return file_path.substr(modified_path.length()+1);
	}

	//拡張子を削除(tar系考慮)
	tstring removeExtensionEx(const tstring& file_path){
		if(file_path.empty())return file_path;

		tstring modified_path(path::removeExtension(file_path));

		//拡張子が'.tar.xz'等の場合、二度removeExtension()する
		if(path::getExtension(modified_path)==_T("tar")){
			return path::removeExtension(modified_path);
		}
		return modified_path;
	}
}

void createInstance(){
	new Reces;
}

bool Reces::init(){
	{
#ifndef BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE
	#define BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE 0x1
#endif

		//巷で話題の脆弱性対策
		bool(WINAPI*p_setSearchPathMode)(DWORD)=(bool(WINAPI*)(DWORD))::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")),"SetSearchPathMode");
		if(p_setSearchPathMode!=NULL){
			p_setSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE);
		}

		bool(WINAPI*p_setDllDirectory)(const TCHAR*)=(bool(WINAPI*)(const TCHAR*))::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")),"SetDllDirectoryA");
		if(p_setDllDirectory!=NULL){
			p_setDllDirectory(_T(""));
		}

		//Dllからのメッセージの受け取りを許可する
		bool(WINAPI*p_changeWindowMessageFilter)(UINT,DWORD)=(bool(WINAPI*)(UINT,DWORD))::GetProcAddress(::GetModuleHandle(_T("user32.dll")),"ChangeWindowMessageFilter");
		if(p_changeWindowMessageFilter!=NULL){
			//MSGFLT_ADD
			p_changeWindowMessageFilter(WM_HOOKDIALOG,1);
		}
	}

	//現在のカレントディレクトリを保存
	m_original_cur_dir=path::getCurrentDirectory();

	//一時ディレクトリ作成/削除登録
	//再圧縮用
	ARCCFG->m_recmp_temp_dir=path::addTailSlash(fileoperation::createTempDir(_T("rcs")));
	ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(ARCCFG->m_recmp_temp_dir.c_str()));
	//分割/結合用
	m_split_temp_dir=path::addTailSlash(fileoperation::createTempDir(_T("rcs")));
	ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(m_split_temp_dir.c_str()));
	//リストファイル出力用
	ARCCFG->m_list_temp_dir=path::addTailSlash(fileoperation::createTempDir(_T("rcs")));
	ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(ARCCFG->m_list_temp_dir.c_str()));

	if(ARCCFG->m_recmp_temp_dir.empty()||
	   m_split_temp_dir.empty()||
	   ARCCFG->m_list_temp_dir.empty()){
		errmsg(_T("一時ディレクトリの作成に失敗しました。\n"));
		return false;
	}

	//*.dll読み込み
	loadArcLib();

	//spi,wcxの読み込みはrun()で行う

	m_hook_dll_path=path::addTailSlash(path::getTempDirPath())+_T("rcsDlgHook")+SOFTWARE_VERSION+_T(".tmp");

	m_pInstallHook=NULL;
	m_pUninstallHook=NULL;

	if(((!path::fileExists(m_hook_dll_path.c_str())&&fileoperation::extractFromResource(NULL,IDR_HOOK_LIB,_T("BINARY"),m_hook_dll_path.c_str()))||
	   path::fileExists(m_hook_dll_path.c_str()))&&
		(m_hook_dll_module=::LoadLibrary(m_hook_dll_path.c_str()))!=NULL){
		m_pInstallHook=(INSTALLHOOK_PTR)::GetProcAddress(m_hook_dll_module,"installPasswordDialogHook");
		m_pUninstallHook=(UNINSTALLHOOK_PTR)::GetProcAddress(m_hook_dll_module,"uninstallPasswordDialogHook");
		//削除登録
		ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(m_hook_dll_path.c_str()));
	}else{
		errmsg(_T("フック用ライブラリの読み込みに失敗しました。\n"));
	}

	return true;
}

void Reces::cleanup(){
	misc::Lock lock(m_cleanup_cs);
	static bool done=false;
	if(done)return;

	if(!STDOUT.isRedirected()&&
	   !CFG.no_display.no_information){
		//プログレスバー更新通知を受け取るスレッドを閉じる
		misc::thread::close(m_progressbar_thread);
	}

	//ダイアログのフック通知を受け取るスレッドを閉じる
	misc::thread::close(m_dialog_hook_thread);

	//作成に失敗したファイルは削除
	if(CFG.mode==MODE_RECOMPRESS||CFG.mode==MODE_COMPRESS){
		if(fileoperation::getFileSize(m_cur_file.arc_path.c_str())==0)::DeleteFile(m_cur_file.arc_path.c_str());
	}

	//パスワードダイアログのフックを終了
	if(m_pUninstallHook){
		DWORD result=0;
		::SendMessageTimeout(HWND_BROADCAST,WM_NULL,0,0,SMTO_ABORTIFHUNG,300,&result);

		m_pUninstallHook();
		::FreeLibrary(m_hook_dll_module);
		m_hook_dll_module=NULL;
	}

	freeArcLib();

	for(size_t i=0,list_size=m_spi_list.size();i<list_size;i++){
		SAFE_DELETE(m_spi_list[i]);
	}

	for(size_t i=0,list_size=m_wcx_list.size();i<list_size;i++){
		SAFE_DELETE(m_wcx_list[i]);
	}

	SAFE_DELETE(m_cal_dll);

	//カレントディレクトリを元に戻す
	::SetCurrentDirectory(m_original_cur_dir.c_str());

	//リストに登録されたファイル/ディレクトリを削除
	for(size_t i=0,list_size=ARCCFG->m_schedule_list.size();i<list_size;i++){
		SAFE_DELETE(ARCCFG->m_schedule_list[i]);
	}

	STDOUT.showCursor(true);
	done=true;
}

void Reces::usage(){
	STDOUT.outputString(_T("`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`\n")
						  _T("              reces Ver.%s by x@rgs\n")
						  _T("              under NYSL Version 0.9982\n")
						  _T("\n`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`\n\n"),SOFTWARE_VERSION);

	STDOUT.outputString(_T("usage:\n"));
	STDOUT.outputString(_T("\treces [/<options>...] [/@<listfiles>...] [<files>...]\n\n"));
	STDOUT.outputString(_T("options:\n")
						  _T("\t/m<r|R|c|C|e|E|l|L|s|S|v>\t #動作モード\n")
						  _T("\t/mr[type|@[option]][:library]\t #再圧縮\n")
						  _T("\t/mR[type|@[option]][:library]\t #再圧縮 (ディレクトリ階層を無視)\n")
						  _T("\t\t\t  ('@'を指定すると入力書庫と同じ形式で、\n")
						  _T("\t\t\t   圧縮に対応していない形式であればzipで再圧縮)\n")
						  _T("\t\t\t  options:[pw][he][sfx][pwsfx][hesfx]\n")
						  _T("\t\t\t  [library]で解凍用ライブラリ指定可能\n")
						  _T("\t/mc[type]\t #圧縮\n")
						  _T("\t/mC[type]\t #圧縮 (ディレクトリ階層を無視)\n")
						  _T("\t/me[library]\t #解凍\n")
						  _T("\t/mE[library]\t #解凍 (ディレクトリ階層を無視)\n")
						  _T("\t/ml[library]\t #書庫内容一覧 (by reces)\n")
						  _T("\t/mL[library]\t #書庫内容一覧 (by Library)\n")
						  _T("\t/mt[library]\t #テスト\n")
						  _T("\t/ms[lib][:prefix]#ライブラリ直接操作\n")
						  _T("\t\t\t  prefixの指定で非対応のライブラリも使用可能\n")
						  _T("\t/mS<lib>[:prefix]#設定ダイアログ表示\n")
						  _T("\t/mv \t\t #バージョン表示\n\n"));

	int item_count=0;

	STDOUT.outputString(_T("compression type:\n\t   "));
	for(size_t i=0;i<m_arcdll_list.size();++i){
		for(size_t ii=0;m_arcdll_list[i]->getMethod(ii).mhd!=NULL;++ii){
			STDOUT.outputString(_T(" [%8s]"),m_arcdll_list[i]->getMethod(ii).mhd);
			if((item_count+1)%4==0)STDOUT.outputString(_T("\n\t   "));
			item_count++;
		}
	}
	if(item_count%4!=0)STDOUT.outputString(_T("\n"));

	STDOUT.outputString(_T("\n")
						  _T("\t/n<options...>\t #情報非表示\n")
						  _T("\t/ni \t\t ;進捗状況\n")
						  _T("\t/nl \t\t ;ログ\n")
						  _T("\t/np \t\t ;パスワード\n")
						  _T("\t/ne \t\t ;エラーメッセージ\n")
						  _T("\t/na \t\t ;エラーメッセージ除く全て\n")
						  _T("\n")
						  _T("\t/c[options...]\t #ディレクトリを新規作成 {mr/me}\n")
						  _T("\t/c<1|2>\t\t #不要なディレクトリを新規作成しない\n")
						  _T("\t/c1\t\t ;ファイルが一つしか含まれない\n")
						  _T("\t/c2\t\t ;二重ディレクトリ\n")
						  _T("\t/c<n|s>\t\t #末尾から数字や記号を取り除く\n")
						  _T("\t/cn\t\t ;数字\n")
						  _T("\t/cs\t\t ;記号\n")
						  _T("\t/c<t>\t\t #更新日時を書庫と同じにする\n")
						  _T("\n")
						  _T("\t/d[1|2]\t\t #処理終了後元ファイルを削除 {mr/mc/me}\n")
						  _T("\t/d[1]\t\t ;ごみ箱へ送る\n")
						  _T("\t/d2\t\t ;完全に削除\n")
						  _T("\n")
						  _T("\t/e\t\t #ファイル/ディレクトリを一つずつ圧縮 {mr/mc}\n")
						  _T("\n")
						  _T("\t/eb\t\t #作成する基底ディレクトリを除外 {mr/mc}\n")
						  _T("\t\t\t  (共通パスを一つ除外)\n")
						  _T("\t/eb[level|x]\t #共通パスを除外して解凍 {mr/me}\n")
						  _T("\t/eb[level]\t ;共通パスを指定の数だけ除外して解凍\n")
						  _T("\t/ebx\t\t ;共通パスを全て除外して解凍\n")
						  _T("\n")
						  _T("\t/D<b|s|w><dir>\t #特殊なディレクトリを指定\n")
						  _T("\t/Db<directory>\t ;b2eのあるディレクトリを指定 {ms}\n")
						  _T("\t/Ds<directory>\t ;spiのあるディレクトリを指定 {mr/me/ml/mv}\n")
						  _T("\t/Dw<directory>\t ;wcxのあるディレクトリを指定 {mr/me/ml/mv}\n")
						  _T("\n")
						  _T("\t/N\t\t #書庫を新規作成 {mr/mc}\n")
						  _T("\n")
						  _T("\t/I<pattern...>\t #処理対象フィルタ(文字列) {mr/mc/me/ml}\n")
						  _T("\t\t\t  (ワイルドカード指定可能) ('i'で再帰的検索をしない)\n")
						  _T("\t\t\t  (';'で区切って複数指定可能)\n")
						  _T("\t/i:<...>\t #文字列以外の処理対象フィルタ\n")
						  _T("\t\t\t  (':'で区切って複数指定可能)\n")
						  _T("\t\t\t  ('@'以外は'i'と'I'の区別なし)\n")
						  _T("\t/i:s<value>\t #指定サイズ以上のファイルを処理対象に\n")
						  _T("\t/i:S<value>\t #指定サイズ以下のファイルを処理対象に\n")
						  _T("\t\t\t  (末尾に<b|k|m|g|t>がなければバイト単位として処理)\n")
						  _T("\t/i:d<DateTime>\t #指定日時以降のファイルを処理対象に\n")
						  _T("\t/i:D<DateTime>\t #指定日時以前のファイルを処理対象に\n")
						  _T("\t\t\t  (DateTimeはyyyymmddhhmmssで指定) (yyyy以外は省略可能)\n")
						  _T("\t/i:a<d|h|r|s>... #指定属性を含むファイルを処理対象に\n")
						  _T("\t/i:@<filename>\t #処理対象フィルタリストファイル\n")
						  _T("\n")
						  _T("\t/X<pattern...>\t #処理対象除外フィルタ(文字列) {mr/mc/me/ml}\n")
						  _T("\t\t\t  (ワイルドカード指定可能) ('x'で再帰的検索をしない)\n")
						  _T("\t\t\t  (';'で区切って複数指定可能)\n")
						  _T("\t/x:<...>\t #文字列以外の処理対象外フィルタ\n")
						  _T("\t\t\t  (':'で区切って複数指定可能)\n")
						  _T("\t\t\t  ('@'以外は'x'と'X'の区別なし)\n")
						  _T("\t/x:s<value>\t #指定サイズ以上のファイルを処理対象外に\n")
						  _T("\t/x:S<value>\t #指定サイズ以下のファイルを処理対象外に\n")
						  _T("\t\t\t  (末尾に<b|k|m|g|t>がなければバイト単位として処理)\n")
						  _T("\t/x:d<DateTime>\t #指定日時以降のファイルを処理対象外に\n")
						  _T("\t/x:D<DateTime>\t #指定日時以前のファイルを処理対象外に\n")
						  _T("\t\t\t  (DateTimeはyyyymmddhhmmssで指定) (yyyy以外は省略可能)\n")
						  _T("\t/x:a<d|e|h|r|s>..#指定属性を含むファイルを処理対象外に\n")
						  _T("\t/x:@<filename>\t #処理対象除外フィルタリストファイル\n")
						  _T("\n")
						  _T("\t/l[<0-9|x>]\t #圧縮率 {mr/mc}\n")
						  _T("\t/l<0-9>\t\t ;圧縮率を数字で指定\n")
						  _T("\t/l \t\t ;最低圧縮率\n")
						  _T("\t/lx \t\t ;最大圧縮率\n")
						  _T("\n")
						  _T("\t/s<value>\t #書庫を分割 {mr/mc}\n")
						  _T("\t\t\t  (分割サイズ指定は末尾に<b|k|m|g|t>が必要)\n")
						  _T("\t\t\t  (それ以外は分割数指定として処理)\n")
						  _T("\n")
						  _T("\t/od[directory]\t #出力先ディレクトリ {mr/mc/me}\n")
						  _T("\t/od \t\t ;デスクトップに出力\n")
						  _T("\t/od<directory>\t ;<directory>に出力\n")
						  _T("\t\t\t  (相対パスは処理元があるディレクトリ基準)\t")
						  _T("\n")
						  _T("\t/of<filename>\t #出力ファイル名 {mr/mc}\n")
						  _T("\t/oF<filename>\t #出力ファイル名(拡張子をrecesで付加しない) {mr/mc}\n")
						  _T("\n")
						  _T("\t/oo<b|r>\t #出力オプション\n")
						  _T("\t/oob\t\t ;カレントディレクトリ基準で {mr/mc/me}\n")
						  _T("\t\t\t  '/od','/of'の相対パスを処理\n")
						  _T("\t/oor\t\t ;出力ファイルが重複する場合リネーム {mr/mc}\n")
						  _T("\t\t\t  'name.zip' -> 'name_1.zip'\n")
						  _T("\n")
						  _T("\t/pw<password>\t #パスワード {mr/mc/me/ml}\n")
						  _T("\t/pf<filename>\t #パスワードリストファイル {mr/me}\n")
						  _T("\t/pn<password>\t #新しいパスワードを指定 {mr}\n")
						  _T("\n")
						  _T("\t/P<param>\t #ユーザ独自のパラメータ {mr/mc/me/ms}\n")
						  _T("\n")
						  _T("\t/t\t\t #更新日時を元ファイルと同じにする {mr/mc}\n")
						  _T("\n")
						  _T("\t/DIRTS[-|0|1]\t #ディレクトリのタイムスタンプを復元する {mr/me}\n")
						  _T("\t/DIRTS<-|0>\t ;無効\n")
						  _T("\t/DIRTS[1]\t ;有効 <Default>\n")
						  _T("\n")
						  _T("\t/r[command]\t #コマンド実行 {mr}\n")
						  _T("\t\t\t  (解凍と圧縮の間にコマンドを実行)\n")
						  _T("\n")
						  _T("\t/b\t\t #バックグラウンドで処理\n")
						  _T("\n")
						  _T("\t/C<CodePage>\t #リストファイルの文字コードを指定\n")
						  _T("\t/Cshiftjis\t ;Shift-JIS\n")
						  _T("\t/Cutf8\t\t ;UTF-8\n")
						  _T("\t/Cutf16\t\t ;UTF16-LE\n")
						  _T("\t/Cutf16be\t ;UTF16-BE\n")
						  _T("\n")
						  _T("\t/UESC[-|0|1]\t #Unicodeエスケープシーケンスをデコードする {mr/me/ml}\n")
						  _T("\t/UESC<-|0>\t ;無効 <Default>\n")
						  _T("\t/UESC[1]\t ;有効\n")
						  _T("\n")
						  _T("\t/{<filename>\t #設定をファイルから読み込む\n")
						  _T("\t/}<filename>\t #設定をファイルに書き出す\n")
						  _T("\n")
						  _T("\t/q[-|0|1]\t #処理完了後recesを終了\n")
						  _T("\t/q<-|0>\t\t ;無効 (一時停止)\n")
						  _T("\t/q[1]\t\t ;有効 <Default>\n")
						  _T("\n")
						  _T("\t//\t\t #オプション解析終了\n")
						  );
	STDOUT.outputString(_T("\n"));
}

void Reces::ctrlCEvent(){
	misc::Lock lock(m_ctrlc_event_cs);
	static bool done=false;
	if(done)return;

	dprintf(_T("terminate()\n"));
	terminateApp();
	STDOUT.showCursor(true);

	if(m_arc_dll){
		m_arc_dll->abort();
	}else{
		for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
			if(m_arcdll_list[i]!=NULL){
				m_arcdll_list[i]->abort();
			}
		}
		for(size_t i=0,list_size=m_spi_list.size();i<list_size;i++){
			if(m_spi_list[i]!=NULL){
				m_spi_list[i]->abort();
			}
		}
		for(size_t i=0,list_size=m_wcx_list.size();i<list_size;i++){
			if(m_wcx_list[i]!=NULL){
				m_wcx_list[i]->abort();
			}
		}
	}

	::WaitForSingleObject(m_arc_thread,INFINITE);
	::CloseHandle(m_arc_thread);

	if(!STDOUT.isRedirected()&&
	   !CFG.no_display.no_information){
		misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
	}

	//コールバック関数の登録を解除
	m_arc_dll->clearCallback();

	STDOUT.outputString(_T("\n処理が中断されました。\n"));
	cleanup();
	done=true;
}

bool Reces::info(const TCHAR* msg,...){
	if(CFG.no_display.no_information)return false;

	DWORD written_chars=0;

	va_list argp;
	va_start(argp,msg);

	STDOUT.write(msg,argp,&written_chars);

	va_end(argp);
	return true;
}

void Reces::errmsg(const TCHAR* msg,...){
	if(!strvalid(msg)||
	   CFG.no_display.no_errmsg)return;

	Console std_err_handle(STD_ERROR_HANDLE);
	DWORD written_chars=0;

	//文字色の変更
	std_err_handle.setFGColor(Console::HIGH_RED);

	va_list argp;
	va_start(argp,msg);

	std_err_handle.outputString(_T("error: "));
	std_err_handle.write(msg,argp,&written_chars);

	va_end(argp);

	//文字色を元に戻す
	std_err_handle.resetColors();
	return;
}

tstring Reces::getVersion(const TCHAR* file_path){
	DWORD major_ver=0;
	DWORD minor_ver=0;

	if(fileoperation::getFileVersion(file_path,&major_ver,&minor_ver)){
		VariableArgument version(_T("%d.%02d.%02d.%02d"),
									   major_ver>>16,
									   major_ver&0xffff,
									   minor_ver>>16,
									   minor_ver&0xffff);
		return version.get();
	}
	return _T("");
}

//パスワードの入力を求める
bool Reces::requirePassword(){
	Console input_password(STD_INPUT_HANDLE);
	DWORD mode=0;

	input_password.getConsoleMode(&mode);

	if(CFG.no_display.no_password){
		if(!STDOUT.isRedirected()){
			STDOUT.outputString(_T("\nパスワード(表示されません): "));
		}else{
			//標準エラー出力でメッセージを表示
			Console password_message(STD_ERROR_HANDLE);
			password_message.outputString(_T("\nパスワード(表示されません): "));
		}
		//入力した文字を表示しないようにする
		input_password.setConsoleMode(mode&~ENABLE_ECHO_INPUT);
	}else{
		if(!STDOUT.isRedirected()){
			STDOUT.outputString(_T("\nパスワード: "));
		}else{
			//標準エラー出力でメッセージを表示
			Console password_message(STD_ERROR_HANDLE);
			password_message.outputString(_T("\nパスワード: "));
		}
	}

	//カーソル再表示
	STDOUT.showCursor(true);

	std::vector<TCHAR> buffer(1024,'\0');

	_fgetts(&buffer[0],buffer.size(),stdin);
	CFG.general.password.assign(&buffer[0]);
	if(CFG.general.password.c_str()[0]=='\n'){
		CFG.general.password.clear();
	}else{
		str::replaceCharacter(CFG.general.password,_T('\n'),_T('\0'));
	}

	//カーソル再非表示
	STDOUT.showCursor(false);

	//元に戻す
	input_password.setConsoleMode(mode);

	if(CFG.no_display.no_password)STDOUT.outputString(_T("\n\n"));

	return !CFG.general.password.empty();
}

//入力されたコマンドを実行する
bool Reces::runCommand(){
	Console* input_command=new Console(STD_INPUT_HANDLE);
	bool result=false;


	tstring current_dir(path::getCurrentDirectory());

	::SetCurrentDirectory(ARCCFG->m_recmp_temp_dir.c_str());
	if(CFG.recompress.run_command.interactive){
		STDOUT.outputString(_T("コマンド: \n"));
		TCHAR* line=NULL;
		std::vector<TCHAR> buffer(1024,'\0');

		do{
			STDOUT.outputString(_T(">"));
			buffer.assign(buffer.size(),'\0');
			line=_fgetts(&buffer[0],buffer.size(),stdin);
			CFG.recompress.run_command.command.assign(&buffer[0]);

			if(CFG.recompress.run_command.command.c_str()[0]!='\n'){
				str::replaceCharacter(CFG.recompress.run_command.command,_T('\n'),_T('\0'));
				if(strvalid(CFG.recompress.run_command.command.c_str())){
					result=_tsystem(CFG.recompress.run_command.command.c_str())!=-1;
				}
			}else{
				//Enterで抜ける
				break;
			}
		}while(!isTerminated()&&
			   line!=NULL);
	}else{
		if(!CFG.recompress.run_command.command.empty()){
			STDOUT.outputString(_T("'%s'を実行しています...\n"),CFG.recompress.run_command.command.c_str());
			result=_tsystem(CFG.recompress.run_command.command.c_str())!=-1;
		}
	}
	::SetCurrentDirectory(current_dir.c_str());

	SAFE_DELETE(input_command);

	return result;
}

//ファイルを削除(設定依存)
bool Reces::removeFile(const TCHAR* file_path){
	if(CFG.general.remove_source!=RMSRC_DISABLE){
		info(_T("'%s' を%sいます...\n"),
			 file_path,
			 (CFG.general.remove_source==RMSRC_RECYCLEBIN)?_T("ごみ箱に送って"):_T("削除して"));
		if(CFG.general.remove_source==RMSRC_RECYCLEBIN){
			return fileoperation::moveToRecycleBinSH(file_path);
		}else{
			if(path::isDirectory(file_path)){
				fileoperation::deleteDirectory(file_path);
			}else{
				::DeleteFile(file_path);
			}
			return true;
		}
	}
	return false;
}

//'od'と'of'を反映した作成する書庫のパスを作成
Reces::ARC_RESULT Reces::arcFileName(CUR_FILE* new_cur_file,const tstring& arc_path,tstring& err_msg){
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
				VariableArgument error_message(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
				err_msg=error_message.get();
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
				VariableArgument error_message(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
				err_msg=error_message.get();
				return ARC_CANNOT_CREATE_DIRECTORY;
			}
		}
	}

	bool need_ext=true;
	tstring ext;

	if(!CFG.compress.raw_file_name){
		//new_cur_file->arc_pathに圧縮形式の拡張子があれば追加しない
		if(m_arc_dll->isSupportedExtension(path::getExtension(new_cur_file->arc_path).c_str())){
			need_ext=false;
		}

		if(!CFG.compress.output_file.empty()){
			//'/of～'に圧縮形式の拡張子が含まれていれば追加しない
			if(m_arc_dll->isSupportedExtension((str::toLower(path::getExtension(CFG.compress.output_file))).c_str())){
				need_ext=false;
			}
		}
	}else{
		need_ext=false;
	}

	if(need_ext){
		if(m_arc_dll->type()==Archiver::CAL){
			ext=static_cast<ArcDll*>(m_arc_dll)->getMethod().ext;
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
				VariableArgument n(_T("_%I64u"),i);

				if(!path::fileExists((new_cur_file->arc_path+n.get()+((need_ext)?ext:_T(""))).c_str())){
					new_cur_file->arc_path+=n.get();
					new_cur_file->auto_renamed=true;
					break;
				}
			}
		}
	}

	if(need_ext&&
	   m_arc_dll->type()==Archiver::CAL){
		//拡張子を追加
		new_cur_file->arc_path+=static_cast<ArcDll*>(m_arc_dll)->getMethod().ext;
	}

	return ARC_SUCCESS;
}

//m_arcdll_list読み込み
void Reces::loadArcLib(){
	if(m_arcdll_list.size()!=0||
	   m_b2e_dll!=NULL)freeArcLib();

	m_arcdll_list.push_back(new ArcLMZip32());
	m_arcdll_list.push_back(new ArcUnlha32());
	m_arcdll_list.push_back(new ArcUnrar32());
	m_arcdll_list.push_back(new ArcTar32());
	m_arcdll_list.push_back(new ArcUniso32());
	m_arcdll_list.push_back(new Arc7zip32());
	m_arcdll_list.push_back(new ArcXacrett());

	//B2e.dllは特別扱い
	m_b2e_dll=new ArcB2e();
}

//m_arcdll_list解放
void Reces::freeArcLib(){
	for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
		SAFE_DELETE(m_arcdll_list[i]);
	}

	std::vector<ArcDll*>().swap(m_arcdll_list);

	if(m_b2e_dll!=NULL){
		SAFE_DELETE(m_b2e_dll);
	}
}

//ライブラリリストの7-zip32とLMZIP32を入れ替える
bool Reces::swap7ZLMZIP(bool sort_by_name){
	if(!sort_by_name)return true;
	for(size_t i=0,ii=0,list_size=m_arcdll_list.size();i<list_size;){
		if(m_arcdll_list[i]!=NULL&&m_arcdll_list[ii]!=NULL){
			if(m_arcdll_list[i]->name()!=_T("7-zip32"))++i;
			if(m_arcdll_list[ii]->name()!=_T("LMZip32"))++ii;

			if(m_arcdll_list[i]->name()==_T("7-zip32")&&
			   m_arcdll_list[ii]->name()==_T("LMZip32")){
				if((i>ii&&sort_by_name)||
				   (i<ii&&!sort_by_name)){
					std::swap(m_arcdll_list[i],m_arcdll_list[ii]);
				}
				break;
			}
		}
	}
	return true;
}

//読み込みと対応チェック
template<typename I>Archiver* Reces::loadAndCheck(I ite,I end,const TCHAR* arc_path,bool* loaded_library,const TCHAR* ext,const TCHAR* libname,const TCHAR* full_libname){
	for(;ite!=end;++ite){
		if(!(ext&&!(*ite)->isSupportedExtension(ext))&&
		   !(!ext&&libname!=NULL&&!str::isEqualStringIgnoreCase((*ite)->name(),libname))){
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
template<typename T>Archiver* Reces::LoadAndCheckPlugin(std::vector<T*>* plugin_list,const TCHAR* arc_path,bool* loaded_library,const tstring& plugin_dir,const TCHAR* libname,Archiver::ARC_TYPE type){
	//フルパスを取得
	std::vector<TCHAR> full_path(MAX_PATH);

	if(path::getFullPath(&full_path[0],full_path.size(),libname)&&
	   path::fileExists(&full_path[0])){
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
			path::getFullPath(&full_path[0],full_path.size(),libname,plugin_dir.c_str())){
		}
	}
	return loadAndCheck(plugin_list->begin(),
						plugin_list->end(),
						arc_path,
						loaded_library,
						NULL,
						&full_path[0]);
}

//書庫にタイムスタンプをコピー
bool Reces::copyArcTimestamp(const tstring& dest_file_path,FILETIME* source_arc_timestamp){
	std::list<tstring> file_list;
	bool result=false;

	//分割ファイルであればリスト化
	if(!fileoperation::makeSplitFileList(&file_list,dest_file_path.c_str())){
		file_list.push_back(dest_file_path);
	}

	for(std::list<tstring>::iterator ite=file_list.begin(),
		end=file_list.end();
		ite!=end;
		++ite){
		File new_arc(ite->c_str());
		result=new_arc.setFileTime(source_arc_timestamp);
	}
	return result;
}

//ファイルのフルパスリストを作成
bool Reces::fullPathList(std::list<tstring>& list,std::vector<tstring>& filepaths,bool recursive){
	for(std::vector<tstring>::size_type i=0,size=filepaths.size();!isTerminated()&&i<size;++i){
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

//*.spiを検索、リストに追加
bool Reces::searchSpi(const TCHAR* search_dir){
	FileSearch fs;

	std::vector<TCHAR> full_path(MAX_PATH);

	if(path::getFullPath(&full_path[0],full_path.size(),search_dir)&&
	   path::fileExists(&full_path[0])){
		for(fs.first(&full_path[0],_T("*.spi"));fs.next();){
			if(!fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
				dprintf(_T("spi: %s\n"),fs.filepath().c_str());
				Spi* spi=new Spi(fs.filepath().c_str());

				if(spi->type()==Archiver::SPI_AM){
					m_spi_list.push_back(spi);
				}else{
					SAFE_DELETE(spi);
				}
			}
		}
	}
	return m_spi_list.size()!=0;
}

//*.wcxを検索、リストに追加
bool Reces::searchWcx(const TCHAR* search_dir){
	FileSearch fs;

	std::vector<TCHAR> full_path(MAX_PATH);

	if(path::getFullPath(&full_path[0],full_path.size(),search_dir)&&
	   path::fileExists(&full_path[0])){
		for(fs.first(&full_path[0],_T("*.wcx"));fs.next();){
			if(!fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
				dprintf(_T("wcx: %s\n"),fs.filepath().c_str());
				Wcx* wcx=new Wcx(fs.filepath().c_str());

				if(wcx->type()==Archiver::WCX){
					m_wcx_list.push_back(wcx);
				}else{
					SAFE_DELETE(wcx);
				}
			}
		}
	}
	return m_wcx_list.size()!=0;
}

bool Reces::parseOptions(CommandArgument& cmd_arg){
	struct l{
		static bool undupList(std::list<tstring>* list){
			if(!list)return false;

			//重複を削除
			list->sort();
			list->unique();
			//末尾の区切り文字を削除
			for(std::list<tstring>::iterator ite=list->begin(),
				end=list->end();
				ite!=end;
				++ite){
				*ite=path::removeTailSlash(*ite);
			}
			return true;
		}
		static bool readFileList(std::list<tstring>* list,File* list_file){
			if(!list||!list_file)return false;
			std::vector<tstring> file_list;

			list_file->readList(&file_list,File::RL_SKIP_BOM|File::RL_REMOVE_QUOTES);
			//取得したパスたちを処理対象リストに追加
			list->insert(list->end(),file_list.begin(),file_list.end());
			undupList(list);
			return true;
		}
	};

	std::vector<tstring>& options=cmd_arg.options();
	Config cfg_file;
	bool save_cfg=false;
	bool end_switch_scan=false;

	//stdinで受け取った内容をファイルパスとして追加
	cmd_arg.filepaths().insert(cmd_arg.filepaths().end(),cmd_arg.stdinput().begin(),cmd_arg.stdinput().end());

	//設定ファイルに関する処理を初めに行う
	for(std::vector<tstring>::size_type i=0,size=options.size();i<size;++i){
		std::vector<TCHAR> cfg_path(MAX_PATH);

		switch(options[i].c_str()[0]){
			case '{':{
				//cfgファイルを読み込む
				if(path::getFullPath(&cfg_path[0],cfg_path.size(),options[i].substr(1).c_str())&&
				   path::fileExists(&cfg_path[0])){
					cfg_file.setFileName(&cfg_path[0]);
					if(cfg_file.load()){
						break;
					}
				}
				errmsg(_T("cfgファイルの読み込みに失敗しました。\n"));
				break;
			}

			case '}':{
				//cfgファイルに書き出す[オプション解析後]
				if(path::getFullPath(&cfg_path[0],cfg_path.size(),options[i].substr(1).c_str())){
					cfg_file.setFileName(&cfg_path[0]);
					save_cfg=true;
					dprintf(_T("save_cfg=true\n"));
				}
				break;
			}
		}
	}


	//パスワードリストのパスを保存
	//オプション解析後読み込む
	tstring password_list_path=CFG.extract.password_list_path;

	for(std::vector<tstring>::size_type i=0,size=options.size();!end_switch_scan&&i<size;++i){
		switch(options[i].c_str()[0]){
			case 'm':{//動作モード
				switch(options[i].c_str()[1]){
					case 'R':
						//ディレクトリ階層を無視して圧縮/解凍
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'r':{//Recompress
						CFG.mode=MODE_RECOMPRESS;
						dprintf(_T("mode : recompress\n"));

						if(options[i].c_str()[2]!='\0'){
							//圧縮形式
							CFG.compress.compression_type=options[i].substr(2);

							tstring::size_type pos=CFG.compress.compression_type.find(_T(":"));
							if(pos!=tstring::npos){
								//ライブラリ名が指定されている
								CFG.general.selected_library_name=CFG.compress.compression_type.substr(pos+1);
								dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());
								CFG.compress.compression_type=CFG.compress.compression_type.substr(0,pos);
							}
							dprintf(_T("compress mode: %s\n"),CFG.compress.compression_type.c_str());
						}
						break;
					}

					case 'C':
						//ディレクトリ階層を無視して圧縮/解凍
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'c':{//Compress
						CFG.mode=MODE_COMPRESS;
						dprintf(_T("mode : compress\n"));

						if(options[i].c_str()[2]=='\0'){
							//圧縮形式が指定されていな場合
						}else{
							//圧縮形式が指定された場合
							CFG.compress.compression_type=options[i].substr(2);

							dprintf(_T("compress mode: %s\n"),CFG.compress.compression_type.c_str());
						}
						break;
					}


					case 'E':
						//ディレクトリ階層を無視して圧縮/解凍
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'e':{//Extract
						CFG.mode=MODE_EXTRACT;
						dprintf(_T("Mode : extract\n"));

						if(options[i].c_str()[2]!='\0'){
							//ライブラリ名を直接指定した場合
							CFG.general.selected_library_name=options[i].substr(2);

							dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());
						}
						break;
					}

					case 'L':
						//FindFirst()/FindNext()/GetFileName()で出力しない
						CFG.output_file_list.api_mode=false;
						//fall through
					case 'l':
						//一覧出力
						CFG.mode=MODE_LIST;
						dprintf(_T("mode : output_file_list\n"));

						if(options[i].c_str()[2]!='\0'){
							//ライブラリ名を直接指定した場合
							CFG.general.selected_library_name=options[i].substr(2);

							dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());
						}
						break;

					case 't':{
						//テスト
						CFG.mode=MODE_TEST;
						dprintf(_T("mode : test\n"));

						if(options[i].c_str()[2]!='\0'){
							//ライブラリ名を直接指定した場合
							CFG.general.selected_library_name=options[i].substr(2);

							dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());
						}
						break;
					}

					case 's':
					case 'S':{
						if(options[i].c_str()[1]=='s'){
							//ライブラリ直接操作
							CFG.mode=MODE_SENDCOMMANDS;
							dprintf(_T("mode : send_commands\n"));
						}else{
							//ライブラリの設定
							CFG.mode=MODE_SETTINGS;
							dprintf(_T("mode : settings\n"));
						}

						if(options[i].c_str()[2]!='\0'){
							//ライブラリ名を直接指定した場合
							CFG.general.selected_library_name=options[i].substr(2);

							tstring::size_type pos=CFG.general.selected_library_name.rfind(_T(":"));
							if(pos!=tstring::npos){
								CFG.general.selected_library_prefix=CFG.general.selected_library_name.substr(pos+1);

								if(CFG.general.selected_library_prefix.find(_T("/"))!=tstring::npos||
								   CFG.general.selected_library_prefix.find(_T("\\"))!=tstring::npos){
									CFG.general.selected_library_prefix.clear();
								}else{
									//プレフィックス名が指定されている
									dprintf(_T("prefix: %s\n"),CFG.general.selected_library_prefix.c_str());
									CFG.general.selected_library_name=CFG.general.selected_library_name.substr(0,pos);
								}
							}
						}

						//spiでもwcxでもなければ拡張子を取り除く
						if(path::getExtension(CFG.general.selected_library_name)!=_T("spi")&&
						   path::getExtension(CFG.general.selected_library_name)!=_T("wcx")){
							CFG.general.selected_library_name=path::removeExtension(CFG.general.selected_library_name);
						}

						dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());

						if(options[i].c_str()[1]=='s'){
							//'ms'以降はライブラリに送る文字列とする
							//再割り当て
							cmd_arg.filepaths().clear();
							for(std::vector<tstring>::size_type ii=i+1,size=cmd_arg.args().size();ii<size;++ii){
								cmd_arg.filepaths().push_back(cmd_arg.args()[ii]);
							}
							//解析終了
							end_switch_scan=true;
						}
						break;
					}


					case 'v':{
						//バージョン確認
						CFG.mode=MODE_VERSION;
						dprintf(_T("mode : version\n"));
						break;
					}

				}
				break;
			}//case 'm'

			case 'n':{//非表示
				const TCHAR*p=options[i].c_str()+1;

				while(*p){
					switch(*p){
						case 'i':
							//進捗状況
							CFG.no_display.no_information=true;
							dprintf(_T("CFG.no_display.no_information=true\n"));
							break;

						case 'l':
							//ログ
							CFG.no_display.no_log=true;
							dprintf(_T("CFG.no_display.no_log=true\n"));
							break;

						case 'p':
							//パスワード
							CFG.no_display.no_password=true;
							dprintf(_T("CFG.no_display.no_password=true\n"));
							break;

						case 'e':
							//エラーメッセージ
							CFG.no_display.no_errmsg=true;
							dprintf(_T("CFG.no_display.no_errmsg=true\n"));
							break;

						case 'a':
							//注意:エラーメッセージ非表示は別指定
							CFG.no_display.no_information=true;
							dprintf(_T("CFG.no_display.no_information=true\n"));

							CFG.no_display.no_log=true;
							dprintf(_T("CFG.no_display.no_log=true\n"));

							CFG.no_display.no_password=true;
							dprintf(_T("CFG.no_display.no_password=true\n"));
							break;

						default:
							break;
					}
					++p;
				}
				break;
			}//case 'n'

			case 'c':{
				//ディレクトリを作成する
				CFG.extract.create_dir=true;
				dprintf(_T("CFG.extract.create_dir=true\n"));

				const TCHAR*p=options[i].c_str()+1;
				while(*p){
					switch(*p){
						//不要なディレクトリを作成しない
						case '1':
							//ファイル単体の場合作成しない
							CFG.extract.create_dir_optimization.remove_redundant_dir.only_file=true;
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.bOnlyFile=true\n"));
							break;
						case '2':
							//二重ディレクトリを作成しない
							CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=true;
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.bDoubleDir=true\n"));
							break;

						//ディレクトリ名末尾の数字や記号を削除
						case 'n':
							//ディレクトリ名末尾の数字を削除
							CFG.extract.create_dir_optimization.omit_number_and_symbol.number=true;
							dprintf(_T("CFG.extract.create_dir_optimization.omit_number_and_symbol.bNumber=true\n"));
							break;

						case 's':
							//ディレクトリ名末尾の記号を削除
							CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol=true;
							dprintf(_T("CFG.extract.create_dir_optimization.omit_number_and_symbol.bSymbol=true\n"));
							break;

						case 't':
							//ディレクトリの更新日時を書庫と同じにする
							CFG.extract.create_dir_optimization.copy_timestamp=true;
							dprintf(_T("CFG.extract.create_dir_optimization.copy_timestamp=true\n"));
							break;

						default:
							break;
					}
					++p;
				}
				break;
			}//case 'c'

			case 'd':{
				//ソースを削除
				switch(options[i].c_str()[1]){
					case '2':
						CFG.general.remove_source=RMSRC_REMOVE;
						dprintf(_T("CFG.general.remove_source=RMSRC_REMOVE\n"));
						break;

					case '1':
					case '\0':
					default:
						CFG.general.remove_source=RMSRC_RECYCLEBIN;
						dprintf(_T("CFG.general.remove_source=RMSRC_RECYCLEBIN\n"));
						break;
				}
				break;
			}

			case 'e':{
				if(options[i].find(_T("eb"))==0){
					switch(options[i].c_str()[2]){
						case 'x':
							//[解凍]共通パスをすべて除く
							CFG.compress.exclude_base_dir=-1;
							break;

						case '\0':
							CFG.compress.exclude_base_dir=1;
							break;

						default:
							//圧縮時は正負のみ確認
							CFG.compress.exclude_base_dir=_ttoi(options[i].substr(2).c_str());
							break;
					}
					if(CFG.compress.exclude_base_dir<-1)CFG.compress.exclude_base_dir=-1;
					dprintf(_T("CFG.compress.exclude_base_dir=%d\n"),CFG.compress.exclude_base_dir);
				}else{
					//個別圧縮
					CFG.compress.each_file=true;
					dprintf(_T("CFG.compress.each_file=%d\n"),CFG.compress.each_file);
					break;
				}
				break;
			}

			case 'D':{
				//特殊なディレクトリを指定
				switch(options[i].c_str()[1]){
					case 'b':
						//b2eスクリプトのあるディレクトリを指定/
						CFG.general.b2e_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.b2e_dir=%s\n"),CFG.general.b2e_dir.c_str());
						break;

					case 's':
						//spiのあるディレクトリを指定
						CFG.general.spi_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.spi_dir=%s\n"),CFG.general.spi_dir.c_str());
						break;

					case 'w':
						//wcxのあるディレクトリを指定
						CFG.general.wcx_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.wcx_dir=%s\n"),CFG.general.wcx_dir.c_str());
						break;

					case 'I':
						//ディレクトリのタイムスタンプを復元する
						if(options[i].find(_T("DIRTS"))==0){
							switch(options[i].c_str()[5]){
								case '0':
								case '-':
									CFG.extract.directory_timestamp=false;
									break;

								default:
									CFG.extract.directory_timestamp=true;
							}
						}
						dprintf(_T("CFG.extract.directory_timestamp=%d\n"),CFG.extract.directory_timestamp);
						break;
				}
				break;
			}

			case 'N':{//書庫新規作成
				CFG.compress.create_new=true;
				dprintf(_T("CFG.compress.create_new=%d\n"),CFG.compress.create_new);
				break;
			}

			case 'I':
				CFG.general.filefilter.recursive=true;
				dprintf(_T("CFG.general.filefilter.recursive\n"));
				//fall through
			case 'i':{
				//処理対象フィルタ
				if(options[i].length()<2)break;

				if(options[i].c_str()[1]!=':'){
					//';'で分割
					str::splitString(&CFG.general.filefilter.pattern_list,options[i].substr(1).c_str(),';');
					//重複を削除
					l::undupList(&CFG.general.filefilter.pattern_list);
				}else{
					const TCHAR*p=options[i].c_str()+2,*pp;

					while(*p){
						for(pp=p+1;*pp&&*pp!=';';pp++);

						switch(*(p++)){
							case 's':{
								tstring str(p,pp-p);

								CFG.general.filefilter.min_size=str::filesize2longlong(str.c_str());
								dprintf(_T("min_size=%I64d\n"),CFG.general.filefilter.min_size);
								break;
							}

							case 'S':{
								tstring str(p,pp-p);

								CFG.general.filefilter.max_size=str::filesize2longlong(str.c_str());
								dprintf(_T("max_size=%I64d\n"),CFG.general.filefilter.max_size);
								break;
							}

							case 'a':
							case 'A':
								while(p!=pp){
									if(*p=='e'||*p=='E'){
										//「空ディレクトリのみ格納」は実装しない方向で
									}else{
										CFG.general.filefilter.attr|=str::attr2DWORD(*p);
									}
									++p;
								}
								break;

							case 'd':{
								tstring str(p,pp-p);

								CFG.general.filefilter.oldest_date=str::datetime2longlong(str.c_str());
								break;
							}

							case 'D':{
								tstring str(p,pp-p);

								CFG.general.filefilter.newest_date=str::datetime2longlong(str.c_str(),true);
								break;
							}

							case '@':{
								tstring file(p,pp-p);
								File list_file;

								if(!list_file.open(file.c_str(),OPEN_EXISTING,GENERIC_READ,0,CFG.general.codepage)){
									errmsg(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
								}else{
									l::readFileList(&CFG.general.filefilter.pattern_list,&list_file);
								}
								break;
							}

							default:
								break;
						}

						if(*pp=='\0')break;
						p=pp+1;
					}
				}
				break;
			}

			case 'X':
				CFG.general.file_ex_filter.recursive=true;
				dprintf(_T("CFG.general.file_ex_filter.recursive\n"));
				//fall through
			case 'x':{
				//処理対象除外フィルタ
				if(options[i].length()<2)break;

				if(options[i].c_str()[1]!=':'){
					//';'で分割
					str::splitString(&CFG.general.file_ex_filter.pattern_list,options[i].substr(1).c_str(),';');
					//重複を削除
					l::undupList(&CFG.general.file_ex_filter.pattern_list);
				}else{
					const TCHAR*p=options[i].c_str()+2,*pp;

					while(*p){
						for(pp=p+1;*pp&&*pp!=';';pp++);

						switch(*(p++)){
							case 's':{
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.min_size=str::filesize2longlong(str.c_str());
								dprintf(_T("min_size=%I64d\n"),CFG.general.file_ex_filter.min_size);
								break;
							}

							case 'S':{
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.max_size=str::filesize2longlong(str.c_str());
								dprintf(_T("max_size=%I64d\n"),CFG.general.file_ex_filter.max_size);
								break;
							}

							case 'a':
							case 'A':
								while(p!=pp){
									if(*p=='e'||*p=='E'){
										//include_empty_dirは除外フィルタの場合のみ操作
										CFG.general.file_ex_filter.include_empty_dir=false;
										dprintf(_T("CFG.general.file_ex_filter.include_empty_dir=false\n"));
									}else{
										CFG.general.file_ex_filter.attr|=str::attr2DWORD(*p);
									}
									++p;
								}
								break;

							case 'd':{
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.oldest_date=str::datetime2longlong(str.c_str());
								break;
							}

							case 'D':{
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.newest_date=str::datetime2longlong(str.c_str(),true);
								break;
							}

							case '@':{
								tstring file(p,pp-p);
								File list_file;

								if(!list_file.open(file.c_str(),OPEN_EXISTING,GENERIC_READ,0,CFG.general.codepage)){
									errmsg(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
								}else{
									l::readFileList(&CFG.general.file_ex_filter.pattern_list,&list_file);
								}
								break;
							}

							default:
								break;
						}
						if(*pp=='\0')break;
						p=pp+1;
					}
				}
				break;
			}

			case 'l':{
				//圧縮率
				if(options[i].c_str()[1]=='\0'){
					//'/l'と指定された場合、圧縮率を最小にする
					CFG.compress.compression_level=Archiver::minimum_compressionlevel;
				}else if(options[i].c_str()[1]=='x'){
					//'/lx'と指定された場合、圧縮率を最大にする
					CFG.compress.compression_level=Archiver::maximum_compressionlevel;
				}else{
					CFG.compress.compression_level=options[i].c_str()[1]-'0';
					//値が範囲内かどうかはcompress()にて判断
				}
				dprintf(_T("CFG.compress.compression_level=%d\n"),CFG.compress.compression_level);
				break;
			}

			case 's':{
				//分割圧縮
				CFG.compress.split_value=options[i].substr(1);
				dprintf(_T("CFG.compress.split_value=%s\n"),CFG.compress.split_value.c_str());
				break;
			}

			case 'o':{
				//出力先
				switch(options[i].c_str()[1]){
					case 'd':
						//ディレクトリ
						if(options[i].c_str()[2]=='\0'){
							//指定されなかった場合デスクトップに出力
							std::vector<TCHAR> buffer(MAX_PATH);

							::SHGetSpecialFolderPath(NULL,&buffer[0],CSIDL_DESKTOP,false);
							CFG.general.output_dir.assign(&buffer[0]);
						}else{
							CFG.general.output_dir=removeTailCharacter(options[i].substr(2),'\"');
						}
						dprintf(_T("CFG.general.output_dir=%s\n"),CFG.general.output_dir.c_str());
						break;

					case 'F':
						CFG.compress.raw_file_name=true;
						dprintf(_T("CFG.compress.raw_file_name\n"));
						//fall through
					case 'f':
						//ファイル
						CFG.compress.output_file=options[i].substr(2);
						dprintf(_T("CFG.compress.output_file=%s\n"),CFG.compress.output_file.c_str());
						break;

					case 'o':
						//設定
						if(options[i].c_str()[2]=='b'){
							//カレントディレクトリ基準で'/od','/of'の相対パスを処理
							CFG.general.default_base_dir=true;
							dprintf(_T("CFG.general.default_base_dir=true\n"));
						}else if(options[i].c_str()[2]=='r'){
							//出力ファイルが重複する場合リネーム
							CFG.general.auto_rename=true;
							dprintf(_T("CFG.general.auto_rename=true\n"));
						}
						break;

					default:
						break;
				}
				break;
			}

			case 'p':{
				//パスワード
				switch(options[i].c_str()[1]){
					case 'w':{//'/mr'と'/me'で複数指定が可能(/pwfoo /pwbar)
						//'/mc'の場合
						if(CFG.general.password_list.empty()){
							CFG.general.password=options[i].substr(2);
							dprintf(_T("Password: %s\n"),CFG.general.password.c_str());
						}

						//以下'/mr'と'/me'の処理
						if(!options[i].substr(2).empty()){
							CFG.general.password_list.push_back(options[i].substr(2));
						}
						break;
					}

					case 'n':
						//再圧縮する書庫のパスワードを新しく設定
						CFG.recompress.new_password=options[i].substr(2);
						if(!options[i].substr(2).empty()){
							CFG.general.password_list.push_back(options[i].substr(2));
						}
						dprintf(_T("NewPassword: %s\n"),CFG.recompress.new_password.c_str());
						break;

					case 'f':{
						CFG.extract.password_list_path=options[i].substr(2);
						dprintf(_T("PasswordList: %s\n"),CFG.extract.password_list_path.c_str());

						File password_list_file(CFG.extract.password_list_path.c_str());

						//パスワードリストファイルに追加
						if(password_list_file.isOpened()){
							std::list<tstring> password_list;

							password_list_file.readList(&password_list,File::RL_SKIP_BOM);
							CFG.general.password_list.splice(CFG.general.password_list.end(),password_list);
						}
						break;
					}

					default:
						break;
				}
				break;
			}

			case 'P':{
				//ユーザ独自のパラメータ
				CFG.general.custom_param=options[i].substr(1);
				dprintf(_T("CFG.general.custom_param=%s\n"),CFG.general.custom_param.c_str());
				break;
			}

			case 't':{
				//更新日時を元書庫と同じにする
				CFG.compress.copy_timestamp=true;
				dprintf(_T("CFG.compress.copy_timestamp=true\n"));
				break;
			}

			case 'r':{
				//コマンド実行
				if(options[i].c_str()[1]=='\0'){
					//対話形式で指定
					CFG.recompress.run_command.interactive=true;
					dprintf(_T("CFG.recompress.run_command.interactive\n"));
				}else{
					CFG.recompress.run_command.command=options[i].substr(1);
				}
				break;
			}

			case 'b':{
				//バックグラウンドで動作
				CFG.general.background_mode=true;
				dprintf(_T("CFG.general.background_mode=true\n"));
				break;
			}

			case 'C':{
				//パスワードリストファイル/ファイルリストの文字コード
				tstring codepage_str(options[i].substr(1).c_str());

				//小文字へ変換
				transform(codepage_str.begin(),codepage_str.end(),codepage_str.begin(),tolower);

				if(codepage_str==_T("sjis")||codepage_str==_T("s-jis")||codepage_str==_T("s_jis")||codepage_str==_T("shiftjis")){
					CFG.general.codepage=File::SJIS;
				}else if(codepage_str==_T("utf8")||codepage_str==_T("utf-8")||codepage_str==_T("utf_8")){
					CFG.general.codepage=File::UTF8;
				}else if(codepage_str==_T("utf16")||codepage_str==_T("utf-16")||codepage_str==_T("utf_16")||
						 codepage_str==_T("utf16le")||codepage_str==_T("utf-16le")||codepage_str==_T("utf_16le")||
						 codepage_str==_T("unicode")){
					CFG.general.codepage=File::UTF16LE;
				}else if(codepage_str==_T("utf16be")||codepage_str==_T("utf-16be")||codepage_str==_T("utf_16be")){
					CFG.general.codepage=File::UTF16BE;
				}

				break;
			}//case 'C'

			case 'U':{//Unicodeエスケープシーケンスをデコードする
				if(options[i].find(_T("UESC"))==0){
					switch(options[i].c_str()[4]){
						case '0':
						case '-':
							CFG.general.decode_uesc=false;
							break;

						default:
							CFG.general.decode_uesc=true;
					}
				}
				dprintf(_T("CFG.general.decode_uesc=%d\n"),CFG.general.decode_uesc);
				break;
			}//case 'U'

			case 'q':{//処理終了後閉じる
				switch(options[i].c_str()[1]){
					case '0':
					case '-':
						CFG.general.quit=false;
						break;

					default:
						CFG.general.quit=true;
				}
				dprintf(_T("CFG.general.quit=%d\n"),CFG.general.quit);
				break;
			}//case 'q'

			case '@':{//リストファイル
				File list_file;

				if(!list_file.open(options[i].c_str()+1,OPEN_EXISTING,GENERIC_READ,0,CFG.general.codepage)){
					errmsg(_T("リストファイル '%s' を開くことが出来ませんでした。"),options[i].c_str()+1);
				}else{
					std::vector<tstring> file_list;

					list_file.readList(&file_list,File::RL_SKIP_BOM|File::RL_REMOVE_QUOTES);
					//取得したパスたちを処理対象リストに追加
					cmd_arg.filepaths().insert(cmd_arg.filepaths().end(),file_list.begin(),file_list.end());
				}
				break;
			}

			//TODO
			case 'w':{
				if(options[i].find(_T("wait:"))==0&&
				   options[i][5]!='\0'){
					wait=_ttoi(options[i].substr(5).c_str());
				}
			}

			default:
				break;
		}
	}

	//圧縮形式が指定されていない場合'zip'に設定
	if(CFG.compress.compression_type.empty()){
		CFG.compress.compression_type=_T("zip");
	}

	//圧縮操作で'@'が指定されたらzipに置換
	if(CFG.mode==MODE_COMPRESS&&CFG.compress.compression_type.c_str()[0]=='@'){
		CFG.compress.compression_type=_T("zip")+CFG.compress.compression_type.substr(1);
	}

	//小文字に変換
	CFG.compress.compression_type=str::toLower(CFG.compress.compression_type);

	dprintf(_T("compress.compression_type=%s\n"),CFG.compress.compression_type.c_str());

	//設定ファイルで指定されたパスワードリストを読み込む
	if(!password_list_path.empty()){
		File password_list_file(password_list_path.c_str());

		//パスワードリストファイルに追加
		if(password_list_file.isOpened()){
			std::list<tstring> password_list;

			password_list_file.readList(&password_list,File::RL_SKIP_BOM);
			CFG.general.password_list.splice(CFG.general.password_list.end(),password_list);
		}
	}

	if(CFG.general.password.empty()&&
	   !CFG.general.password_list.empty()){
		//リスト先頭のパスワードをコピー
		CFG.general.password.assign(*CFG.general.password_list.begin());
	}

	//パスワードリスト末尾に空要素追加
	//(一致するパスワードが空要素までになければ、requirePassword()を呼ぶ)
	CFG.general.password_list.push_back(_T(""));

	if(save_cfg){
		//cfgファイルに書き出す
		cfg_file.save();
	}
	return true;
}

unsigned __stdcall Reces::manageProgressBar(void* param){
	{
		MSG msg;
		static Reces* this_ptr=reinterpret_cast<Reces*>(reinterpret_cast<misc::thread::PARAM*>(param)->this_ptr);

		misc::Lock lock(this_ptr->m_progressbar_cs);

		::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE);

		//メインスレッドの処理再開
		reinterpret_cast<misc::thread::PARAM*>(param)->ready.signal();

		while(::GetMessage(&msg,NULL,0,0)>0){
			if(msg.message==WM_CREATE_PROGRESSBAR){
				this_ptr->m_progressbar=new ProgressBar(this_ptr->m_stdout);
				if(msg.wParam){
					reinterpret_cast<misc::thread::PARAM*>(msg.wParam)->ready.signal();
				}
			}else if(msg.message==Archiver::WM_UPDATE_PROGRESSBAR){
				if(!this_ptr->isTerminated()&&
				   this_ptr->m_progressbar&&
				   msg.wParam){
					Archiver::ARC_PROCESSING_INFO arc_processing_info=
						*(reinterpret_cast<Archiver::ARC_PROCESSING_INFO*>(reinterpret_cast<misc::thread::PARAM*>(msg.wParam)->data));

					reinterpret_cast<misc::thread::PARAM*>(msg.wParam)->ready.signal();

					this_ptr->m_progressbar->update(arc_processing_info.done,
										   arc_processing_info.total,
										   arc_processing_info.file_name.c_str());
				}else{
					reinterpret_cast<misc::thread::PARAM*>(msg.wParam)->ready.signal();
				}
			}else if(msg.message==WM_DESTROY_PROGRESSBAR){
				if(this_ptr->m_progressbar){
					SAFE_DELETE(this_ptr->m_progressbar);
				}
				if(msg.wParam){
					reinterpret_cast<misc::thread::PARAM*>(msg.wParam)->ready.signal();
				}
			}
		}
	}

	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::dialogHookProc(void* param){
	{
		MSG msg;
		static Reces* this_ptr=reinterpret_cast<Reces*>(reinterpret_cast<misc::thread::PARAM*>(param)->this_ptr);

		misc::Lock lock(this_ptr->m_dialog_hook_cs);

		::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE);

		//メインスレッドの処理再開
		reinterpret_cast<misc::thread::PARAM*>(param)->ready.signal();

		while(::GetMessage(&msg,NULL,0,0)>0){
			if(msg.message==WM_HOOKDIALOG){
				HWND console_handle=this_ptr->wnd();
				HWND target_handle=reinterpret_cast<HWND>(msg.wParam);
				ARCCFG->m_hook_dialog_type=msg.lParam;

				if(ARCCFG->m_hook_dialog_type!=HOOK_XACRETT_EXTRACT){
					for(size_t i=0;i<5&&console_handle!=::GetForegroundWindow();i++){
						//recesをフォアグラウンドに
						setForegroundWindow(console_handle);
						::Sleep(200);
					}
					if(CFG.general.password.empty()){
						if(!ARCCFG->m_password_input_cancelled&&
						   //パスワードの入力を求める
						   this_ptr->requirePassword()){
							//入力されたパスワードを空要素の手前に追加
							CFG.general.password_list.insert(--CFG.general.password_list.end(),
																				  CFG.general.password);
							//パスワード入力行を消去する為リセット
							if(this_ptr->m_progressbar)this_ptr->m_progressbar->reset(ProgressBar::RESET_LAST_DOTS);
						}else{
							//パスワード入力がキャンセルされた
							ARCCFG->m_password_input_cancelled=true;
							if(ARCCFG->m_hook_dialog_type!=HOOK_XACRETT_PASSWORD){
								::SendMessage(::GetDlgItem(::GetParent(target_handle),IDCANCEL),BM_CLICK,0,0);
							}else{
								::SendMessage(::GetDlgItem(target_handle,IDOK),BM_CLICK,0,0);
							}
						}
					}

					if(this_ptr->isTerminated()){
						//OK押下
						::SendMessage(::GetDlgItem(::GetParent(target_handle),IDOK),BM_CLICK,0,0);
					}else{
						//パスワード入力
						::SendMessage(target_handle,WM_SETTEXT,(WPARAM)0,(LPARAM)CFG.general.password.c_str());

						//OK押下
						::SendMessage(::GetDlgItem(::GetParent(target_handle),IDOK),BM_CLICK,0,0);
					}
				}else{
					ARCCFG->m_extracting_wnd_handle=target_handle;
				}

				if(ARCCFG->m_hook_dialog_type!=HOOK_XACRETT_EXTRACT){
					//recesをフォアグラウンドに
					setForegroundWindow(console_handle);
				}
			}
		}
	}

	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callCompress(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->compress(((ARC_THREAD_PARAM*)param)->file_list,
														   ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callExtract(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->extract(((ARC_THREAD_PARAM*)param)->file_name,
														  ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callList(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->list(((ARC_THREAD_PARAM*)param)->file_name,
												   ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callSendCommands(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->sendCommands(((ARC_THREAD_PARAM*)param)->file_list,
														   ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callTest(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->test(((ARC_THREAD_PARAM*)param)->file_name,
												   ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callSettings(void* param){
	{
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			((ARC_THREAD_PARAM*)param)->this_ptr->settings(((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
	}
	_endthreadex(0);
	return 0;
}

Reces::ARC_RESULT Reces::compress(std::list<tstring>& compress_file_list,tstring& err_msg){
	misc::Lock lock(this->m_arc_cs);

	if(isTerminated())return ARC_USER_CANCEL;

	m_arc_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

	for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
		if(m_arcdll_list[i]->isSupportedMethod((CFG.compress.compression_type.c_str()[0]!='@')?
											   CFG.compress.compression_type.c_str():
											   m_cur_file.recompress_mhd.c_str())){
			m_arc_dll=m_arcdll_list[i];
			break;
		}
	}

	if(!m_arc_dll){
		STDOUT.outputString(_T("\n"));
		err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
		return ARC_NOT_SUPPORTED_METHOD;
	}

	if(m_arc_dll->type()==Archiver::CAL){
		if((CFG.mode==MODE_RECOMPRESS||CFG.mode==MODE_COMPRESS)&&
		   static_cast<ArcDll*>(m_arc_dll)->getMethod().opt&ArcDll::MHD_SFX&&
		   !CFG.compress.split_value.empty()){
			err_msg=_T("オプションの組み合わせが正しくありません。(sfx+/s)\n");
			return ARC_INCORRECT_OPTION_SFX_S;
		}
	}

	if(isTerminated())return ARC_USER_CANCEL;

	if(!m_arc_dll->isLoaded()&&!m_arc_dll->load()){
		STDOUT.outputString(_T("\n"));
		VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),m_arc_dll->name().c_str());
		err_msg=error_message.get();
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		if(isTerminated())return ARC_USER_CANCEL;

		{
			//'od'と'of'を反映した作成する書庫のパスを作成
			if(arcFileName(&m_cur_file,m_cur_file.arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
				return ARC_CANNOT_CREATE_DIRECTORY;
			}

			if(path::isDirectory(m_cur_file.arc_path.c_str())&&
			   CFG.compress.raw_file_name){
				//作成する書庫と同名のディレクトリが存在する場合oFを無効にして再度パス作成
				CFG.compress.raw_file_name=false;
				if(arcFileName(&m_cur_file,m_cur_file.arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
					return ARC_CANNOT_CREATE_DIRECTORY;
				}
			}
		}

		//本来の出力先ディレクトリ
		tstring orig_dir;
		bool split_file=!CFG.compress.split_value.empty();

		if(split_file){
			orig_dir=path::getParentDirectory(m_cur_file.arc_path);
			m_cur_file.arc_path=m_split_temp_dir+_T("\\")+path::getFileName(m_cur_file.arc_path);
		}

		if(isTerminated())return ARC_USER_CANCEL;

		if(CFG.general.ignore_directory_structures){
			//ディレクトリ階層を無視する場合/ebを無効に
			CFG.compress.exclude_base_dir=0;
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_CREATE_PROGRESSBAR);
		}
		//コールバック関数を登録
		m_arc_dll->setCallback(m_progressbar_thread.id);

		if(isTerminated())return ARC_USER_CANCEL;

		//圧縮
		tstring log;
		bool result=false;

		switch(m_arc_dll->compress(m_cur_file.arc_path.c_str(),&compress_file_list,&log)){
			case Archiver::ARC_SUCCESS:
				result=true;
				break;
			case Archiver::ARC_CANNOT_OPEN_LISTFILE:
				err_msg=_T("リストファイルを開くことが出来ませんでした。\n");
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				err_msg=_T("フィルタに一致するファイルはありません。\n");
				break;
			case Archiver::ARC_FAILURE:
			default:
				err_msg=_T("エラーが発生しました。\n");
				break;
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
		}
		//コールバック関数の登録を解除
		m_arc_dll->clearCallback();

		if(!result){
			void* msg_buffer=NULL;

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT),
							static_cast<TCHAR*>(msg_buffer),
							0,
							NULL
							);
			if(msg_buffer!=NULL)errmsg(_T("%s\n"),static_cast<TCHAR*>(msg_buffer));
			::LocalFree(msg_buffer);
		}

		if(!CFG.no_display.no_log){
			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
		}

		if(split_file){
			//分割処理
			info(_T("ファイルを分割しています...\n"));
			switch(fileoperation::splitFile(m_cur_file.arc_path.c_str(),CFG.compress.split_value.c_str(),orig_dir.c_str())){
				case fileoperation::SFRET_SUCCESS:
					//一時ディレクトリ内のファイル/ディレクトリを削除
					fileoperation::deleteContents(m_split_temp_dir.c_str());
					return ARC_SUCCESS;
				case fileoperation::SFRET_CANNOT_OPEN:
					err_msg=_T("分割対象ファイル '%s' を開くことが出来ませんでした。\n"),m_cur_file.arc_path.c_str();
					break;
				case fileoperation::SFRET_INVALID_PARAM:
					err_msg=_T("分割サイズまたは分割数が正しくありません。\n");
					break;
				case fileoperation::SFRET_NOT_NECESSARY:
					err_msg=_T("分割する必要がありません。\n");
					break;
				case fileoperation::SFRET_TOO_MANY:
					err_msg=_T("分割数が多すぎます。\n");
					break;
				case fileoperation::SFRET_MALLOC_ERR:
					err_msg=_T("メモリの確保に失敗しました。\n");
					break;
				case fileoperation::SFRET_CANNOT_CREATE:
					err_msg=_T("分割ファイルの作成に失敗しました。\n");
					break;
			}
			//エラーであってもとりあえずコピー
			//ファイル名を作成
			orig_dir+=_T("\\")+path::getFileName(m_cur_file.arc_path);
			::CopyFile(m_cur_file.arc_path.c_str(),orig_dir.c_str(),false);
			//一時ディレクトリ内のファイル/ディレクトリを削除
			fileoperation::deleteContents(m_split_temp_dir.c_str());
		}
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}

Reces::ARC_RESULT Reces::extract(const tstring& arc_path,tstring& err_msg){
	misc::Lock lock(this->m_arc_cs);

	if(isTerminated())return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(fileoperation::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case fileoperation::JFRET_SUCCESS:{
				split_file=true;
				info(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir);
				join_file_name+=path::removeExtension(path::getFileName(arc_path));
				break;
			}
			case fileoperation::JFRET_NOT_SPLIT:
			default:
				//分割された書庫ではない
				break;
			case fileoperation::JFRET_CANNOT_CREATE:
			case fileoperation::JFRET_MALLOC_ERR:
				err_msg=_T("ファイルの結合に失敗しました。\n");
				return ARC_FAILURE;
		}
	}

	m_arc_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	//7-zip32とLMZIP32を入れ替える
	swap7ZLMZIP(arc_path.length()>=MAX_PATH);

	//ファイルがロックされていないかテスト
	if(wait){
		bool success=false;
		int retries=0;
		const int max_retries=5;

		do{
			HANDLE handle=::CreateFile(arc_path.c_str(),
									   GENERIC_READ,
									   0,
									   NULL,
									   OPEN_EXISTING,
									   FILE_ATTRIBUTE_NORMAL,
									   NULL);

			if(handle==INVALID_HANDLE_VALUE){
				retries++;
				::Sleep(wait);
				continue;
			}
			success=true;
			::CloseHandle(handle);
			break;
		}while(retries<max_retries);

		if(!success){
			info(_T("\n"));
			err_msg+=_T("ファイルを開くことが出来ません。");
			err_msg+=arc_path.c_str();
			err_msg+=_T("\n");
			return ARC_FAILURE;
		}
	}

	{
		bool loaded_library=false;

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}else{
			//拡張子からの推測
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   path::getExtension(arc_path).c_str());

			//総当たり
			if(!m_arc_dll){
				m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
									   m_arcdll_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=LoadAndCheckPlugin(&m_spi_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.spi_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::SPI_AM);
			}else{
				m_arc_dll=loadAndCheck(m_spi_list.begin(),
									   m_spi_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=LoadAndCheckPlugin(&m_wcx_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.wcx_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::WCX);
			}else{
				m_arc_dll=loadAndCheck(m_wcx_list.begin(),
									   m_wcx_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(ARCCFG->m_password_input_cancelled){
				STDOUT.outputString(_T("\n"));
				err_msg=_T("パスワードの入力がキャンセルされました。\n");
				return ARC_INPUT_PASSWORD_CANCEL;
			}else if(!CFG.general.selected_library_name.empty()&&!loaded_library){
				STDOUT.outputString(_T("\n"));
				VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
				err_msg=error_message.get();
				return ARC_CANNOT_LOAD_LIBRARY;
			}else{
				STDOUT.outputString(_T("\n"));
				err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
				return ARC_NOT_SUPPORTED_METHOD;
			}
		}
	}

	if(isTerminated())return ARC_USER_CANCEL;

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		if(m_arc_dll->type()==Archiver::CAL&&
		   CFG.mode==MODE_RECOMPRESS){
			//'@'の処理で
			if(CFG.compress.compression_type.c_str()[0]=='@'&&
			   //一括再圧縮の一度目か
			   ((!CFG.compress.each_file&&m_cur_file.recompress_mhd.empty())||
				//'/e'
			   CFG.compress.each_file)){
				DWORD mhd_opt=0;

				if(CFG.compress.compression_type.find(_T("pw"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_PASSWORD;
				}

				if(CFG.compress.compression_type.find(_T("he"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_HEADERENCRYPTION;
				}

				if(CFG.compress.compression_type.find(_T("sfx"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_SFX;
				}

				if(m_arc_dll){
					m_cur_file.recompress_mhd.assign(static_cast<ArcDll*>(m_arc_dll)->getCompressionMethod((!split_file)?arc_path.c_str():join_file_name.c_str()));
				}

				if(m_cur_file.recompress_mhd.empty())m_cur_file.recompress_mhd=_T("zip");
				if(mhd_opt&ArcDll::MHD_PASSWORD)m_cur_file.recompress_mhd+=_T("pw");
				if(mhd_opt&ArcDll::MHD_HEADERENCRYPTION)m_cur_file.recompress_mhd+=_T("he");
				if(mhd_opt&ArcDll::MHD_SFX)m_cur_file.recompress_mhd+=_T("sfx");
			}
		}

		if(isTerminated())return ARC_USER_CANCEL;

		tstring output_dir(path::getParentDirectory(arc_path));
		bool created_dir=false;

		if(!CFG.general.output_dir.empty()){
			std::vector<TCHAR> buffer(MAX_PATHW);

			path::getFullPath(&buffer[0],
							  buffer.size(),
							  CFG.general.output_dir.c_str(),
							  ((!CFG.general.default_base_dir)?
							   output_dir.c_str():
							   m_original_cur_dir.c_str()));
			output_dir.assign(&buffer[0]);
			if(!path::fileExists(output_dir.c_str())){
				if(!fileoperation::createDirectory(output_dir.c_str())){
					VariableArgument error_message(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
					err_msg=error_message.get();
					return ARC_CANNOT_CREATE_DIRECTORY;
				}
			}
		}

		if(m_arc_dll->type()==Archiver::CAL&&
		   CFG.mode==MODE_RECOMPRESS&&
		   CFG.recompress.run_command.disable()&&
		   CFG.compress.each_file&&
		   !CFG.general.ignore_directory_structures&&
		   CFG.general.custom_param.empty()&&
		   CFG.compress.exclude_base_dir==0&&
		   !CFG.extract.create_dir&&
		   !CFG.general.decode_uesc){
			//可能であれば解凍->圧縮ではなく、ライブラリの削除コマンドを使用して処理

			tstring current_mhd=static_cast<ArcDll*>(m_arc_dll)->getCompressionMethod((!split_file)?arc_path.c_str():join_file_name.c_str());

			if(!current_mhd.empty()){
				if(str::isEqualStringIgnoreCase((CFG.compress.compression_type.c_str()[0]!='@')?
												CFG.compress.compression_type:
												m_cur_file.recompress_mhd,
												current_mhd)){

					tstring old_arc_path((!split_file)?arc_path:join_file_name);
					CUR_FILE new_cur_file(old_arc_path);

					if(str::isEqualStringIgnoreCase(path::getExtension(old_arc_path),_T("exe"))){
						//拡張子がexeなら削る
						new_cur_file.arc_path=old_arc_path=removeExtensionEx(old_arc_path);
					}

					tstring ext=getExtensionEx(old_arc_path);
					tstring old_arc_path_bak(old_arc_path);
					bool need_ext=false;

					if(CFG.general.auto_rename&&
					   !ext.empty()&&
					   old_arc_path.find(str::toLower(ext))!=tstring::npos){
						ext=_T(".")+getExtensionEx(old_arc_path);
						old_arc_path=removeExtensionEx(old_arc_path);
						need_ext=true;
					}

					if(arcFileName(&new_cur_file,old_arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
						return ARC_CANNOT_CREATE_DIRECTORY;
					}

					if(CFG.general.auto_rename&&
					   need_ext){
						new_cur_file.arc_path=removeExtensionEx(new_cur_file.arc_path);
						new_cur_file.arc_path+=ext;
					}

					//出力するファイルのパスが同一か、異なる場合はそのファイルが存在しない場合のみ処理
					if(old_arc_path==new_cur_file.arc_path||
					   (!path::fileExists(new_cur_file.arc_path.c_str())&&old_arc_path!=new_cur_file.arc_path)){
						//オリジナルとパスが異なるようであればコピー
						bool copy_file=!split_file&&
							old_arc_path!=new_cur_file.arc_path;

						if(copy_file){
							::CopyFile(arc_path.c_str(),
									   new_cur_file.arc_path.c_str(),false);
						}else if(split_file){
							::MoveFileEx(join_file_name.c_str(),
										 new_cur_file.arc_path.c_str(),
										 MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
						}

						tstring log;

						if(static_cast<ArcDll*>(m_arc_dll)->del(new_cur_file.arc_path.c_str(),&log)==Archiver::ARC_SUCCESS){
							if(!CFG.no_display.no_log){
								STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
							}

							m_cur_file=new_cur_file;

							return ARC_DELETE_COMMAND;
						}else{
							if(copy_file){
								//失敗したため削除
								::DeleteFile(new_cur_file.arc_path.c_str());
							}else if(split_file){
								//ファイルを元の位置に戻す
								::MoveFileEx(new_cur_file.arc_path.c_str(),join_file_name.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
							}
						}
					}
				}
			}
		}

		if(CFG.mode==MODE_RECOMPRESS){
			output_dir=ARCCFG->m_recmp_temp_dir.c_str();
		}

		//新規ディレクトリ作成
		if(CFG.extract.create_dir){
			bool redundant_dir=false;

			//ディレクトリ階層を無視する場合は二重ディレクトリを考慮しない
			if(CFG.general.ignore_directory_structures){
				CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=false;
			}

			//パスを二つ以上除く場合であれば二重ディレクトリ防止を無効に
			if(CFG.compress.exclude_base_dir==-1||
			   CFG.compress.exclude_base_dir){
				CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=false;
			}

			if(CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir||
			   CFG.extract.create_dir_optimization.remove_redundant_dir.only_file){
				redundant_dir=
					m_arc_dll->isRedundantDir((!split_file)?arc_path.c_str():join_file_name.c_str(),
											  CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir,
											  CFG.extract.create_dir_optimization.remove_redundant_dir.only_file);
			}

			if(!redundant_dir){
				//新規ディレクトリ名作成
				tstring new_dir_name(path::getFileName((!split_file)?arc_path:join_file_name));

				//tar系を考慮しつつ拡張子削除
				new_dir_name=removeExtensionEx(new_dir_name);

				if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number&&
				   CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
					//ディレクトリ名末尾の数字と記号を削除
					new_dir_name=path::removeLastNumberAndSymbol(new_dir_name);
				}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number){
					//ディレクトリ名末尾の数字を削除
					new_dir_name=path::removeLastNumber(new_dir_name);
				}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
					//ディレクトリ名末尾の記号を削除
					new_dir_name=path::removeLastSymbol(new_dir_name);
				}

				output_dir=path::addTailSlash(output_dir)+=new_dir_name;

				created_dir=fileoperation::createDirectory(output_dir.c_str());
				if(!created_dir){
					//コールバック関数の登録を解除
					m_arc_dll->clearCallback();
					VariableArgument error_message(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
					err_msg=error_message.get();
					return ARC_CANNOT_CREATE_DIRECTORY;
				}
			}
		}

		//区切り文字を末尾に追加
		output_dir=path::addTailSlash(output_dir);

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_CREATE_PROGRESSBAR);
		}
		//コールバック関数を登録
		m_arc_dll->setCallback(m_progressbar_thread.id);

		if(isTerminated())return ARC_USER_CANCEL;

		//解凍
		tstring log;
		bool result=false;

		switch(m_arc_dll->extract((!split_file)?arc_path.c_str():join_file_name.c_str(),
								  output_dir.c_str(),
								  &log)){
			case Archiver::ARC_SUCCESS:
				result=true;
				break;
			case Archiver::ARC_CANNOT_OPEN_LISTFILE:
				errmsg(_T("リストファイルを開くことが出来ませんでした。\n"));
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				errmsg(_T("フィルタに一致するファイルはありません。\n"));
				break;
			case Archiver::ARC_FAILURE:
			default:
				err_msg=_T("エラーが発生しました。\n");
				break;
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
		}
		//コールバック関数の登録を解除
		m_arc_dll->clearCallback();

		if(created_dir&&
		   CFG.extract.create_dir_optimization.copy_timestamp){
			//作成したディレクトリの更新日時を書庫と同じにする
			File arc(arc_path.c_str(),
						  OPEN_EXISTING,
						  GENERIC_READ,
						  FILE_SHARE_READ);
			File dir(output_dir.c_str());
			FILETIME ft={};

			arc.getFileTime(&ft);
			dir.setFileTime(&ft);
		}

		if(!result){
			void* msg_buffer=NULL;

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT),
							static_cast<TCHAR*>(msg_buffer),
							0,
							NULL
							);
			if(msg_buffer!=NULL)errmsg(_T("%s\n"),static_cast<TCHAR*>(msg_buffer));
			::LocalFree(msg_buffer);
		}

		if(!CFG.no_display.no_log){
			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
		}
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}

Reces::ARC_RESULT Reces::list(const tstring& arc_path,tstring& err_msg){
	if(isTerminated())return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(fileoperation::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case fileoperation::JFRET_SUCCESS:{
				split_file=true;
				STDOUT.outputString(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir.c_str());
				join_file_name+=removeExtensionEx(path::getFileName(arc_path));
				break;
			}
			case fileoperation::JFRET_NOT_SPLIT:
			default:
				//分割された書庫ではない
				break;
			case fileoperation::JFRET_CANNOT_CREATE:
			case fileoperation::JFRET_MALLOC_ERR:
				err_msg=_T("ファイルの結合に失敗しました。\n");
				return ARC_FAILURE;
		}
	}

	m_arc_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	//7-zip32とLMZIP32を入れ替える
	swap7ZLMZIP(arc_path.length()>=MAX_PATH);

	{
		bool loaded_library=false;

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}else{
			//拡張子からの推測
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   path::getExtension(arc_path).c_str());

			//総当たり
			if(!m_arc_dll){
				m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
									   m_arcdll_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=LoadAndCheckPlugin(&m_spi_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.spi_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::SPI_AM);
			}else{
				m_arc_dll=loadAndCheck(m_spi_list.begin(),
									   m_spi_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=LoadAndCheckPlugin(&m_wcx_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.wcx_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::WCX);
			}else{
				m_arc_dll=loadAndCheck(m_wcx_list.begin(),
									   m_wcx_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(ARCCFG->m_password_input_cancelled){
				STDOUT.outputString(_T("\n"));
				err_msg=_T("パスワードの入力がキャンセルされました。\n");
				return ARC_INPUT_PASSWORD_CANCEL;
			}else if(!CFG.general.selected_library_name.empty()&&!loaded_library){
				STDOUT.outputString(_T("\n"));
				VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
				err_msg=error_message.get();
				return ARC_CANNOT_LOAD_LIBRARY;
			}else{
				STDOUT.outputString(_T("\n"));
				err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
				return ARC_NOT_SUPPORTED_METHOD;
			}
		}
	}

	if(isTerminated())return ARC_USER_CANCEL;

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		//リスト出力
		switch(m_arc_dll->list((!split_file)?arc_path.c_str():join_file_name.c_str())){
			case Archiver::ARC_SUCCESS:
				return ARC_SUCCESS;
			case Archiver::ARC_FAILURE:
			default:
				return ARC_FAILURE;
		}
	}
	return ARC_SUCCESS;
}

Reces::ARC_RESULT Reces::sendCommands(std::list<tstring>& commands_list,tstring& err_msg){
	if(isTerminated())return ARC_USER_CANCEL;

	m_arc_dll=NULL;
	m_cal_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

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

		if(!m_arc_dll){
			STDOUT.outputString(_T("\n"));
			VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
			err_msg=error_message.get();
			return ARC_CANNOT_LOAD_LIBRARY;
		}
	}

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
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

Reces::ARC_RESULT Reces::test(const tstring& arc_path,tstring& err_msg){
	if(isTerminated())return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(fileoperation::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case fileoperation::JFRET_SUCCESS:{
				split_file=true;
				STDOUT.outputString(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir.c_str());
				join_file_name+=removeExtensionEx(path::getFileName(arc_path));
				break;
			}
			case fileoperation::JFRET_NOT_SPLIT:
			default:
				//分割された書庫ではない
				break;
			case fileoperation::JFRET_CANNOT_CREATE:
			case fileoperation::JFRET_MALLOC_ERR:
				err_msg=_T("ファイルの結合に失敗しました。\n");
				return ARC_FAILURE;
		}
	}

	m_arc_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	//7-zip32とLMZIP32を入れ替える
	swap7ZLMZIP(arc_path.length()>=MAX_PATH);

	{
		bool loaded_library=false;

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   NULL,
								   NULL,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}else{
			//拡張子からの推測
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   NULL,
								   NULL,
								   path::getExtension(arc_path).c_str());

			//リスト先頭から読み込み
			if(!m_arc_dll){
				m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
									   m_arcdll_list.end());
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=LoadAndCheckPlugin(&m_wcx_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.wcx_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::WCX);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()&&!loaded_library){
				STDOUT.outputString(_T("\n"));
				VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
				err_msg=error_message.get();
				return ARC_CANNOT_LOAD_LIBRARY;
			}else{
				STDOUT.outputString(_T("\n"));
				err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
				return ARC_NOT_SUPPORTED_METHOD;
			}
		}
	}

	if(isTerminated())return ARC_USER_CANCEL;

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		//リスト出力
		switch(m_arc_dll->test((!split_file)?arc_path.c_str():join_file_name.c_str())){
			case Archiver::ARC_SUCCESS:
				return ARC_SUCCESS;
			case Archiver::ARC_FAILURE:
			default:
				return ARC_FAILURE;
		}
	}
	return ARC_FAILURE;
}

Reces::ARC_RESULT Reces::settings(tstring& err_msg){
	if(isTerminated())return ARC_USER_CANCEL;

	m_arc_dll=NULL;
	m_cal_dll=NULL;

	if(CFG.general.selected_library_name.empty()){
		err_msg=(_T("ライブラリが指定されていません。\n"));
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	info(_T("ライブラリを読み込んでいます..."));

	m_arc_dll=LoadAndCheckPlugin(&m_spi_list,
								 NULL,
								 NULL,
								 CFG.general.spi_dir,
								 CFG.general.selected_library_name.c_str(),
								 Archiver::SPI_AM);

	if(!m_arc_dll){
		m_arc_dll=LoadAndCheckPlugin(&m_wcx_list,
									 NULL,
									 NULL,
									 CFG.general.wcx_dir,
									 CFG.general.selected_library_name.c_str(),
									 Archiver::WCX);
	}

	if(!m_arc_dll){
		tstring library_name=path::getFileName(CFG.general.selected_library_name);

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
	}

	if(!m_arc_dll){
		STDOUT.outputString(_T("\n"));
		VariableArgument error_message(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
		err_msg=error_message.get();
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		//ライブラリの設定ダイアログ呼び出し
		if(m_arc_dll->configurationDialog(wnd())){
			return ARC_SUCCESS;
		}else{
			err_msg=_T("設定に失敗したか、設定ダイアログが用意されていません。\n");
			return ARC_FAILURE;
		}
	}
	return ARC_FAILURE;
}

bool Reces::run(CommandArgument& cmd_arg){
	//オプション解析
	if(!parseOptions(cmd_arg)){
		return false;
	}

	if(CFG.mode!=MODE_VERSION&&
	   CFG.mode!=MODE_SETTINGS&&
	   !cmd_arg.filepaths().size()){
		errmsg(_T("引数が不足しています。\n"));
		return false;
	}

	//mr@ /N /oF /e
	if(CFG.mode==MODE_RECOMPRESS&&
	   CFG.compress.compression_type.c_str()[0]=='@'&&
	   CFG.compress.create_new&&
	   CFG.compress.raw_file_name&&
	   CFG.compress.each_file){
		errmsg(_T("オプションの組み合わせが正しくありません。(/mr@+/N+/oF+/e)\n"));
		return false;
	}

	//spi読み込み
	searchSpi((!CFG.general.spi_dir.empty())?
			  CFG.general.spi_dir.c_str():
			  path::getExeDirectory().c_str());

	//wcx読み込み
	searchWcx((!CFG.general.wcx_dir.empty())?
			  CFG.general.wcx_dir.c_str():
			  path::getExeDirectory().c_str());

	if(!STDOUT.isRedirected()&&
	   !CFG.no_display.no_information){
		//プログレスバーを管理するスレッドを作成
		m_progressbar_thread.handle=misc::thread::create(&Reces::manageProgressBar,&m_progressbar_thread.id,this);
	}

	//ダイアログのフック通知を受け取るスレッドを作成
	m_dialog_hook_thread.handle=misc::thread::create(&Reces::dialogHookProc,&m_dialog_hook_thread.id,this);

	std::vector<tstring>& filepaths=cmd_arg.filepaths();

	switch(CFG.mode){
		case MODE_RECOMPRESS:{
			//パスワードダイアログのフックを開始
			if(m_pInstallHook){
				if(!m_pInstallHook(::GetCurrentProcessId(),m_dialog_hook_thread.id,WM_HOOKDIALOG)){
					errmsg(_T("フック用ライブラリの設定に失敗しました。\n"));
				}
			}

			std::list<tstring> extract_file_list;

			if(!fullPathList(extract_file_list,filepaths)){
				errmsg(_T("そのようなファイルは存在しません。\n"));
				return false;
			}else if(extract_file_list.size()==1){
				//対象ファイルが一つなら/e扱いに
				CFG.compress.each_file=true;
			}

			info(_T("\n"));

			//元書庫のタイムスタンプ
			FILETIME orig_arc_timestamp;

			//処理後削除対象リスト
			std::list<std::pair<tstring,tstring> > remove_list;

			ARCCFG->m_password_input_cancelled=false;
			ARC_RESULT result=ARC_FAILURE;
			tstring err_msg;
			ARC_RESULT_MSG result_msg(result,err_msg);

			for(std::list<tstring>::iterator ite_list=extract_file_list.begin(),
				list_begin=extract_file_list.begin(),
				list_end=extract_file_list.end();
				ite_list!=list_end;
				++ite_list){
				ARC_THREAD_PARAM thread_param(this,*ite_list,result_msg);

				if(((!CFG.compress.each_file&&ite_list==list_begin)||
					   CFG.compress.each_file)&&
				   CFG.compress.copy_timestamp){
					//元書庫のタイムスタンプを保存
					File orig_arc(ite_list->c_str(),
								  OPEN_EXISTING,
								  GENERIC_READ,
								  FILE_SHARE_READ);

					orig_arc.getFileTime(&orig_arc_timestamp);
				}

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callExtract,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);

				::CloseHandle(m_arc_thread);

				switch(result){
					case ARC_SUCCESS:{
						if((!CFG.compress.each_file&&ite_list==list_begin)||
						   CFG.compress.each_file){
							if(fileoperation::isSplitFile(ite_list->c_str())){
								//分割ファイル
								m_cur_file.arc_path=path::removeExtension(*ite_list);
							}else{
								m_cur_file.arc_path=*ite_list;
							}

							if(!CFG.compress.raw_file_name){
								bool is_supported_ext=false;
								tstring ext=path::getExtension(m_cur_file.arc_path);

								for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
									if(m_arcdll_list[i]!=NULL){
										is_supported_ext=m_arcdll_list[i]->isSupportedExtension(ext.c_str());
										if(is_supported_ext)break;
									}
								}

								if(!is_supported_ext){
									for(size_t i=0,list_size=m_spi_list.size();i<list_size;i++){
										if(m_spi_list[i]!=NULL){
											//spiは拡張子で判断するためisSupportedArchive()を利用
											is_supported_ext=m_spi_list[i]->isSupportedArchive(m_cur_file.arc_path.c_str());
											if(is_supported_ext)break;
										}
									}
								}

								if(is_supported_ext||
								   str::isEqualStringIgnoreCase(path::getExtension(m_cur_file.arc_path),_T("exe"))){
									//元書庫の拡張子が対応している書庫のものかexeであれば
									//tar系を考慮しつつ拡張子削除
									m_cur_file.arc_path=removeExtensionEx(m_cur_file.arc_path);
								}
							}
						}

						if(!CFG.compress.each_file){
							if(CFG.general.remove_source!=RMSRC_DISABLE&&
							   ite_list!=list_begin){
								remove_list.push_back(std::make_pair(_T(""),*ite_list));
							}
							if(++ite_list!=list_end){
								--ite_list;
								continue;
							}
							--ite_list;
						}

						//コマンド実行
						if(!CFG.recompress.run_command.disable())runCommand();

						if(isTerminated())break;

						::SetCurrentDirectory(ARCCFG->m_recmp_temp_dir.c_str());

						std::list<tstring> dummy_list;
						ARC_RESULT result=ARC_FAILURE;
						tstring err_msg;
						ARC_RESULT_MSG result_msg(result,err_msg);
						ARC_THREAD_PARAM thread_param(this,dummy_list,result_msg);

						m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callCompress,&thread_param,0,NULL);

						::WaitForSingleObject(m_arc_thread,INFINITE);

						::CloseHandle(m_arc_thread);

						if(!m_cur_file.auto_renamed&&
						   CFG.general.remove_source!=RMSRC_DISABLE){
							if(CFG.compress.each_file){
								remove_list.push_back(std::make_pair(m_cur_file.arc_path,*ite_list));
							}else{
								remove_list.push_back(std::make_pair(m_cur_file.arc_path,*list_begin));
							}
						}

						switch(result){
							case ARC_SUCCESS:{
								if(!isTerminated()){
									if(CFG.compress.copy_timestamp){
										//タイムスタンプを設定
										copyArcTimestamp(
											(!CFG.compress.split_value.empty())?
											//分割ファイル
											(path::getParentDirectory(*ite_list)+
											 _T("\\")+
											 path::getFileName(m_cur_file.arc_path)+
											 _T(".")+
											 path::createPartExtension(1)):
											//通常ファイル
											 m_cur_file.arc_path,
											&orig_arc_timestamp);
									}

									if(CFG.general.remove_source!=RMSRC_DISABLE){
										for(std::list<std::pair<tstring,tstring> >::iterator ite=remove_list.begin(),
											end=remove_list.end();
											ite!=end;
											++ite){
											if(!str::isEqualStringIgnoreCase(ite->first,ite->second)&&
											   !fileoperation::removeSplitFile(ite->second.c_str(),CFG.general.remove_source==RMSRC_RECYCLEBIN)){
												dprintf(_T("removeFile(%s)\n"),ite->second.c_str());
												removeFile(ite->second.c_str());
											}
										}
										remove_list.clear();
									}
								}
								break;
							}//ARC_SUCCESS(Compress)

							default:
								errmsg(err_msg.c_str());
								setExitCode(EXIT_FAILURE);
								break;
						}//switch
						break;
					}//ARC_SUCCESS(Extract)

					case ARC_DELETE_COMMAND:{
						//再圧縮ではなく削除コマンドで対応
						if(CFG.compress.copy_timestamp){
							//タイムスタンプを設定
							copyArcTimestamp(
								(!CFG.compress.split_value.empty())?
								//分割ファイル
								(path::getParentDirectory(*ite_list)+
								 _T("\\")+
								 path::getFileName(m_cur_file.arc_path)+
								 _T(".")+
								 path::createPartExtension(1)):
								//通常ファイル
								 m_cur_file.arc_path,
								&orig_arc_timestamp);
						}
						if(CFG.general.remove_source!=RMSRC_DISABLE&&
						   !m_cur_file.auto_renamed){
							if(!str::isEqualStringIgnoreCase(m_cur_file.arc_path,*ite_list)&&
								!fileoperation::removeSplitFile(ite_list->c_str(),CFG.general.remove_source==RMSRC_RECYCLEBIN)){
								dprintf(_T("removeFile(%s)\n"),ite_list->c_str());
								removeFile(ite_list->c_str());
							}
						}
						break;
					}//ARC_DELETE_COMMAND

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
				if(CFG.compress.each_file){
					//一時ディレクトリ内のファイル/ディレクトリを削除
					fileoperation::deleteContents(ARCCFG->m_recmp_temp_dir.c_str());
				}
			}

			break;
		}

		case MODE_COMPRESS:{
			//パスワードダイアログのフックを開始
			if(m_pInstallHook){
				if(!m_pInstallHook(::GetCurrentProcessId(),m_dialog_hook_thread.id,WM_HOOKDIALOG)){
					errmsg(_T("フック用ライブラリの設定に失敗しました。\n"));
				}
			}

			//元書庫のタイムスタンプ
			FILETIME orig_arc_timestamp;

			std::list<tstring> compress_file_list;

			if(!fullPathList(compress_file_list,filepaths,false)){
				errmsg(_T("そのようなファイルは存在しません。\n"));
				return false;
			}
			info(_T("\n"));

			std::list<tstring> compress_file_list_bak=compress_file_list;

			std::list<tstring>::iterator ite_list=compress_file_list_bak.begin();
			std::list<tstring>::iterator list_end=compress_file_list_bak.end();

			ARC_RESULT result=ARC_FAILURE;
			tstring err_msg;
			ARC_RESULT_MSG result_msg(result,err_msg);
			ARC_THREAD_PARAM thread_param(this,compress_file_list,result_msg);

			do{
				if(CFG.compress.each_file){
					//リストの保持するアイテムを常に一つにする
					compress_file_list.clear();
					compress_file_list.push_back(*ite_list);
				}

				if(CFG.compress.copy_timestamp){
					//元書庫のタイムスタンプを保存
					File orig_arc(compress_file_list.begin()->c_str(),
								  OPEN_EXISTING,
								  GENERIC_READ,
								  FILE_SHARE_READ);

					orig_arc.getFileTime(&orig_arc_timestamp);
				}

				if(!CFG.compress.raw_file_name&&
				   !CFG.compress.output_file.empty()){
					m_cur_file.arc_path=*compress_file_list.begin();
				}else{
					if(path::isDirectory(compress_file_list.begin()->c_str())){
						m_cur_file.arc_path=path::removeTailSlash(*compress_file_list.begin());
					}else{
						m_cur_file.arc_path=path::removeExtension(*compress_file_list.begin());
					}
				}

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callCompress,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);

				::CloseHandle(m_arc_thread);

				switch(result){
					case ARC_SUCCESS:
						if(!isTerminated()){
							if(CFG.compress.copy_timestamp){
								//タイムスタンプを設定
								copyArcTimestamp(
									(!CFG.compress.split_value.empty())?
									//分割ファイル
									(path::getParentDirectory(*compress_file_list.begin())+
									 _T("\\")+
									 path::getFileName(m_cur_file.arc_path)+
										 _T(".")+
									 path::createPartExtension(1)):
									//通常ファイル
									 m_cur_file.arc_path,
									&orig_arc_timestamp);
							}
							if(CFG.general.remove_source!=RMSRC_DISABLE){
								for(std::list<tstring>::iterator ite=compress_file_list.begin(),
									end=compress_file_list.end();
									ite!=end;
									++ite){
									if(m_cur_file.arc_path!=*ite){
										dprintf(_T("removeFile(%s)\n"),ite->c_str());
										removeFile(ite->c_str());
									}
								}
							}
						}
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
			}while(CFG.compress.each_file&&
				   (++ite_list)!=list_end);

			if(m_pUninstallHook){
				m_pUninstallHook();
			}
			break;
		}

		case MODE_EXTRACT:{
			//パスワードダイアログのフックを開始
			if(m_pInstallHook){
				if(!m_pInstallHook(::GetCurrentProcessId(),m_dialog_hook_thread.id,WM_HOOKDIALOG)){
					errmsg(_T("フック用ライブラリの設定に失敗しました。\n"));
				}
			}
			std::list<tstring> extract_file_list;

			if(!fullPathList(extract_file_list,filepaths)){
				errmsg(_T("そのようなファイルは存在しません。\n"));
				return false;
			}
			info(_T("\n"));

			for(std::list<tstring>::iterator ite=extract_file_list.begin(),
				end=extract_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callExtract,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);

				::CloseHandle(m_arc_thread);

				switch(result){
					case ARC_SUCCESS:
						if(!isTerminated()){
							if(CFG.general.remove_source!=RMSRC_DISABLE){
								if(!fileoperation::removeSplitFile(ite->c_str(),CFG.general.remove_source==RMSRC_RECYCLEBIN)){
									dprintf(_T("removeFile(%s)\n"),ite->c_str());
									removeFile(ite->c_str());
								}
							}
						}
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
			}
			if(m_pUninstallHook){
				m_pUninstallHook();
			}
			break;
		}

		case MODE_LIST:{
			//パスワードダイアログのフックを開始
			if(m_pInstallHook){
				if(!m_pInstallHook(::GetCurrentProcessId(),m_dialog_hook_thread.id,WM_HOOKDIALOG)){
					errmsg(_T("フック用ライブラリの設定に失敗しました。\n"));
				}
			}
			std::list<tstring> list_file_list;

			if(!fullPathList(list_file_list,filepaths)){
				errmsg(_T("そのようなファイルは存在しません。\n"));
				return false;
			}
			info(_T("\n"));

			for(std::list<tstring>::iterator ite=list_file_list.begin(),
				end=list_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callList,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);

				::CloseHandle(m_arc_thread);

				switch(result){
					case ARC_SUCCESS:
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
			}
			if(m_pUninstallHook){
				m_pUninstallHook();
			}
			break;
		}

		case MODE_TEST:{
			//パスワードダイアログのフックを開始
			if(m_pInstallHook){
				if(!m_pInstallHook(::GetCurrentProcessId(),m_dialog_hook_thread.id,WM_HOOKDIALOG)){
					errmsg(_T("フック用ライブラリの設定に失敗しました。\n"));
				}
			}
			std::list<tstring> test_file_list;

			if(!fullPathList(test_file_list,filepaths)){
				errmsg(_T("そのようなファイルは存在しません。\n"));
				return false;
			}
			info(_T("\n"));

			for(std::list<tstring>::iterator ite=test_file_list.begin(),
				end=test_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callTest,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);

				::CloseHandle(m_arc_thread);

				switch(result){
					case ARC_SUCCESS:
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
			}
			if(m_pUninstallHook){
				m_pUninstallHook();
			}
			break;
		}

		case MODE_SENDCOMMANDS:{
			if(cmd_arg.filepaths().empty()){
				errmsg(_T("有効なコマンドが存在しません。\n"));
				return false;
			}

			ARC_RESULT result=ARC_FAILURE;
			tstring err_msg;
			std::list<tstring> arg_list(cmd_arg.filepaths().begin(),cmd_arg.filepaths().end());
			ARC_RESULT_MSG result_msg(result,err_msg);
			ARC_THREAD_PARAM thread_param(this,arg_list,result_msg);

			m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callSendCommands,&thread_param,0,NULL);

			::WaitForSingleObject(m_arc_thread,INFINITE);

			::CloseHandle(m_arc_thread);

			switch(result){
				case ARC_SUCCESS:
					break;

				default:
					errmsg(err_msg.c_str());
					setExitCode(EXIT_FAILURE);
					break;
			}
			break;
		}

		case MODE_VERSION:{
			if(cmd_arg.filepaths().empty()){
				//対応するライブラリ全てのバージョンを表示
				for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
					if(m_arcdll_list[i]!=NULL){
#if 0
							STDOUT.outputString(_T("%-12s %s\n"),
												  m_arcdll_list[i]->name().c_str(),
												  getVersion(m_arcdll_list[i]->name().c_str()).c_str());
#else
							STDOUT.outputString(_T("%-12s %s\n"),
												  m_arcdll_list[i]->name().c_str(),
												  m_arcdll_list[i]->getVersionStr().c_str());
#endif
					}
				}

				if(m_b2e_dll){
#if 0
						STDOUT.outputString(_T("%-12s %s\n"),
											  m_b2e_dll->name().c_str(),
											  getVersion(m_b2e_dll->name().c_str()).c_str());
#else
						STDOUT.outputString(_T("%-12s %s\n"),
											  m_b2e_dll->name().c_str(),
											  m_b2e_dll->getVersionStr().c_str());
#endif
				}

				for(size_t i=0,list_size=m_spi_list.size();i<list_size;i++){
					if(m_spi_list[i]!=NULL){
#if 0
							STDOUT.outputString(_T("%-12s %s\n"),
												  m_spi_list[i]->name().c_str(),
												  getVersion(m_spi_list[i]->name().c_str()).c_str());
#else
							STDOUT.outputString(_T("%s\n"),
												  m_spi_list[i]->getInformation().c_str());
#endif
					}
				}

				for(size_t i=0,list_size=m_wcx_list.size();i<list_size;i++){
					if(m_wcx_list[i]!=NULL){
							STDOUT.outputString(_T("%-12s %s\n"),
												  m_wcx_list[i]->name().c_str(),
												  getVersion(m_wcx_list[i]->name().c_str()).c_str());
					}
				}
			}else{
				for(std::vector<tstring>::size_type i=0,size=filepaths.size();!isTerminated()&&i<size;++i){
						STDOUT.outputString(_T("%-12s %s\n"),
											  filepaths[i].c_str(),
											  getVersion(filepaths[i].c_str()).c_str());
				}
			}
			break;
		}

		case MODE_SETTINGS:{
			ARC_RESULT result=ARC_FAILURE;
			tstring err_msg;
			ARC_RESULT_MSG result_msg(result,err_msg);
			ARC_THREAD_PARAM thread_param(this,_T(""),result_msg);

			m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callSettings,&thread_param,0,NULL);

			::WaitForSingleObject(m_arc_thread,INFINITE);

			::CloseHandle(m_arc_thread);

			switch(result){
				case ARC_SUCCESS:
					break;

				default:
					errmsg(err_msg.c_str());
					setExitCode(EXIT_FAILURE);
					break;
			}
			break;
		}
	}

	if(!CFG.general.quit){
		system("pause");
	}

	return true;
}
