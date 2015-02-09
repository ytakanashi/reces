//FileOperation.cpp
//ファイル操作

#include"../StdAfx.h"
#include"sslib.h"
#include<ctype.h>
#include<vector>
#include<shlobj.h>


namespace sslib{
namespace fileoperation{

//splitFile()
const long split_buffer_size=(1<<20)*5;
const long split_max_part=999+26*26*26;


//対象ディレクトリが多階層に亘り存在しなくても作成
bool createDirectory(const TCHAR* dir_path_orig,LPSECURITY_ATTRIBUTES security_attributes){
	if(dir_path_orig==NULL)return false;
	if(path::isDirectory(dir_path_orig))return true;
	tstring dir_path(path::addTailSlash(dir_path_orig));
	tstring current_path;

	for(size_t i=0,length=dir_path.length();i<length;i++){
#ifndef UNICODE
			if(::IsDBCSLeadByte(dir_path.c_str()[i])){
				++i;
				continue;
			}
#endif

		current_path.push_back(dir_path.c_str()[i]);

		if(dir_path.c_str()[i]=='\\'||dir_path.c_str()[i]=='/'){
			//同一名称のファイルやフォルダが存在しなければフォルダ作成
			if(!path::fileExists(current_path.c_str())){
				if(!::CreateDirectory(path::addLongPathPrefix(current_path).c_str(),security_attributes)){
					return false;
				}else{
					::SHChangeNotify(SHCNE_MKDIR,SHCNF_PATH,(const void*)current_path.c_str(),NULL);
				}
			}
		}
	}
	return true;
}

//ファイル/ディレクトリをリネーム
//異なるドライブや、途中のディレクトリが重複する場合失敗
bool renameFile(const TCHAR* src_orig,const TCHAR* dest_orig){
	if(src_orig==NULL||dest_orig==NULL)return false;
	if(lstrcmpi(src_orig,dest_orig)==0)return true;

	tstring src(path::removeTailSlash(src_orig));
	tstring dest(path::removeTailSlash(dest_orig));

	std::list<tstring> src_list;
	std::list<tstring> dest_list;

	str::splitString(&src_list,src.c_str(),(str::locateFirstCharacter(src.c_str(),'\\'))?'\\':'/');
	str::splitString(&dest_list,dest.c_str(),(str::locateFirstCharacter(dest.c_str(),'\\'))?'\\':'/');
	if(src_list.size()!=dest_list.size())return false;

	tstring cur_path;
	bool result=false;

	for(std::list<tstring>::iterator ite_src=src_list.begin(),
		end_src=src_list.end(),
		ite_dest=dest_list.begin();
		ite_src!=end_src;
		++ite_src,++ite_dest){
		if(*ite_src==*ite_dest){
			cur_path+=*ite_src+_T("\\");
		}else{
			tstring tmp(cur_path);
			tmp+=*ite_src;
			cur_path+=*ite_dest;
			if(!path::isRoot(cur_path.c_str())){
				result=::MoveFileEx(path::addLongPathPrefix(tmp).c_str(),path::addLongPathPrefix(cur_path).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
			}
			cur_path+=_T("\\");
		}
	}
	return result;
}

//ディレクトリやファイルをごみ箱に送る(MAX_PATH制限有)
bool moveToRecycleBinSH(const TCHAR* dir_path_orig){
	if(!path::fileExists(dir_path_orig))return false;

	std::vector<TCHAR> dir_path(MAX_PATHW);
	SHFILEOPSTRUCT fop={};

	lstrcpyn(&dir_path[0],path::removeTailSlash(dir_path_orig).c_str(),dir_path.size());
	lstrcat(&dir_path[0],_T("\0\0"));

	fop.pFrom=&dir_path[0];
	fop.hwnd=NULL;
	fop.wFunc=FO_DELETE;
	fop.pTo=NULL;
	fop.fFlags=FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION|FOF_ALLOWUNDO;
	fop.fAnyOperationsAborted=false;
	fop.hNameMappings=NULL;
	fop.lpszProgressTitle=NULL;
	return SHFileOperation(&fop)==0;
}

//ファイルやディレクトリを移動(MAX_PATH制限有)
bool moveFileSH(const TCHAR* src_path_orig,const TCHAR* dest_path_orig,FILEOP_FLAGS flag){
	std::vector<TCHAR> src_path(MAX_PATHW);
	std::vector<TCHAR> dest_path(MAX_PATHW);
	tstring tmp;
	SHFILEOPSTRUCT fop={};

	tmp.assign(path::removeTailSlash(src_path_orig));
	lstrcpyn(&src_path[0],
			 (tmp.length()<MAX_PATH)?tmp.c_str():path::getShortPathName(tmp.c_str()).c_str(),
			 src_path.size());
	lstrcat(&src_path[0],_T("\0\0"));

	tmp.assign(path::removeTailSlash(dest_path_orig));
	lstrcpyn(&dest_path[0],
			 (tmp.length()<MAX_PATH)?tmp.c_str():path::getShortPathName(tmp.c_str()).c_str(),
			 dest_path.size());
	lstrcat(&dest_path[0],_T("\0\0"));

	if(lstrcmp(&src_path[0],_T(""))==0||
	   lstrcmp(&dest_path[0],_T(""))==0)return false;

	fop.pFrom=&src_path[0];
	fop.hwnd=NULL;
	fop.wFunc=FO_MOVE;
	fop.pTo=&dest_path[0];
	fop.fFlags=flag;
	fop.hNameMappings=NULL;
	fop.lpszProgressTitle=NULL;
	return SHFileOperation(&fop)==0&&
		fop.fAnyOperationsAborted==0;
}

//ファイルやディレクトリをコピー(MAX_PATH制限有)
bool copyFileSH(const TCHAR* src_path_orig,const TCHAR* dest_path_orig,FILEOP_FLAGS flag){
	std::vector<TCHAR> src_path(MAX_PATHW);
	std::vector<TCHAR> dest_path(MAX_PATHW);
	tstring tmp;
	SHFILEOPSTRUCT fop={};

	tmp.assign(path::removeTailSlash(src_path_orig));
	lstrcpyn(&src_path[0],
			 (tmp.length()<MAX_PATH)?tmp.c_str():path::getShortPathName(tmp.c_str()).c_str(),
			 src_path.size());
	lstrcat(&src_path[0],_T("\0\0"));

	tmp.assign(path::removeTailSlash(dest_path_orig));
	lstrcpyn(&dest_path[0],
			 (tmp.length()<MAX_PATH)?tmp.c_str():path::getShortPathName(tmp.c_str()).c_str(),
			 dest_path.size());
	lstrcat(&dest_path[0],_T("\0\0"));

	if(lstrcmp(&src_path[0],_T(""))==0||
	   lstrcmp(&dest_path[0],_T(""))==0)return false;

	fop.pFrom=&src_path[0];
	fop.hwnd=NULL;
	fop.wFunc=FO_COPY;
	fop.pTo=&dest_path[0];
	fop.fFlags=flag;
	fop.hNameMappings=NULL;
	fop.lpszProgressTitle=NULL;
	return SHFileOperation(&fop)==0&&
		fop.fAnyOperationsAborted==0;
}

//ファイルやディレクトリを削除(MAX_PATH制限有)
bool removeFileSH(const TCHAR* file_path_orig){
	if(!path::fileExists(file_path_orig))return false;

	std::vector<TCHAR> file_path(MAX_PATHW);
	tstring tmp;
	SHFILEOPSTRUCT fop={};

	tmp.assign(path::removeTailSlash(file_path_orig));
	lstrcpyn(&file_path[0],
			 (tmp.length()<MAX_PATH)?tmp.c_str():path::getShortPathName(tmp.c_str()).c_str(),
			 file_path.size());
	lstrcat(&file_path[0],_T("\0\0"));

	if(lstrcmp(&file_path[0],_T(""))==0)return false;

	fop.pFrom=&file_path[0];
	fop.hwnd=NULL;
	fop.wFunc=FO_DELETE;
	fop.pTo=NULL;
	fop.fFlags=FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION;
	fop.hNameMappings=NULL;
	fop.lpszProgressTitle=NULL;
	return SHFileOperation(&fop)==0&&
		fop.fAnyOperationsAborted==0;
}

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//ファイルやディレクトリを削除
bool removeFile(const TCHAR* file_path_orig){
	if(!path::fileExists(file_path_orig))return false;

	if(path::isDirectory(file_path_orig)){
		deleteDirectory(file_path_orig);
	}else{
		//読み込み専用属性削除
		::SetFileAttributes(file_path_orig,
						  ::GetFileAttributes(file_path_orig)&~FILE_ATTRIBUTE_READONLY);
		return ::DeleteFile(file_path_orig)!=0;
	}
	return true;
}
#endif

//ディレクトリをディレクトリへ移動
void moveDirToDir(const TCHAR* src_path_orig,const TCHAR* dest_path_orig){
	if(src_path_orig==NULL||dest_path_orig==NULL)return;

	const tstring src_path=path::addTailSlash(src_path_orig);
	const tstring dest_path=path::addTailSlash(dest_path_orig);

	std::vector<scheduleDelete*> schedule_list;
	FileSearch fs;

	for(fs.first(src_path.c_str());fs.next();){
		FILETIME ft={};

		tstring file(fs.filepath());

		if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
			ft=fs.data()->ftLastWriteTime;
		}

		file.replace(0,src_path.length(),dest_path);

		if(file!=dest_path){
			if(::MoveFileEx(path::addLongPathPrefix(fs.filepath()).c_str(),path::addLongPathPrefix(file).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)==0){
				//移動に失敗した場合、取り敢えずコピー
				tstring dest((fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)?path::getParentDirectory(file):file));
				fileoperation::copyFileSH(fs.filepath().c_str(),path::addLongPathPrefix(dest).c_str());
				if(!fileoperation::removeFile(fs.filepath().c_str())){
					//削除は後に行う
					schedule_list.push_back(new scheduleDelete(fs.filepath().c_str()));
				}
			}

			if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
				//移動元のタイムスタンプを適用
				File d(file.c_str());

				d.setFileTime(&ft);
			}
		}
	}
	for(size_t i=0,list_size=schedule_list.size();i<list_size;i++){
		if(schedule_list[i]!=NULL){
			SAFE_DELETE(schedule_list[i]);
		}
	}
}

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//ディレクトリを削除
void deleteDirectory(const TCHAR* dir_path_orig){
	tstring dir_path(path::removeTailSlash(dir_path_orig));
	FileSearch fs;

	for(fs.first(dir_path.c_str());fs.next();){
		//読み込み専用属性削除
		::SetFileAttributes(fs.filepath().c_str(),
						  ::GetFileAttributes(fs.filepath().c_str())&~FILE_ATTRIBUTE_READONLY);
		if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
			deleteDirectory(fs.filepath().c_str());
		}else{
			::DeleteFile(fs.filepath().c_str());
		}
	}

	//対象ディレクトリ自身を操作するため、閉じておく
	fs.close();

	::SetFileAttributes(dir_path.c_str(),
					  ::GetFileAttributes(dir_path.c_str())&~FILE_ATTRIBUTE_READONLY);

	tstring cur_dir(path::getCurrentDirectory());

	::SetCurrentDirectory(path::getParentDirectory(cur_dir.c_str()).c_str());

	if(!::RemoveDirectory(path::addLongPathPrefix(dir_path).c_str())){
		removeFileSH(dir_path.c_str());
	}

	::SetCurrentDirectory(cur_dir.c_str());
}
#endif

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//ディレクトリ内のファイルやディレクトリを削除(指定されたディレクトリ自身は削除しない)
void deleteContents(const TCHAR* dir_path){
	FileSearch fs;

	for(fs.first(dir_path);fs.next();){
		//読み込み専用属性削除
		::SetFileAttributes(fs.filepath().c_str(),
						  ::GetFileAttributes(fs.filepath().c_str())&~FILE_ATTRIBUTE_READONLY);
		if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
			deleteContents(fs.filepath().c_str());
			if(!::RemoveDirectory(path::addLongPathPrefix(fs.filepath()).c_str())){
				removeFile(fs.filepath().c_str());
			}
		}else{
			::DeleteFile(path::addLongPathPrefix(fs.filepath()).c_str());
		}
	}
}
#endif

//一意の名前を持つディレクトリ名を取得
tstring generateTempDirName(const TCHAR* prefix,const TCHAR* base_dir){
	std::vector<TCHAR> temp_dir(MAX_PATHW);

	if(base_dir!=NULL){
		if(!path::fileExists(base_dir)){
			if(!createDirectory(base_dir))return _T("");
		}
	}

	::GetTempFileName(path::getTempDirPath().c_str(),prefix,0,&temp_dir[0]);
	::DeleteFile(&temp_dir[0]);

	return (base_dir!=NULL)?path::addTailSlash(base_dir)+=path::getFileName(&temp_dir[0]):&temp_dir[0];
}

//一意の名前を持つディレクトリを作成
tstring createTempDir(const TCHAR* prefix,const TCHAR* base_dir){
	tstring dir_name(generateTempDirName(prefix,base_dir));

	return (!dir_name.empty()&&createDirectory(dir_name.c_str()))?dir_name:_T("");
}

//一意の名前を持つファイル名を取得
tstring generateTempFileName(const TCHAR* prefix,const TCHAR* base_dir){
	tstring file_name(createTempFile(prefix,base_dir));

	if(!file_name.empty()){
		::DeleteFile(file_name.c_str());
	}
	return file_name;
}

//一意の名前を持つファイルを作成
tstring createTempFile(const TCHAR* prefix,const TCHAR* base_dir){
	std::vector<TCHAR> temp_dir(MAX_PATH);

	if(base_dir!=NULL){
		lstrcpyn(&temp_dir[0],base_dir,temp_dir.size());
		if(!path::fileExists(base_dir)){
			if(!createDirectory(base_dir))return _T("");
		}
	}else{
		lstrcpyn(&temp_dir[0],path::getTempDirPath().c_str(),temp_dir.size());
	}

	return (::GetTempFileName(&temp_dir[0],prefix,0,&temp_dir[0])!=0)?&temp_dir[0]:_T("");
}

