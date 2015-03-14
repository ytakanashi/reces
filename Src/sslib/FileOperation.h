//FileOperation.h

#ifndef _FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD
#define _FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD



namespace sslib{
namespace fileoperation{

//対象ディレクトリが多階層に亘り存在しなくても作成
bool createDirectory(const TCHAR* dir_path_orig,LPSECURITY_ATTRIBUTES security_attributes=NULL);

//ファイル/ディレクトリをリネーム
bool renameFile(const TCHAR* src_orig,const TCHAR* dest_orig);

//ディレクトリやファイルをごみ箱に送る(MAX_PATH制限有)
bool moveToRecycleBinSH(const TCHAR* dir_path_orig);

//ファイルやディレクトリを移動(MAX_PATH制限有)
bool moveFileSH(const TCHAR* src_path_orig,const TCHAR* dest_path_orig,FILEOP_FLAGS flag=FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION);

//ファイルやディレクトリをコピー(MAX_PATH制限有)
bool copyFileSH(const TCHAR* src_path_orig,const TCHAR* dest_path_orig,FILEOP_FLAGS flag=FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION);

//ディレクトリをディレクトリへ移動
void moveDirToDir(const TCHAR* src_path_orig,const TCHAR* dest_path_orig);

//ファイルやディレクトリを削除(MAX_PATH制限有)
bool removeFileSH(const TCHAR* file_path_orig);
//ファイルやディレクトリを削除
bool removeFile(const TCHAR* file_path_orig);

//ディレクトリを削除
void deleteDirectory(const TCHAR* dir_path_orig);

//ディレクトリ内のファイルやディレクトリを削除(指定されたディレクトリ自身は削除しない)
void deleteContents(const TCHAR* dir_path);

//削除予約
class scheduleDelete:private misc::NonCopyable<scheduleDelete>{
public:
	scheduleDelete(const TCHAR* file_path=NULL):m_file_path(){if(file_path)m_file_path=file_path;}
	~scheduleDelete(){
		if(path::fileExists(m_file_path.c_str())){
			if(path::isDirectory(m_file_path.c_str())){
				deleteContents(m_file_path.c_str());
				if(!::RemoveDirectory(path::addLongPathPrefix(m_file_path).c_str())){
					removeFile(m_file_path.c_str());
				}
			}else{
				::DeleteFile(m_file_path.c_str());
			}
		}
	}
	void set(const TCHAR* file_path){m_file_path=file_path;}
	inline const tstring filepath()const{return m_file_path;}
private:
	tstring m_file_path;
};

//ファイルのバージョンを取得
bool getFileVersion(const TCHAR* file_path,DWORD* major_ver,DWORD* minor_ver);

//ファイルのサイズを取得
long long getFileSize(const TCHAR* file_path);

//ディレクトリのサイズを取得
long long getDirectorySize(const TCHAR* dir_path);

//分割ファイルを削除
bool removeSplitFile(const TCHAR* file_name,bool recycle_bin);

//リソースからファイルを取り出す
bool extractFromResource(const HINSTANCE instance_handle,const WORD id,const TCHAR*type,const TCHAR* file_name);

//ショートカット作成
bool createShortcut(const TCHAR* shortcut_file,const TCHAR* src_file,const TCHAR* args=NULL,int show_cmd=SW_SHOWNORMAL,const TCHAR* description=NULL,const TCHAR* working_dir=NULL);

//namespace fileoperation
}
//namespace sslib
}

#endif //_FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD
