//ParseOptions.cpp
//オプション解析

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ParseOptions.h"
#include"recesBase.h"
#include<shlobj.h>


using namespace sslib;


bool parseOptions(CommandArgument& cmd_arg){
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
		static void removeDllExt(tstring* libname){
			//dllなら拡張子を取り除く
			if(str::toLower(path::getExtension(*libname))==_T("dll")){
				*libname=path::removeExtension(*libname);
			}
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
				msg::err(_T("cfgファイルの読み込みに失敗しました。\n"));
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
						//ディレクトリ階層を無視して再圧縮
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'r':{
						//再圧縮
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
						//ディレクトリ階層を無視して圧縮
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'c':{
						//圧縮
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
						//ディレクトリ階層を無視して解凍
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'e':{
						//解凍
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
							CFG.extract.directory_timestamp=true;
							dprintf(_T("CFG.extract.directory_timestamp=%d\n"),CFG.extract.directory_timestamp);
						}
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
						for(pp=p+1;*pp&&*pp!=':';pp++);

						switch(*(p++)){
							case 'r':{
								tstring str(p,pp-p);

								CFG.general.filefilter.regex=true;
								dprintf(_T("CFG.general.filefilter.regex=%d\n"),CFG.general.filefilter.regex);
								//';'で分割
								str::splitString(&CFG.general.filefilter.pattern_list,str.c_str(),';');
								//重複を削除
								l::undupList(&CFG.general.filefilter.pattern_list);
								for(std::list<tstring>::iterator ite=CFG.general.filefilter.pattern_list.begin(),
									end=CFG.general.filefilter.pattern_list.end();
									ite!=end;
									++ite){dprintf(_T("%s\n"),ite->c_str());}
								break;
							}

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
									msg::err(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
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
						for(pp=p+1;*pp&&*pp!=':';pp++);

						switch(*(p++)){
							case 'r':{
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.regex=true;
								dprintf(_T("CFG.general.file_ex_filter.regex=%d\n"),CFG.general.file_ex_filter.regex);
								//';'で分割
								str::splitString(&CFG.general.file_ex_filter.pattern_list,str.c_str(),';');
								//重複を削除
								l::undupList(&CFG.general.file_ex_filter.pattern_list);
								break;
							}

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
									msg::err(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
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

			case 'U':{
				//Unicodeエスケープシーケンスをデコードする
				if(options[i].find(_T("UESC"))==0){
					CFG.general.decode_uesc=true;
					dprintf(_T("CFG.general.decode_uesc=%d\n"),CFG.general.decode_uesc);
				}
				break;
			}//case 'U'

			case 'q':{
				//処理終了後閉じる
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

			case '@':{
				//リストファイル
				File list_file;

				if(!list_file.open(options[i].c_str()+1,OPEN_EXISTING,GENERIC_READ,0,CFG.general.codepage)){
					msg::err(_T("リストファイル '%s' を開くことが出来ませんでした。"),options[i].c_str()+1);
				}else{
					std::vector<tstring> file_list;

					list_file.readList(&file_list,File::RL_SKIP_BOM|File::RL_REMOVE_QUOTES);
					//取得したパスたちを処理対象リストに追加
					cmd_arg.filepaths().insert(cmd_arg.filepaths().end(),file_list.begin(),file_list.end());
				}
				break;
			}

			case 'w':{
				//wait指定
				if(options[i].find(_T("wait:"))==0&&
				   options[i][5]!='\0'){
					ARCCFG->m_wait=_ttoi(options[i].substr(5).c_str());
				}
			}

			default:
				break;
		}
	}

	//dllなら拡張子を取り除く
	l::removeDllExt(&CFG.general.selected_library_name);

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