//ファイルのバージョンを取得
bool getFileVersion(const TCHAR* file_path,DWORD* major_ver,DWORD* minor_ver){
	bool result=false;
	DWORD handle;
	unsigned char* ver_res=NULL;
	DWORD info_size=::GetFileVersionInfoSize(file_path,&handle);

	if(info_size!=0){
		ver_res=new unsigned char[info_size];
	}

	if(ver_res==NULL){
		if(path::fileExists(file_path)){
			//バージョンは取得できないが、存在はする
			result=true;
		}
	}else{
		if(::GetFileVersionInfo(file_path,handle/*無視される*/,info_size,ver_res)){
			void* ver_buffer=NULL;

			::VerQueryValue(ver_res,_T("\\"),&ver_buffer,NULL);
			VS_FIXEDFILEINFO *ver_info=(VS_FIXEDFILEINFO*)ver_buffer;
			*(major_ver)=ver_info->dwFileVersionMS;
			*(minor_ver)=ver_info->dwFileVersionLS;

			result=true;
		}
		SAFE_DELETE_ARRAY(ver_res);
	}
	return result;
}

#ifdef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
//ファイルのサイズを取得
long long getFileSize(const TCHAR* file_path){
	if(path::isDirectory(file_path))return getDirectorySize(file_path);

	File file(file_path);
	return file.getSize();
}
#endif

