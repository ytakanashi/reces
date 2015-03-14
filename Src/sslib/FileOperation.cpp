//FileOperation.cpp
//ファイル操作

#include"../StdAfx.h"
#include"sslib.h"
#include<ctype.h>
#include<shlobj.h>


namespace sslib{
namespace fileoperation{

//対象ディレクトリが多階層に亘り存在しなくても作成
bool createDirectory(const TCHAR* dir_path_orig,LPSECURITY_ATTRIBUTES security_attributes){
	if(dir_path_orig==NULL)return false;
	if(path::isDirectory(dir_path_orig))return true;
	tstring dir_path(path::addTailSlash(dir_path_orig));
	tstring current_path;

	for(size_t i=0,length=dir_path.length();!IS_TERMINATED&&i<length;i++){
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

	for(fs.first(src_path.c_str());!IS_TERMINATED&&fs.next();){
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

	for(fs.first(dir_path.c_str());!IS_TERMINATED&&fs.next();){
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

	for(fs.first(dir_path);!IS_TERMINATED&&fs.next();){
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

	for(fs.first(dir_path);!IS_TERMINATED&&fs.next();){
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
