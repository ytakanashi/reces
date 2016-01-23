//ParseOptions.cpp
//オプション解析

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ParseOptions.h"
#include"recesBase.h"
#include<shlobj.h>
#include<mlang.h>


using namespace sslib;


namespace{
	bool readFileList(std::list<tstring>* list,File* list_file){
		if(!list||!list_file)return false;
		std::vector<tstring> file_list;

		list_file->readList(&file_list,File::RL_SKIP_BOM|File::RL_REMOVE_QUOTES);
		//取得したパスたちを処理対象リストに追加
		list->insert(list->end(),file_list.begin(),file_list.end());
		misc::undupList(list);

		//末尾の区切り文字を削除
		for(std::list<tstring>::iterator ite=list->begin(),
			end=list->end();
			ite!=end;
			++ite){
			*ite=path::removeTailSlash(*ite);
		}

		return true;
	}

	void removeDllExt(tstring* libname){
		//dllなら拡張子を取り除く
		if(str::toLower(path::getExtension(*libname))==_T("dll")){
			*libname=path::removeExtension(*libname);
		}
	}

	void parseB2eOptions(tstring& option){
		//圧縮形式
		CFG.compress.b2e.format=option.substr(lstrlen((option.find(_T("b2e:"))==2)?_T("b2e:"):_T("b2esfx:"))+2);
		tstring::size_type pos=CFG.compress.b2e.format.rfind(_T(":"));

		if(pos!=tstring::npos){
			//圧縮メソッド
			CFG.compress.b2e.method=CFG.compress.b2e.format.substr(pos+1);
			//圧縮形式
			CFG.compress.b2e.format=CFG.compress.b2e.format.substr(0,pos);
		}

		if(option.find(_T("b2esfx:"))==2)CFG.compress.b2e.sfx=true;

		if(CFG.compress.b2e.format.c_str()[0]=='@'){
			//b2e:@指定
			CFG.compress.compression_type=CFG.compress.b2e.format;
			CFG.compress.b2e.format.clear();
		}

		dprintf(_T("b2e format: %s\n"),CFG.compress.b2e.format.c_str());
		if(!CFG.compress.b2e.method.empty()){
			dprintf(_T("b2e method: %s\n"),CFG.compress.b2e.method.c_str());
		}
		dprintf(_T("b2e sfx: %d\n"),CFG.compress.b2e.sfx);
	}
}