#ifdef _FILESEARCH_H_5AFE0001_7E75_4496_A177_D666A6867AD3
//ディレクトリのサイズを取得
long long getDirectorySize(const TCHAR* dir_path){
	FileSearch fs;
	long long dir_size=0;

	for(fs.first(dir_path);fs.next();){
		if(!fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
			//ファイル
			dir_size+=fs.getSize();
		}else{
			//サブディレクトリを検索
			dir_size+=getDirectorySize(fs.filepath().c_str());
		}
	}
	return dir_size;
}
#endif

#ifdef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
//ファイルを分割する(分割サイズまたは分割数指定による)
//分割サイズは1024b、10k、100mの様に指定(10kb、100mbでも可)
//分割数は6,12の様に単位を付加せず指定すること
SPLITFILE_RESULT splitFile(const TCHAR* file_name,const TCHAR* param,const TCHAR* output_dir){
	long long split_size=0;
	long long* split_sizes=NULL;
	long long chunks=0;
	bool by_count=false;

	if(file_name==NULL||param==NULL){
		return SFRET_CANNOT_OPEN;
	}

	File src_file;

	if(!src_file.open(file_name)){
		return SFRET_CANNOT_OPEN;
	}

	//分割数指定
	by_count=isdigit(param[lstrlen(param)-1])!=0;

	((!by_count)?split_size:chunks)=str::filesize2longlong(param);

	if((!by_count&&split_size<1)||
	   (by_count&&chunks<2)){
		return SFRET_INVALID_PARAM;
	}

	long long src_file_size;

	src_file_size=src_file.getSize();

	if(src_file_size<=split_size){
		return SFRET_NOT_NECESSARY;
	}

	if(chunks>split_max_part||
	   (chunks==0&&src_file_size/split_size>split_max_part)){
		return SFRET_TOO_MANY;
	}

	if(chunks>0){
		split_sizes=new long long[chunks];

		for(int ii=0;ii<chunks;ii++){
			//小数点以下を切り捨てた数値を配列に代入
			split_sizes[ii]=src_file_size/chunks;
		}

		if(src_file_size%chunks!=0){
			//割り切れなければ、先頭のファイルから1バイトずつファイルサイズを増やす
			for(int i=0;i<chunks;i++){
				split_sizes[i]++;
				long long total=0;
				for(int ii=0;ii<chunks;ii++){
					total+=split_sizes[ii];
				}
				//割り切れればループを抜ける
				if(src_file_size%total==0)break;
			}
		}
	}

	TCHAR* file_buffer=new TCHAR[split_buffer_size];
	if(file_buffer==NULL){
		return SFRET_MALLOC_ERR;
	}

	long long total_written=0;

	tstring base_path;

	if(output_dir!=NULL){
		base_path=path::addTailSlash(output_dir);
		base_path+=path::getFileName(file_name);
	}else{
		base_path=file_name;
	}

	tstring output_path;

	for(long i=0;;i++){
		long long dest_file_size=0,read_size=0,written_size=0;
		File dest_file;

		output_path=base_path+_T(".")+path::createPartExtension(i+1);

		if(!dest_file.open(output_path.c_str(),OPEN_ALWAYS)){
			return SFRET_CANNOT_CREATE;
		}

		if(chunks>0)split_size=split_sizes[i];

		for(dest_file_size=0,read_size=split_buffer_size;dest_file_size<split_size;dest_file_size+=read_size){
			if(read_size>split_size-dest_file_size)read_size=split_size-dest_file_size;

			written_size=src_file.read(file_buffer,read_size);
			total_written+=written_size;
			if(written_size<=0)break;

			written_size=dest_file.write(file_buffer,written_size);
		}
		if(written_size<=0||total_written==src_file_size)break;
	}
	if(chunks)SAFE_DELETE_ARRAY(split_sizes);
	SAFE_DELETE_ARRAY(file_buffer);
	return SFRET_SUCCESS;
}
#endif

