//FileOperation.h

#ifndef _FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD
#define _FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD



namespace sslib{
namespace fileoperation{

enum SPLITFILE_RESULT{
	SFRET_SUCCESS,
	SFRET_CANNOT_OPEN,
	SFRET_INVALID_PARAM,
	SFRET_NOT_NECESSARY,
	SFRET_TOO_MANY,
	SFRET_MALLOC_ERR,
	SFRET_CANNOT_CREATE
};

enum JOINFILE_RESULT{
	JFRET_SUCCESS,
	JFRET_NOT_SPLIT,
	JFRET_CANNOT_CREATE,
	JFRET_MALLOC_ERR
};

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

//一意の名前を持つディレクトリ名を取得
tstring generateTempDirName(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つディレクトリを作成
tstring createTempDir(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つファイル名を取得
tstring generateTempFileName(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つファイルを作成
tstring createTempFile(const TCHAR* prefix,const TCHAR* base_dir=NULL);

//ファイルのバージョンを取得
bool getFileVersion(const TCHAR* file_path,DWORD* major_ver,DWORD* minor_ver);

//ファイルのサイズを取得
long long getFileSize(const TCHAR* file_path);

//ディレクトリのサイズを取得
long long getDirectorySize(const TCHAR* dir_path);

//ファイルを分割する(分割サイズまたは分割数指定による)
//分割サイズは1024b、10k、100mの様に指定(10kb、100mbでも可)
//分割数は6,12の様に単位を付加せず指定すること
SPLITFILE_RESULT splitFile(const TCHAR* file_name,const TCHAR* param,const TCHAR* output_dir=NULL);//,ProgressBar* p_progressbar=NULL);

//分割ファイルの先頭ファイルかどうか
//拡張子が000、001、0000、00001、000000など
//分割ファイルならばその拡張子の桁数を取得
bool isSplitFile(const TCHAR* file_name,int* p_digit=NULL);

//ファイルを結合する
JOINFILE_RESULT joinFile(const TCHAR* file_name,const TCHAR* output_dir=NULL);

//分割ファイルのリストを作成
template<class T>bool makeSplitFileList(T* file_list,const TCHAR* file_name);

//分割ファイルを削除
bool removeSplitFile(const TCHAR* file_name,bool recycle_bin);

//リソースからファイルを取り出す
bool extractFromResource(const HINSTANCE instance_handle,const WORD id,const TCHAR*type,const TCHAR* file_name);

//ショートカット作成
bool createShortcut(const TCHAR* shortcut_file,const TCHAR* src_file,const TCHAR* args=NULL,const TCHAR* description=NULL,const TCHAR* working_dir=NULL);

//namespace fileoperation
}
//namespace sslib
}

#endif //_FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD
