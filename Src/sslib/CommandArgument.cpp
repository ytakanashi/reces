﻿//CommandArgument.cpp
//コマンドライン引数を取得

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

namespace{
	enum{
		NUL='\0',
		DQUOTE='\"',
		SPACE=' ',
		ESCAPE='\\'
	};
	bool isNul(TCHAR c){return c==NUL;}
	bool isDQuote(TCHAR c){return c==DQUOTE;}
	bool isSpace(TCHAR c){return c==SPACE;}
	bool isEscape(TCHAR c){return c==ESCAPE;}

	void parse(std::vector<tstring>* list,const TCHAR* cmd_line=::GetCommandLine()){
		if(list==NULL)return;

		const TCHAR* args=cmd_line;

		dprintf(_T("GetCommandLine(): '%s'\n"),args);

		while(*args){
			bool in_dquotes=false;

			while(isSpace(*args))++args;
			if((in_dquotes=isDQuote(*args))){
				//二重引用符をスキップ
				++args;
			}

			const TCHAR* head=args;
			tstring arg(args);

			if(in_dquotes){
				while(*args&&
					  //引用符の終端を探す
					  !(isDQuote(*args)&&
						(isNul(*(args+1))||isSpace(*(args+1)))))++args;
				if(*args){
					arg=arg.substr(0,args-head);
					//次の文字列があれば進める
					if(*(args+1))args+=2;
				}
			}else{
				size_t pos=tstring::npos;

				while(*args&&
					  ((!in_dquotes&&!isSpace(*args))||
					  (in_dquotes&&
					   !(isDQuote(*args)&&!isEscape(*(args-1))&&(isNul(*(args+1))||isSpace(*(args+1))))
					   )
					   )){
					if(!isEscape(*(args-1))&&isDQuote(*args)){if(!in_dquotes)pos=args-head;in_dquotes=true;}
					++args;
				}
				if(in_dquotes){
					//文字列内に引用符が含まれる場合
					//'/@"abc de"'
					++args;
					arg=arg.substr(0,args-head);
					{
						//含まれる引用符削除
						if(pos!=tstring::npos)arg.erase(pos,1);
						//終端が'\"'でなければ削除
						if(arg[arg.length()-1]=='\"'&&((arg.length()>1)?arg[arg.length()-2]!='\\':1))arg.erase(arg.length()-1);
						//'\"'->'"'
						while((pos=arg.find(_T("\\\"")))!=tstring::npos)arg.erase(pos,1);
					}
					if(*args)++args;
				}else if(*args&&
				   isSpace(*args)){
					arg=arg.substr(0,args-head);
					++args;
				}
			}
			if(!arg.empty())list->push_back(arg);
		}
	}
}

CommandArgument::CommandArgument(DWORD opt):
	m_args(),m_options(),m_filepaths(){

	//引数分析
	parse(&m_args);
	//argv[0]は捨てる
	args().erase(args().begin());

	bool end_switch_scan=false;

	for(std::vector<tstring>::size_type i=0,size=args().size();i<size;++i){
		if(!end_switch_scan){
			switch(*(args()[i].c_str())){
				case '-':
				case '/':
					if(args()[i].find(_T("--"))==0||
					   args()[i].find(_T("//"))==0){
						//解析終了
						dprintf(_T("end_switch_scan=true(//)\n"));
						end_switch_scan=true;
						continue;
					}
					//オプション追加
					options().push_back(args()[i].c_str()+1);
					dprintf(_T("opt:%s\n"),args()[i].c_str());
					continue;

				default:
					//オプション解析終了
					dprintf(_T("end_switch_scan=true\n"));
					end_switch_scan=true;
					break;
			}
		}

		if(opt&DO_WILDCARD&&
		   path::containsWildcard(args()[i].c_str())){
			//ワイルドカードがあれば展開して追加
			tstring dir;
			TCHAR dlmtr[]={(TCHAR)path::guessDelimiter(args()[i].c_str()),'\0'};

			if(args()[i].rfind(dlmtr)!=std::string::npos){
				dir=args()[i].substr(0,args()[i].rfind(dlmtr));
			}

			HANDLE h=NULL;
			WIN32_FIND_DATA fd={};

			if((h=::FindFirstFile(path::addLongPathPrefix(args()[i]).c_str(),&fd))!=INVALID_HANDLE_VALUE){
				do{
					if(lstrcmp(fd.cFileName,_T("."))==0||
						lstrcmp(fd.cFileName,_T(".."))==0)continue;
					//ファイルを追加
					filepaths().push_back((!dir.empty())?dir+dlmtr+fd.cFileName:fd.cFileName);
					dprintf(_T("file:%s\n"),(!dir.empty())?(dir+dlmtr+fd.cFileName).c_str():fd.cFileName);
				}while(::FindNextFile(h,&fd));
			}

			::FindClose(h);
		}else{
			//ファイル追加
			filepaths().push_back(args()[i]);
			dprintf(_T("file:%s\n"),args()[i].c_str());
		}
	}

#ifndef DISABLE_CONSOLE
	Console input_file(STD_INPUT_HANDLE);
	DWORD mode=0;

	if(!input_file.getConsoleMode(&mode)){
		if(opt&READ_STDIN){
			//標準入力から取得
			DWORD read=0;
			std::vector<char> buffer;
			char c;

			while(!IS_TERMINATED&&
				  input_file.read((LPVOID)&c,sizeof(char),&read)){
				if(c=='\r')continue;
				if(c=='\n'){
					buffer.push_back('\0');
					stdinput().push_back(str::toUtf16(GetConsoleCP(),&buffer[0]));
					dprintf(_T("stdinput:%s\n"),stdinput().back().c_str());
					buffer.clear();
				}else{
					buffer.push_back(c);
				}
			}
		}

		//標準入力をコンソールに戻す(キーボードからの入力を可能にする)
#if 1
		//コンソールからの読み取りはReadFile()/ReadConsole()で行うのでfreopen()は必要なし
		CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
		SetStdHandle(STD_INPUT_HANDLE,CreateFile(_T("CONIN$"),GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,0));
#else
		//fgets()を使うならfreopen()が必要
#ifdef _tfreopen_s
		FILE*dummy;
		_tfreopen_s(&dummy,
#else
		_tfreopen(
#endif
			_T("CON"),_T("r"),stdin);
#endif

	}
#endif
}

//namespace sslib
}