bool parseOptions(CommandArgument& cmd_arg){
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
				//'{'
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
				//'}'
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
						//'mR'
						//ディレクトリ階層を無視して再圧縮
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'r':{
						//'mr'
						//再圧縮
						CFG.mode=MODE_RECOMPRESS;
						dprintf(_T("mode : recompress\n"));

						if(options[i].c_str()[2]!='\0'){
							if(options[i].find(_T("b2e:"))==2||options[i].find(_T("b2esfx:"))==2){
								//b2eスクリプト
								parseB2eOptions(options[i]);
							}else{
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
						}
						break;
					}

					case 'C':
						//'mC'
						//ディレクトリ階層を無視して圧縮
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'c':{
						//'mc'
						//圧縮
						CFG.mode=MODE_COMPRESS;
						dprintf(_T("mode : compress\n"));

						if(options[i].c_str()[2]=='\0'){
							//圧縮形式が指定されていない場合
						}else{
							if(options[i].find(_T("b2e:"))==2||options[i].find(_T("b2esfx:"))==2){
								//b2eスクリプト
								parseB2eOptions(options[i]);
							}else{
								//圧縮形式が指定された場合
								CFG.compress.compression_type=options[i].substr(2);

								dprintf(_T("compress mode: %s\n"),CFG.compress.compression_type.c_str());
							}
						}
						break;
					}

					case 'E':
						//'mE'
						//ディレクトリ階層を無視して解凍
						CFG.general.ignore_directory_structures=true;
						dprintf(_T("CFG.general.ignore_directory_structures\n"));
						//fall through
					case 'e':{
						//'me'
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
						//'mL'
						//FindFirst()/FindNext()/GetFileName()で出力しない
						CFG.output_file_list.api_mode=false;
						//fall through
					case 'l':
						//'ml'
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
						//'mt'
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

					case 'd':{
						//'md'
						//削除
						CFG.mode=MODE_DELETE;
						dprintf(_T("mode : delete\n"));

						if(options[i].c_str()[2]!='\0'){
							//ライブラリ名を直接指定した場合
							CFG.general.selected_library_name=options[i].substr(2);
							dprintf(_T("select library: %s\n"),CFG.general.selected_library_name.c_str());
						}
						break;
					}

					case 'N':
						CFG.rename.regex=true;
						dprintf(_T("CFG.rename.regex\n"));
					case 'n':{
						//'mn'
						//リネーム
						CFG.mode=MODE_RENAME;
						dprintf(_T("mode : rename\n"));

						if(options[i].c_str()[2]!='\0'){
							std::list<tstring> pattern_list;

							//';'で分割
							str::splitString(&pattern_list,options[i].substr(2).c_str(),';');
							//重複を削除
							misc::undupList(&pattern_list);

							for(std::list<tstring>::const_iterator ite=pattern_list.begin(),
								end=pattern_list.end();
								ite!=end;++ite){
								std::list<tstring> pattern;

								str::splitString(&pattern,ite->c_str(),':');
								if(pattern.front().empty())continue;
								if(pattern.size()<2)pattern.push_back(_T(""));
								CFG.rename.pattern_list.push_back(RENAME::pattern(pattern.front(),pattern.back()));
							}
						}
						break;
					}

					case 's':
					case 'S':{
						if(options[i].c_str()[1]=='s'){
							//'ms'
							//ライブラリ直接操作
							CFG.mode=MODE_SENDCOMMANDS;
							dprintf(_T("mode : send_commands\n"));
						}else{
							//'mS'
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
						//'mv'
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
							//'ni'
							//進捗状況
							CFG.no_display.no_information=true;
							dprintf(_T("CFG.no_display.no_information=true\n"));
							break;

						case 'l':
							//'nl'
							//ログ
							CFG.no_display.no_log=true;
							dprintf(_T("CFG.no_display.no_log=true\n"));
							break;

						case 'p':
							//'np'
							//パスワード
							CFG.no_display.no_password=true;
							dprintf(_T("CFG.no_display.no_password=true\n"));
							break;

						case 'e':
							//'ne'
							//エラーメッセージ
							CFG.no_display.no_errmsg=true;
							dprintf(_T("CFG.no_display.no_errmsg=true\n"));
							break;

						case 'a':
							//'na'
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
				//'c'
				//ディレクトリを作成する
				CFG.extract.create_dir=true;
				dprintf(_T("CFG.extract.create_dir=true\n"));

				const TCHAR*p=options[i].c_str()+1;
				while(*p){
					switch(*p){
						//不要なディレクトリを作成しない
						case '1':
							//'c1'
							//ファイル単体の場合作成しない
							CFG.extract.create_dir_optimization.remove_redundant_dir.only_file=true;
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.only_file=true\n"));
							break;
						case '2':
							//'c2'
							//二重ディレクトリを作成しない
							CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=true;
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=true\n"));
							break;
						case '3':
							//'c3'
							//同名の二重ディレクトリを作成しない
							CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=true;
							CFG.extract.create_dir_optimization.remove_redundant_dir.same_dir=true;
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=true\n"));
							dprintf(_T("CFG.extract.create_dir_optimization.remove_redundant_dir.same_dir=true\n"));
							break;

						//ディレクトリ名末尾の数字や記号を削除
						case 'n':
							//'cn'
							//ディレクトリ名末尾の数字を削除
							CFG.extract.create_dir_optimization.omit_number_and_symbol.number=true;
							dprintf(_T("CFG.extract.create_dir_optimization.omit_number_and_symbol.bNumber=true\n"));
							break;

						case 's':
							//'cs'
							//ディレクトリ名末尾の記号を削除
							CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol=true;
							dprintf(_T("CFG.extract.create_dir_optimization.omit_number_and_symbol.bSymbol=true\n"));
							break;

						case 't':
							//'ct'
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
						//'d2'
						CFG.general.remove_source=RMSRC_REMOVE;
						dprintf(_T("CFG.general.remove_source=RMSRC_REMOVE\n"));
						break;

					case '1':
					case '\0':
					default:
						//'d1'
						//'d'
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
							//'ebx'
							//[解凍]共通パスをすべて除く
							CFG.compress.exclude_base_dir=-1;
							break;

						case '\0':
							//'eb'
							CFG.compress.exclude_base_dir=1;
							break;

						default:
							//'eb'
							//圧縮時は正負のみ確認
							CFG.compress.exclude_base_dir=_ttoi(options[i].substr(2).c_str());
							break;
					}
					if(CFG.compress.exclude_base_dir<-1)CFG.compress.exclude_base_dir=-1;
					dprintf(_T("CFG.compress.exclude_base_dir=%d\n"),CFG.compress.exclude_base_dir);
				}else{
					//'e'
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
						//'Db'
						//b2eスクリプトのあるディレクトリを指定/
						CFG.general.b2e_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.b2e_dir=%s\n"),CFG.general.b2e_dir.c_str());
						break;

					case 's':
						//'Ds'
						//spiのあるディレクトリを指定
						CFG.general.spi_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.spi_dir=%s\n"),CFG.general.spi_dir.c_str());
						break;

					case 'w':
						//'Dw'
						//wcxのあるディレクトリを指定
						CFG.general.wcx_dir=removeTailCharacter(options[i].substr(2),'\"');
						dprintf(_T("CFG.general.wcx_dir=%s\n"),CFG.general.wcx_dir.c_str());
						break;

					case 'I':
						//'DIRTS'
						//ディレクトリのタイムスタンプを復元する
						if(options[i].find(_T("DIRTS"))==0){
							CFG.extract.directory_timestamp=true;
							dprintf(_T("CFG.extract.directory_timestamp=%d\n"),CFG.extract.directory_timestamp);
						}
						break;
				}
				break;
			}

			case 'N':{
				//新規作成
				switch(options[i].c_str()[1]){
					case 'F':
					//'NF'
					//書庫強制新規作成
					CFG.compress.force_create_new=true;
					dprintf(_T("CFG.compress.force_create_new=%d\n"),CFG.compress.force_create_new);
						break;

					case '\0':
					default:
						//'N'
						//書庫新規作成
						CFG.compress.create_new=true;
						dprintf(_T("CFG.compress.create_new=%d\n"),CFG.compress.create_new);
						break;
				}
				break;
			}

			case 'I':
				//'I'
				CFG.general.filefilter.recursive=true;
				dprintf(_T("CFG.general.filefilter.recursive\n"));
				//fall through
			case 'i':{
				//'i'
				//処理対象フィルタ
				if(options[i].length()<2)break;

				if(options[i].c_str()[1]!=':'){
					//';'で分割
					str::splitString(&CFG.general.filefilter.pattern_list,options[i].substr(1).c_str(),';');
					//重複を削除
					misc::undupList(&CFG.general.filefilter.pattern_list);
					//末尾の区切り文字を削除
					for(std::list<tstring>::iterator ite=CFG.general.filefilter.pattern_list.begin(),
						end=CFG.general.filefilter.pattern_list.end();
						ite!=end;
						++ite){
						*ite=path::removeTailSlash(*ite);
					}
				}else{
					const TCHAR*p=options[i].c_str()+2,*pp;

					while(*p){
						for(pp=p+1;*pp&&*pp!=':';pp++);

						switch(*(p++)){
							case 'r':{
								//'i:r'
								tstring str(p,pp-p);

								CFG.general.filefilter.regex=true;
								dprintf(_T("CFG.general.filefilter.regex=%d\n"),CFG.general.filefilter.regex);
								//';'で分割
								str::splitString(&CFG.general.filefilter.pattern_list,str.c_str(),';');
								//重複を削除
								misc::undupList(&CFG.general.filefilter.pattern_list);
								//末尾の区切り文字を削除
								for(std::list<tstring>::iterator ite=CFG.general.filefilter.pattern_list.begin(),
									end=CFG.general.filefilter.pattern_list.end();
									ite!=end;
									++ite){
									*ite=path::removeTailSlash(*ite);
								}

								for(std::list<tstring>::iterator ite=CFG.general.filefilter.pattern_list.begin(),
									end=CFG.general.filefilter.pattern_list.end();
									ite!=end;
									++ite){dprintf(_T("%s\n"),ite->c_str());}
								break;
							}

							case 's':{
								//'i:s'
								tstring str(p,pp-p);

								CFG.general.filefilter.min_size=strex::filesize2longlong(str.c_str());
								dprintf(_T("min_size=%I64d\n"),CFG.general.filefilter.min_size);
								break;
							}

							case 'S':{
								//'i:S'
								tstring str(p,pp-p);

								CFG.general.filefilter.max_size=strex::filesize2longlong(str.c_str());
								dprintf(_T("max_size=%I64d\n"),CFG.general.filefilter.max_size);
								break;
							}

							case 'a':
							case 'A':
								//'i:a'
								while(p!=pp){
									if(*p=='e'||*p=='E'){
										//「空ディレクトリのみ格納」は実装しない方向で
									}else{
										CFG.general.filefilter.attr|=strex::attr2DWORD(*p);
									}
									++p;
								}
								break;

							case 'd':{
								//'i:d'
								tstring str(p,pp-p);

								CFG.general.filefilter.oldest_date=strex::datetime2longlong(str.c_str());
								break;
							}

							case 'D':{
								//'i:D'
								tstring str(p,pp-p);

								CFG.general.filefilter.newest_date=strex::datetime2longlong(str.c_str(),true);
								break;
							}

							case '@':{
								//'i:@'
								tstring file(p,pp-p);
								File list_file;

								if(!list_file.open(file.c_str(),OPEN_EXISTING,GENERIC_READ,0,CFG.general.list_codepage)){
									msg::err(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
								}else{
									readFileList(&CFG.general.filefilter.pattern_list,&list_file);
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
				//'X'
				CFG.general.file_ex_filter.recursive=true;
				dprintf(_T("CFG.general.file_ex_filter.recursive\n"));
				//fall through
			case 'x':{
				//'x'
				//処理対象除外フィルタ
				if(options[i].length()<2)break;

				if(options[i].c_str()[1]!=':'){
					//';'で分割
					str::splitString(&CFG.general.file_ex_filter.pattern_list,options[i].substr(1).c_str(),';');
					//重複を削除
					misc::undupList(&CFG.general.file_ex_filter.pattern_list);
					//末尾の区切り文字を削除
					for(std::list<tstring>::iterator ite=CFG.general.file_ex_filter.pattern_list.begin(),
						end=CFG.general.file_ex_filter.pattern_list.end();
						ite!=end;
						++ite){
						*ite=path::removeTailSlash(*ite);
					}
				}else{
					const TCHAR*p=options[i].c_str()+2,*pp;

					while(*p){
						for(pp=p+1;*pp&&*pp!=':';pp++);

						switch(*(p++)){
							case 'r':{
								//'x:r'
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.regex=true;
								dprintf(_T("CFG.general.file_ex_filter.regex=%d\n"),CFG.general.file_ex_filter.regex);
								//';'で分割
								str::splitString(&CFG.general.file_ex_filter.pattern_list,str.c_str(),';');
								//重複を削除
								misc::undupList(&CFG.general.file_ex_filter.pattern_list);
								//末尾の区切り文字を削除
								for(std::list<tstring>::iterator ite=CFG.general.file_ex_filter.pattern_list.begin(),
									end=CFG.general.file_ex_filter.pattern_list.end();
									ite!=end;
									++ite){
									*ite=path::removeTailSlash(*ite);
								}
								break;
							}

							case 's':{
								//'x:s'
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.min_size=strex::filesize2longlong(str.c_str());
								dprintf(_T("min_size=%I64d\n"),CFG.general.file_ex_filter.min_size);
								break;
							}

							case 'S':{
								//x:S'
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.max_size=strex::filesize2longlong(str.c_str());
								dprintf(_T("max_size=%I64d\n"),CFG.general.file_ex_filter.max_size);
								break;
							}

							case 'a':
							case 'A':
								//'x:a'
								while(p!=pp){
									if(*p=='e'||*p=='E'){
										//include_empty_dirは除外フィルタの場合のみ操作
										CFG.general.file_ex_filter.include_empty_dir=false;
										dprintf(_T("CFG.general.file_ex_filter.include_empty_dir=false\n"));
									}else{
										CFG.general.file_ex_filter.attr|=strex::attr2DWORD(*p);
									}
									++p;
								}
								break;

							case 'd':{
								//'x:d'
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.oldest_date=strex::datetime2longlong(str.c_str());
								break;
							}

							case 'D':{
								//'x:D'
								tstring str(p,pp-p);

								CFG.general.file_ex_filter.newest_date=strex::datetime2longlong(str.c_str(),true);
								break;
							}

							case '@':{
								//'x:@'
								tstring file(p,pp-p);
								File list_file;

								if(!list_file.open(file.c_str(),OPEN_EXISTING,GENERIC_READ,0,CFG.general.list_codepage)){
									msg::err(_T("リストファイル '%s' を開くことが出来ませんでした。"),file.c_str());
								}else{
									readFileList(&CFG.general.file_ex_filter.pattern_list,&list_file);
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
					//'l'
					//'/l'と指定された場合、圧縮率を最小にする
					CFG.compress.compression_level=Archiver::minimum_compressionlevel;
				}else if(options[i].c_str()[1]=='x'){
					//'lx'
					//'/lx'と指定された場合、圧縮率を最大にする
					CFG.compress.compression_level=Archiver::maximum_compressionlevel;
				}else{
					//'l'
					CFG.compress.compression_level=options[i].c_str()[1]-'0';
					//値が範囲内かどうかはcompress()にて判断
				}
				dprintf(_T("CFG.compress.compression_level=%d\n"),CFG.compress.compression_level);
				break;
			}

			case 's':{
				//'s'
				//分割圧縮
				CFG.compress.split_value=options[i].substr(1);
				dprintf(_T("CFG.compress.split_value=%s\n"),CFG.compress.split_value.c_str());
				break;
			}

			case 'o':{
				//出力先
				switch(options[i].c_str()[1]){
					case 'd':
						//'od'
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
						//'oF'
						CFG.compress.raw_file_name=true;
						dprintf(_T("CFG.compress.raw_file_name\n"));
						//fall through
					case 'f':
						//'of'
						//ファイル
						CFG.compress.output_file=options[i].substr(2);
						dprintf(_T("CFG.compress.output_file=%s\n"),CFG.compress.output_file.c_str());
						break;

					case 'o':
						//設定
						if(options[i].c_str()[2]=='b'){
							//'oob'
							//カレントディレクトリ基準で'/od','/of'の相対パスを処理
							CFG.general.default_base_dir=true;
							dprintf(_T("CFG.general.default_base_dir=true\n"));
						}else if(options[i].substr(2)==_T("ke")){
							//'ooke'
							//元ファイルの拡張子を保持
							CFG.compress.keep_extension=true;
							dprintf(_T("CFG.compress.keep_extension=true\n"));
						}else if(options[i].c_str()[2]=='r'){
							//'oor'
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
						//'pw'
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
						//'pn'
						//再圧縮する書庫のパスワードを新しく設定
						CFG.recompress.new_password=options[i].substr(2);
						if(!options[i].substr(2).empty()){
							CFG.general.password_list.push_back(options[i].substr(2));
						}
						dprintf(_T("NewPassword: %s\n"),CFG.recompress.new_password.c_str());
						break;

					case 'f':{
						//'pf'
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
				//'P'
				//ユーザ独自のパラメータ
				CFG.general.custom_param=options[i].substr(1);
				dprintf(_T("CFG.general.custom_param=%s\n"),CFG.general.custom_param.c_str());
				break;
			}

			case 't':{
				//'t'
				//更新日時を元書庫と同じにする
				CFG.compress.copy_timestamp=true;
				dprintf(_T("CFG.compress.copy_timestamp=true\n"));
				break;
			}

			case 'r':{
				//'r'
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

			case 'R':{
				//'R'
				//対象ディレクトリを再帰的検索
				CFG.compress.recursive=true;
				dprintf(_T("CFG.compress.recursive\n"));
				break;
			}

			case 'b':{
				//'b'
				//バックグラウンドで動作
				CFG.general.background_mode=true;
				dprintf(_T("CFG.general.background_mode=true\n"));
				break;
			}

			case 'C':{
				if(options[i].find(_T("C:@"))==0){
					//'C:@'
					//パスワードリストファイル/ファイルリストの文字コード
					tstring codepage_str(options[i].substr(lstrlen(_T("C:@"))).c_str());

					//小文字へ変換
					transform(codepage_str.begin(),codepage_str.end(),codepage_str.begin(),tolower);

					if(codepage_str==_T("sjis")||codepage_str==_T("s-jis")||codepage_str==_T("s_jis")||codepage_str==_T("shiftjis")){
						CFG.general.list_codepage=File::SJIS;
					}else if(codepage_str==_T("utf8")||codepage_str==_T("utf-8")||codepage_str==_T("utf_8")){
						CFG.general.list_codepage=File::UTF8;
					}else if(codepage_str==_T("utf16")||codepage_str==_T("utf-16")||codepage_str==_T("utf_16")||
							 codepage_str==_T("utf16le")||codepage_str==_T("utf-16le")||codepage_str==_T("utf_16le")||
							 codepage_str==_T("unicode")){
						CFG.general.list_codepage=File::UTF16LE;
					}else if(codepage_str==_T("utf16be")||codepage_str==_T("utf-16be")||codepage_str==_T("utf_16be")){
						CFG.general.list_codepage=File::UTF16BE;
					}
				}else if(options[i].find(_T("C:oa"))==0){
					//'C:oa'
					CFG.general.ansi_stdout=true;
					dprintf(_T("CFG.general.ansi_stdout=%d\n"),CFG.general.ansi_stdout);
				}else if(options[i].c_str()[1]!='\0'){
					//'C'
					//アーカイバに送る文字コード(現在7-zip32.dllのみ対象)
					if(isdigit(options[i].c_str()[1])){
						CFG.general.arc_codepage=_ttoi(options[i].substr(1).c_str());
					}else{
						::CoInitialize(NULL);
						IMultiLanguage2* lang=NULL;
						if(FAILED(CoCreateInstance(CLSID_CMultiLanguage,NULL,
												   CLSCTX_ALL,IID_IMultiLanguage2,
												   (LPVOID*)&lang)))
						   continue;

						MIMECSETINFO info;
						if(SUCCEEDED(lang->GetCharsetInfo(const_cast<wchar_t*>(options[i].substr(1).c_str()),&info))){
							CFG.general.arc_codepage=(info.uiInternetEncoding)?info.uiInternetEncoding:info.uiCodePage;
						}
						if(lang)lang->Release();
						::CoUninitialize();
					}
					dprintf(_T("CFG.general.arc_codepage=%d\n"),CFG.general.arc_codepage);
				}

				break;
			}//case 'C'

			case 'q':{
				//処理終了後閉じる
				switch(options[i].c_str()[1]){
					case '0':
					case '-':
						//'q0'
						CFG.general.quit=false;
						break;

					default:
						//'q'
						CFG.general.quit=true;
				}
				dprintf(_T("CFG.general.quit=%d\n"),CFG.general.quit);
				break;
			}//case 'q'

			case '@':{
				//'@'
				//リストファイル
				File list_file;

				if(!list_file.open(options[i].c_str()+1,OPEN_EXISTING,GENERIC_READ,0,CFG.general.list_codepage)){
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
				//'w'
				//wait指定
				if(options[i].find(_T("wait:"))==0&&
				   options[i][lstrlen(_T("wait:"))]!='\0'){
					ARCCFG->m_wait=_ttoi(options[i].substr(5).c_str());
					dprintf(_T("wait:%d\n"),ARCCFG->m_wait);
				}
			}

			default:
				break;
		}
	}

	//dllなら拡張子を取り除く
	removeDllExt(&CFG.general.selected_library_name);

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