//分割ファイルの先頭ファイルかどうか
//拡張子が000、001、0000、00001、000000など
//分割ファイルならばその拡張子の桁数を取得
bool isSplitFile(const TCHAR* file_name,int* p_digit){
	bool result=false;
	//拡張子の桁数
	int digit=0;
	int dot_pos=str::locateLastCharacter(file_name,'.');

	if(dot_pos!=-1){
		tstring ext=path::getExtension(file_name);
		if(ext[ext.length()-1]=='0'||ext[ext.length()-1]=='1'){
			//拡張子末尾が'0'か'1'であれば
			digit++;
			if(ext.length()==1){
				//'.0'の様に一桁である場合
				result=true;
			}else{
				for(size_t i=2;i<=ext.length();i++){
					if(ext[ext.length()-i]=='0'){
						digit++;
						result=true;
					}else{
						//'0'以外の数字があるため先頭ファイルではない
						result=false;
						digit=0;
						break;
					}
				}
			}
		}
	}
	if(p_digit)*p_digit=digit;
	return result;
}

#ifdef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
//ファイルを結合する
JOINFILE_RESULT joinFile(const TCHAR* file_name,const TCHAR* output_dir){
	int digit=0;

	if(!isSplitFile(file_name,&digit))return JFRET_NOT_SPLIT;

	tstring src_path;
	tstring dest_path;
	tstring base_path(file_name);
	int dot_pos=str::locateLastCharacter(file_name,'.');

	if(file_name==NULL||dot_pos==-1){
		return JFRET_NOT_SPLIT;
	}

	base_path=path::removeExtension(base_path);

	if(output_dir!=NULL){
		dest_path=path::addTailSlash(output_dir)+=path::getFileName(base_path);
	}else{
		dest_path=base_path;
	}

	File dest_file;

	if(!dest_file.open(dest_path.c_str(),CREATE_ALWAYS)){
		return JFRET_CANNOT_CREATE;
	}

	TCHAR* file_buffer=new TCHAR[split_buffer_size];
	if(file_buffer==NULL){
		return JFRET_MALLOC_ERR;
	}

	for(long i=0;;i++){
		long long dest_file_size=0,written_size=0;

		src_path=base_path+_T(".")+path::createPartExtension(i,digit);

		File src_file;

		if(!src_file.open(src_path.c_str())){
			//拡張子'00...0'が存在しない場合以外は分割ファイル終端とみなす
			if(i!=0)break;
			else continue;
		}

		while((written_size=src_file.read(file_buffer,split_buffer_size))){
			dest_file.write(file_buffer,written_size);
			dest_file_size+=written_size;
		}
	}
	SAFE_DELETE_ARRAY(file_buffer);

	return JFRET_SUCCESS;
}
#endif

