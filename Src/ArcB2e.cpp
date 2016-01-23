//ArcB2e.cpp
//B2e.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcB2e.h"
#include"ArcCfg.h"

using namespace sslib;


ArcB2e::ArcB2e():
	ArcDll(
#ifndef _WIN64
			_T("b2e32"),
#else
			_T("b2e64"),
#endif
			_T("B2E"),
			_T("*"),
			_T("\\")),
		m_compress_b2e_list(),
		m_method_index(0){
		COMPRESSION_FORMAT format[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_formats.assign(format,format+ARRAY_SIZEOF(format));
}

//圧縮用b2eスクリプトで使用出来るformatとmethodの組み合わせを取得
tstring ArcB2e::getCompressScriptInformation(){
	if(!isLoaded())load();

	tstring info;

	//圧縮用b2eスクリプトの情報を列挙
	enumCompressScript();

	for(size_t i=0,list_size=m_compress_b2e_list.size();
		i<list_size;
		++i){
		size_t method_size=getB2e(i).methods.size();

		if(method_size==0){
			info.append(format(_T("%s\n"),
							   getFormat(i).c_str()));
			continue;
		}

		for(size_t ii=0;ii<method_size;++ii){
			info.append(format(_T("%s:%s\n"),
							   getFormat(i).c_str(),
							   getMethod(i,ii).c_str()));
		}
	}
	return info;
}

//対応している圧縮形式であるか
bool ArcB2e::isSupportedFormat(const TCHAR* format,const TCHAR* method){
	size_t size=m_compress_b2e_list.size();

	for(m_format_index=0;
		m_format_index<size;
		++m_format_index){
		if(str::isEqualStringIgnoreCase(m_compress_b2e_list[m_format_index].format,format)){
			if(lstrcmp(method,_T(""))==0){
				m_method_index=m_compress_b2e_list[m_format_index].default_method;
				return true;
			}

			size_t method_size=m_compress_b2e_list[m_format_index].methods.size();

			for(m_method_index=0;
				m_method_index<method_size;
				++m_method_index){
				if(str::isEqualStringIgnoreCase(m_compress_b2e_list[m_format_index].methods[m_method_index],method)){
					return true;
				}
			}
		}
	}
	m_format_index=0;
	return false;
}

ArcB2e::ARC_RESULT ArcB2e::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	tstring arc_path_str(arc_path);

	if(!(getScriptAbility(getB2e().index)&B2E_ABILITY_COMPRESS))return ARC_NOT_IMPLEMENTED;

	tstring list_file_path;
	File list_file;

	//リストファイルを作成
	list_file_path=tempfile::create(_T("b2e"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	//書庫ファイル名出力
	list_file.writeEx(_T("%s\r\n"),arc_path_str.c_str());

	//リストファイルにパスを出力
	if(CFG.mode==MODE_RECOMPRESS){
		list_file.writeEx(_T("*\r\n"));
	}else{
		for(std::list<tstring>::const_iterator ite=file_list->begin(),
			end=file_list->end();
			ite!=end;
			++ite){
			writeFormatedList(list_file,*ite);
		}
	}

	if(!list_file.getSize())return ARC_NO_MATCHES_FOUND;

	list_file.close();

	//区切り文字置換
	replaceDelimiter(list_file_path);

	tstring cmd_line(format(_T("%s %s %s "),
							path::quote(tstring(_T("-c"))+getFormat()).c_str(),
							CFG.general.custom_param.c_str(),
							path::quote(tstring(_T("-@"))+list_file_path).c_str()));

	if(!getMethod().empty()){
		cmd_line.append(format(_T("%s "),
							   //-m<メソッド名> : 圧縮メソッドを指定。省略するとデフォルト圧縮メソッドが使われます。
							   path::quote(tstring(_T("-m"))+getMethod()).c_str()));
	}

	if(CFG.compress.b2e.sfx){
		cmd_line.append(_T("-s "));
	}

//	cmd_line.append(format(_T("-- %s"),
//						   path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	msg::info(_T("'%s'に圧縮しています...\n\n"),arc_path);

	//実行
	int dll_ret=-1;

	if(CFG.no_display.no_log||log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.c_str(),log_msg,log_buffer_size);
	}

	msg::info(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcB2e::ARC_RESULT ArcB2e::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	if(IS_TERMINATED)return ARC_FAILURE;

	tstring output_dir_bak;
	fileoperation::scheduleDelete schedule_delete;

	if(CFG.compress.exclude_base_dir!=0){
		output_dir_bak=output_dir_str;
		output_dir_str=path::addTailSlash(tempfile::createDir(_T("rcs"),output_dir_str.c_str()));

		//一時ディレクトリ削除予約
		schedule_delete.set(output_dir_str.c_str());
		//削除漏れ対策
		ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(output_dir_str.c_str()));
	}

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(output_dir_str);

	tstring cmd_line(format(_T("%s %s %s -- %s"),
							(!CFG.general.ignore_directory_structures)?_T("-e"):_T("-e -g"),
							//-e     : 解凍。使用するB2Eは自動的に探し出します。
							//-g     : 解凍時、パス情報を無視
							CFG.general.custom_param.c_str(),

							path::quote(tstring(_T("-o"))+output_dir_str).c_str(),

							path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	msg::info(_T("'%s'を解凍しています...\n\n"),arc_path);

	//実行
	int dll_ret=-1;

	if(CFG.no_display.no_log||log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.c_str(),log_msg,log_buffer_size);
	}

	msg::info(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir_str,'/','\\');

	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		m_util->recoverDirectoryTimestamp(arc_path,output_dir_str.c_str(),true);
	}

	unload();

	if(CFG.compress.exclude_base_dir!=0){
		//共通パスを取り除く
		m_util->excludeCommonPath(output_dir_bak.c_str(),output_dir_str.c_str(),CFG.compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcB2e::ARC_RESULT ArcB2e::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	if(CFG.output_file_list.api_mode){
		outputFileList(arc_path_str.c_str());
		return ARC_SUCCESS;
	}

	tstring cmd_line(format(_T("%s -- %s"),
							_T("-l"),
							//-l : 圧縮ファイル中のファイルを列挙
							path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	tstring log_msg;

	execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
	STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	return ARC_SUCCESS;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcB2e::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	//file_pathの末尾区切り文字を削ること
	return list_file.writeEx(_T("%s\r\n"),
							 path::quote(tstring(base_dir)+m_delimiter+path::removeTailSlash(file_path)).c_str());
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcB2e::writeFormatedList(const File& list_file,const tstring& full_path){
	if(path::isDirectory(full_path.c_str())&&
	   !path::isEmptyDirectory(full_path.c_str())){
		//full_pathはディレクトリ
		if(CFG.compress.exclude_base_dir==0){
			list_file.writeEx(_T("%s\r\n"),full_path.c_str());
		}else{
			//基底ディレクトリを含まない
			FileSearch fs;

			for(fs.first(full_path.c_str());!IS_TERMINATED&&fs.next();){
				list_file.writeEx(_T("%s\r\n"),fs.filepath().c_str());
			}
		}
	}else{
		//full_pathはファイル
		list_file.writeEx(_T("%s\r\n"),full_path.c_str());
	}
	return true;
}

//b2eスクリプトのあるディレクトリを指定
bool ArcB2e::setScriptDirectory(const TCHAR* dir_path){
	typedef BOOL(WINAPI*SETSCRIPTDIRECTORY_PTR)(const char*);
	SETSCRIPTDIRECTORY_PTR p_setScriptDirectory;
	bool result=false;

	if((p_setScriptDirectory=(SETSCRIPTDIRECTORY_PTR)getAddress(_T("SetScriptDirectory")))!=NULL){
		result=p_setScriptDirectory((isUnicodeMode()?str::utf162utf8(dir_path):str::utf162sjis(dir_path)).c_str())!=0;
	}
	return result;
}

//b2eスクリプトの数を取得
int ArcB2e::getScriptCount(){
	typedef int(WINAPI*SCRIPTGETCOUNT_PTR)();
	SCRIPTGETCOUNT_PTR p_ScriptGetCount;
	int result=-1;

	if((p_ScriptGetCount=(SCRIPTGETCOUNT_PTR)getAddress(_T("ScriptGetCount")))!=NULL){
		result=p_ScriptGetCount();
	}
	return result;
}

//b2eスクリプト名を取得
bool ArcB2e::getScriptName(const UINT index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETNAME_PTR)(const UINT,char*,DWORD);
	SCRIPTGETNAME_PTR p_ScriptGetName;
	bool result=false;

	if((p_ScriptGetName=(SCRIPTGETNAME_PTR)getAddress(_T("ScriptGetName")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetName(index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//b2eスクリプトの能力を列挙
ArcB2e::B2E_ABILITY ArcB2e::getScriptAbility(const UINT index){
	typedef int(WINAPI*SCRIPTGETABILITY_PTR)(const UINT,DWORD*);
	SCRIPTGETABILITY_PTR p_ScriptGetAbility;
	B2E_ABILITY result=B2E_ABILITY_NONE;
	DWORD ability=0;

	if((p_ScriptGetAbility=(SCRIPTGETABILITY_PTR)getAddress(_T("ScriptGetAbility")))!=NULL){
		if(p_ScriptGetAbility(index,&ability)){
			result=static_cast<B2E_ABILITY>(ability);
		}
	}
	return result;
}

//解凍できる拡張子('.'付き)の_nExtIndex番目のものを取得
bool ArcB2e::getExtractExtensions(const UINT index,const UINT ext_index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETEXTRACTEXTENSIONS_PTR)(const UINT,const UINT,char*,DWORD);
	SCRIPTGETEXTRACTEXTENSIONS_PTR p_ScriptGetExtractExtensions;
	bool result=false;

	if((p_ScriptGetExtractExtensions=(SCRIPTGETEXTRACTEXTENSIONS_PTR)getAddress(_T("ScriptGetExtractExtensions")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetExtractExtensions(index,ext_index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//b2eの圧縮対応形式名を取得(拡張子名でないことに注意)
bool ArcB2e::getCompressType(const UINT index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETCOMPRESSTYPE_PTR)(const UINT,char*,DWORD);
	SCRIPTGETCOMPRESSTYPE_PTR p_ScriptGetCompressType;
	bool result=false;

	if((p_ScriptGetCompressType=(SCRIPTGETCOMPRESSTYPE_PTR)getAddress(_T("ScriptGetCompressType")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetCompressType(index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//圧縮用b2eの対応メソッドを取得
bool ArcB2e::getCompressMethod(const UINT index,const UINT mhd_index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETCOMPRESSMETHOD_PTR)(const UINT,const UINT,char*,DWORD);
	SCRIPTGETCOMPRESSMETHOD_PTR p_ScriptGetCompressMethod;
	bool result=false;

	if((p_ScriptGetCompressMethod=(SCRIPTGETCOMPRESSMETHOD_PTR)getAddress(_T("ScriptGetCompressMethod")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetCompressMethod(index,mhd_index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//圧縮用B2Eのデフォルトメソッド番号を取得
int ArcB2e::getDefaultCompressMethod(const UINT index){
	typedef int(WINAPI*SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR)(int);
	SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR p_ScriptGetDefaultCompressMethod;
	int result=-1;

	if((p_ScriptGetDefaultCompressMethod=(SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR)getAddress(_T("ScriptGetDefaultCompressMethod")))!=NULL){
		result=p_ScriptGetDefaultCompressMethod(index);
	}
	return result;
}

//指定したアーカイブファイルの解凍に使うB2Eのインデックスを取得
UINT ArcB2e::getExtractorIndex(const TCHAR* file_path){
	typedef UINT(WINAPI*SCRIPTGETEXTRACTORINDEX_PTR)(const char*);
	SCRIPTGETEXTRACTORINDEX_PTR p_ScriptGetExtractorIndex;
	UINT result=0xffffffff;

	if((p_ScriptGetExtractorIndex=(SCRIPTGETEXTRACTORINDEX_PTR)getAddress(_T("ScriptGetExtractorIndex")))!=NULL){
		result=p_ScriptGetExtractorIndex((isUnicodeMode()?str::utf162utf8(file_path):str::utf162sjis(file_path)).c_str());
	}
	return result;
}

//圧縮用b2eスクリプトの情報を列挙
bool ArcB2e::enumCompressScript(){
	m_compress_b2e_list.clear();

	int count=getScriptCount();

	if(count==-1)return false;

	for(size_t index=0;index<(size_t)count;++index){
		B2E_INFO info;

		info.index=index;
		info.ability=getScriptAbility(index);

		if(info.ability&B2E_ABILITY_COMPRESS){
			tstring buffer(128,'\0');

			getCompressType(index,&buffer,buffer.length());
			info.format.assign(buffer);

			for(int i=0;;++i){
				tstring buffer(128,'\0');

				if(!getCompressMethod(index,i,&buffer,buffer.length()))break;
				info.methods.push_back(buffer);
			}

			info.default_method=getDefaultCompressMethod(index);

			m_compress_b2e_list.push_back(info);
		}
	}

	return !m_compress_b2e_list.empty();
}
