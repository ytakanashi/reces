//reces.cpp
//recesメイン

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"reces.h"
#include"recesBase.h"
#include"ParseOptions.h"
#include"Compress.h"
#include"Extract.h"
#include"List.h"
#include"SendCommands.h"
#include"Test.h"
#include"Settings.h"
#include"Hook/HookArchiverDialog.h"
#include"resources/resource.h"


using namespace sslib;


namespace{
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
	long long done=0;
	long long total=0;
	void startmsg(const TCHAR* file_path){
		if(total>1)info(_T("[%I64d / %I64d] '%s'\n"),done++,total,file_path);
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

	m_hook_dll_path=
		path::addTailSlash(path::getTempDirPath())+
			_T("rcsDlgHook")+
#ifndef _WIN64
			_T("32")+
#else
			_T("64")+
#endif
			SOFTWARE_VERSION+
			_T(".tmp");

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
	dprintf(_T("cleanup\n"));

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
		::SendMessageTimeout(HWND_BROADCAST,WM_NULL,0,0,SMTO_ABORTIFHUNG,300,NULL);

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
						  _T("\t\t\t  libraryで解凍用ライブラリ指定可能\n")
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
						  _T("\t/eb[[level]|x]\t #共通パスを除外して解凍 {mr/me}\n")
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
						  _T("\t/DIRTS\t\t #ディレクトリのタイムスタンプを復元する {mr/me}\n")
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
						  _T("\t/UESC\t\t #Unicodeエスケープシーケンスをデコードする {mr/me/ml}\n")
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
		misc::Lock lock_arc(m_arc_cs);
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
	{
		misc::Lock lock(m_arc_cs);
		SAFE_CLOSE(m_arc_thread);
	}

	if(!STDOUT.isRedirected()&&
	   !CFG.no_display.no_information){
		misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
	}

	//コールバック関数の登録を解除
	if(m_arc_dll)m_arc_dll->clearCallback();

	STDOUT.outputString(_T("\n処理が中断されました。\n"));
	cleanup();
	done=true;
}

tstring Reces::getVersion(const TCHAR* file_path){
	DWORD major_ver=0;
	DWORD minor_ver=0;

	if(fileoperation::getFileVersion(file_path,&major_ver,&minor_ver)){
		return format(_T("%d.%02d.%02d.%02d"),
								major_ver>>16,
								major_ver&0xffff,
								minor_ver>>16,
								minor_ver&0xffff);
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

//*.spiを検索、リストに追加
bool Reces::searchSpi(const TCHAR* search_dir){
	FileSearch fs;

	std::vector<TCHAR> full_path(MAX_PATH);

	if(path::getFullPath(&full_path[0],full_path.size(),search_dir)&&
	   path::fileExists(&full_path[0])){
#ifndef _WIN64
		for(fs.first(&full_path[0],_T("*.spi"));fs.next();){
#else
		for(fs.first(&full_path[0],_T("*.sph"));fs.next();){
#endif
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
		for(fs.first(&full_path[0],_T("*.wcx;*.wcx64"));fs.next();){
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
				ARCCFG->m_hook_dialog_type=static_cast<int>(msg.lParam);

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
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		Compress* compress=new Compress();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			compress->operator()(((ARC_THREAD_PARAM*)param)->file_list,
								 ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(compress);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callExtract(void* param){
	{
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		Extract* extract=new Extract();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			extract->operator()(((ARC_THREAD_PARAM*)param)->file_name,
								((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(extract);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callList(void* param){
	{
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		List* list=new List();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			list->operator()(((ARC_THREAD_PARAM*)param)->file_name,
							 ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(list);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callSendCommands(void* param){
	{
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		SendCommands* send_commands=new SendCommands();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			send_commands->operator()(((ARC_THREAD_PARAM*)param)->file_list,
									  ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(send_commands);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callTest(void* param){
	{
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		Test* test=new Test();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			test->operator()(((ARC_THREAD_PARAM*)param)->file_name,
							 ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(test);
	}
	_endthreadex(0);
	return 0;
}

unsigned __stdcall Reces::callSettings(void* param){
	{
		misc::Lock lock(((ARC_THREAD_PARAM*)param)->this_ptr->m_arc_cs);
		Settings* settings=new Settings();
		((ARC_THREAD_PARAM*)param)->result_msg.result=
			settings->operator()(((ARC_THREAD_PARAM*)param)->this_ptr->wnd(),
								 ((ARC_THREAD_PARAM*)param)->result_msg.err_msg);
		SAFE_DELETE(settings);
	}
	_endthreadex(0);
	return 0;
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

	done=total=1;

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
			int env_index=0;

			total=extract_file_list.size();

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

				startmsg(ite_list->c_str());

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callExtract,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);
				{
					misc::Lock lock(m_arc_cs);
					SAFE_CLOSE(m_arc_thread);
				}
				if(isTerminated())break;

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
							if(!CFG.recompress.run_command.disable()){
								if(ite_list==list_begin){
									//環境変数FILEPATHに入力ファイルパスを代入
									env::set(_T("FILEPATH"),ite_list->c_str());
								}

								//環境変数FILEPATH[env_index]に入力ファイルパスを代入
								env::set(format(_T("FILEPATH[%d]"),env_index++).c_str(),ite_list->c_str());
							}
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
						if(!CFG.recompress.run_command.disable()){
							if(CFG.compress.each_file){
								//環境変数FILEPATHに入力ファイルパスを代入
								env::set(_T("FILEPATH"),ite_list->c_str());
							}
							env::set(_T("FILECOUNT"),format(_T("%d"),(CFG.compress.each_file)?1:extract_file_list.size()).c_str());
							runCommand();
						}

						if(isTerminated())break;

						::SetCurrentDirectory(ARCCFG->m_recmp_temp_dir.c_str());

						std::list<tstring> dummy_list;
						ARC_RESULT result=ARC_FAILURE;
						tstring err_msg;
						ARC_RESULT_MSG result_msg(result,err_msg);
						ARC_THREAD_PARAM thread_param(this,dummy_list,result_msg);

						m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callCompress,&thread_param,0,NULL);

						::WaitForSingleObject(m_arc_thread,INFINITE);
						{
							misc::Lock lock(m_arc_cs);
							SAFE_CLOSE(m_arc_thread);
						}
						if(isTerminated())break;

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
				info(_T("\n"));
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

			if(CFG.compress.each_file)total=compress_file_list.size();

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

				startmsg(compress_file_list.begin()->c_str());

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callCompress,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);
				{
					misc::Lock lock(m_arc_cs);
					SAFE_CLOSE(m_arc_thread);
				}
				if(isTerminated())break;

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
				info(_T("\n"));
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

			total=extract_file_list.size();

			for(std::list<tstring>::iterator ite=extract_file_list.begin(),
				end=extract_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				startmsg(ite->c_str());

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callExtract,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);
				{
					misc::Lock lock(m_arc_cs);
					SAFE_CLOSE(m_arc_thread);
				}
				if(isTerminated())break;

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
				info(_T("\n"));
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

			total=list_file_list.size();

			for(std::list<tstring>::iterator ite=list_file_list.begin(),
				end=list_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				startmsg(ite->c_str());

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callList,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);
				{
					misc::Lock lock(m_arc_cs);
					SAFE_CLOSE(m_arc_thread);
				}
				if(isTerminated())break;

				switch(result){
					case ARC_SUCCESS:
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
				info(_T("\n"));
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

			total=test_file_list.size();

			for(std::list<tstring>::iterator ite=test_file_list.begin(),
				end=test_file_list.end();
				ite!=end&&!isTerminated();
				++ite){

				ARCCFG->m_password_input_cancelled=false;
				ARC_RESULT result=ARC_FAILURE;
				tstring err_msg;
				ARC_RESULT_MSG result_msg(result,err_msg);
				ARC_THREAD_PARAM thread_param(this,*ite,result_msg);

				startmsg(ite->c_str());

				m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::callTest,&thread_param,0,NULL);

				::WaitForSingleObject(m_arc_thread,INFINITE);
				{
					misc::Lock lock(m_arc_cs);
					SAFE_CLOSE(m_arc_thread);
				}
				if(isTerminated())break;

				switch(result){
					case ARC_SUCCESS:
						break;

					default:
						errmsg(err_msg.c_str());
						setExitCode(EXIT_FAILURE);
						break;
				}
				info(_T("\n"));
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
			{
				misc::Lock lock(m_arc_cs);
				SAFE_CLOSE(m_arc_thread);
			}
			if(isTerminated())break;

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

#ifndef _WIN64
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
#endif

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
			{
				misc::Lock lock(m_arc_cs);
				SAFE_CLOSE(m_arc_thread);
			}
			if(isTerminated())break;

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