//分割ファイルのリストを作成
template<class T>bool makeSplitFileList(T* file_list,const TCHAR* file_name){
	bool result=false;
	int digit=0;

	if(!isSplitFile(file_name,&digit))return false;

	for(long i=0;;i++){
		tstring src_path(path::removeExtension(file_name));
		src_path=src_path+_T(".")+path::createPartExtension(i,digit);

		if(!path::fileExists(src_path.c_str())){
			//拡張子'00...0'が存在しない場合以外は分割ファイル終端とみなす
			if(i!=0)break;
			else continue;
		}
		file_list->push_back(src_path);
		result=true;
	}
	return result;
}
template bool makeSplitFileList(std::list<tstring>* file_list,const TCHAR* file_name);
template bool makeSplitFileList(std::vector<tstring>* file_list,const TCHAR* file_name);

//分割ファイルを削除
bool removeSplitFile(const TCHAR* file_name,bool recycle_bin){
	std::list<tstring> file_list;

	if(!makeSplitFileList(&file_list,file_name)||
	   file_list.empty())return false;

	bool result=false;

	for(std::list<tstring>::iterator ite=file_list.begin(),
		end=file_list.end();
		ite!=end;
		++ite){
		if(recycle_bin){
			result=moveToRecycleBinSH(ite->c_str());
		}else{
			result=::DeleteFile(ite->c_str())!=0;
		}
	}
	return result;
}

#ifdef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
//リソースからファイルを取り出す
bool extractFromResource(const HINSTANCE instance_handle,const WORD id,const TCHAR*type,const TCHAR* file_name){
	HRSRC info=::FindResource(instance_handle,MAKEINTRESOURCE(id),type);
	if(!info){return false;}
	HGLOBAL resource_handle=::LoadResource(instance_handle,info);
	if(!resource_handle){return false;}

	DWORD resource_size=::SizeofResource(instance_handle,info);
	void* resource_data=::LockResource(resource_handle);

	File output(file_name,CREATE_ALWAYS);

	HANDLE file_map_handle=::CreateFileMapping(output.handle(),
											   NULL,
											   PAGE_READWRITE,
											   0,
											   resource_size,
											   NULL);

	void* base_addr=::MapViewOfFile(file_map_handle,FILE_MAP_WRITE,0,0,0);

	::CopyMemory(base_addr,resource_data,resource_size);

	::UnmapViewOfFile(base_addr);
	SAFE_CLOSE(file_map_handle);
	return true;
}
#endif

//ショートカット作成
bool createShortcut(const TCHAR* shortcut_file,const TCHAR* src_file,const TCHAR* args,int show_cmd,const TCHAR* description,const TCHAR* working_dir){
	bool result=false;

	::CoInitialize(NULL);
	IShellLink *pShellLink=NULL;
	IPersistFile *pPersistFile=NULL;

	if(SUCCEEDED(::CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&pShellLink))){
		if(SUCCEEDED(pShellLink->QueryInterface(IID_IPersistFile,(LPVOID*)&pPersistFile))){
			if(SUCCEEDED(pShellLink->SetPath(src_file))){
				if(args){
					pShellLink->SetArguments(args);
				}
				pShellLink->SetShowCmd(show_cmd);
				if(description){
					pShellLink->SetDescription(description);
				}
				if(working_dir){
					pShellLink->SetWorkingDirectory(working_dir);
				}

#ifdef _UNICODE
				if(SUCCEEDED(pPersistFile->Save(shortcut_file,true)))result=true;
#else
					if(SUCCEEDED(pPersistFile->Save(str::sjis2utf16(shortcut_file).c_str(),true)))result=true;
				}
#endif //_UNICODE

			}
			pPersistFile->Release();
		}
		pShellLink->Release();
	}

	::CoUninitialize();
	return result;
}


//namespace fileoperation
}
//namespace sslib
}
