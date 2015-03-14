//Archiver.cpp
//書庫操作共通

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Archiver.h"

using namespace sslib;

//プログレスバー更新通知
const UINT Archiver::WM_UPDATE_PROGRESSBAR=::RegisterWindowMessage(_T("WM_UPDATE_PROGRESSBAR"));

namespace{
//書庫内の最小共通区切り文字数を取得
int countCommonComponents(const std::vector<fileinfo::FILEINFO>& paths){
	static tstring::size_type next_pos=0;
	struct l{
		static tstring extractComponent(const tstring& str,tstring::size_type pos){
			tstring::size_type first_pos=pos,second_pos=first_pos;

			++(next_pos=second_pos=str.find_first_of(_T("\\/"),first_pos));
			if(second_pos==tstring::npos)next_pos=second_pos=str.length();
			return (first_pos!=tstring::npos&&second_pos!=tstring::npos&&first_pos<second_pos)?
				str.substr(first_pos,second_pos-first_pos):
				_T("");
		}

		static bool isCommonComponent(const std::vector<fileinfo::FILEINFO>& paths,int max_length_index,tstring::size_type pt){
			bool result=false;

			tstring max_length_str=(extractComponent(paths[max_length_index].name,pt));
			for(int i=0,size=paths.size();i<size;i++){
				if(i!=max_length_index){
					tstring str=(extractComponent(paths[i].name,pt));

					//自分と異なる空白以外の要素があれば終了
					if(!str.empty()&&max_length_str!=str){
						result=false;
						break;
					}else{
						result=true;
						continue;
					}
				}
			}
			return result;
		}
	};

	next_pos=0;

	if(paths.size()<2)return 0;

	int result=0;
	int max_length_index=0;

	{
		int max_length=0;

		for(size_t i=0,size=paths.size();i<size;i++){
			int cur_length=str::countCharacter(paths[i].name,_T("\\/"));
			if(cur_length>max_length){max_length=cur_length;max_length_index=i;}
		}
	}

	for(int size=str::countCharacter(paths[max_length_index].name,_T("\\/"));result<size;++result){
		if(!l::isCommonComponent(paths,max_length_index,next_pos))break;
	}

	return result;
}
}

Archiver::Archiver(const TCHAR* library_name):
	Library(library_name),
	m_progress_thread_id(0),
	m_background_mode(false){
}

Archiver::Archiver(const TCHAR* library_name,const TCHAR* library_prefix):
	Library(library_name,library_prefix),
	m_progress_thread_id(0),
	m_background_mode(false){
}

//書庫内の最小共通区切り文字数を取得
int Archiver::countDelimiter(const TCHAR* arc_path,const std::vector<fileinfo::FILEINFO>& arc_info){
	int delimiter_count=0;

	if(!createFilesList(arc_path))return delimiter_count;

	delimiter_count=countCommonComponents(arc_info);

	return delimiter_count;
}

//プラグインについての情報を取得
tstring Archiver::getInformation(){
	if(!isLoaded())load();

	DWORD major_ver=0;
	DWORD minor_ver=0;

	if(fileoperation::getFileVersion(name().c_str(),&major_ver,&minor_ver)){

		return format(_T("%-12s ver.%d.%02d.%02d.%02d\n"),
								name().c_str(),
								major_ver>>16,
								major_ver&0xffff,
								minor_ver>>16,
								minor_ver&0xffff);
	}
	return _T("");
}

//ライブラリから情報を受け取るスレッドを設定
bool Archiver::setCallback(unsigned int progress_thread_id){
	clearCallback();

	m_progress_thread_id=progress_thread_id;
	return true;
}

//コールバックの設定を解除
void Archiver::clearCallback(){
	m_progress_thread_id=0;
}

#if 0
//バックグラウンドモードか否か
bool Archiver::getBackgroundMode(){
	return m_background_mode;
}
#endif

//バックグラウンドモードを設定
bool Archiver::setBackgroundMode(bool mode){
	return ::SetPriorityClass(::GetCurrentProcess(),
							  (m_background_mode=mode)?IDLE_PRIORITY_CLASS:NORMAL_PRIORITY_CLASS
							  )!=0;
}

//処理を中止する
void Archiver::abort(){
	if(!IS_TERMINATED){
		terminateApp();
	}
}

//true=break/false=continue
bool RedundantDir::operator()(const fileinfo::FILEINFO& fileinfo){
	tstring buffer(fileinfo.name.c_str());

	//パス区切り文字を'/'に統一する
	str::replaceCharacter(buffer,'\\','/');

	if(str::locateFirstCharacter(buffer.c_str(),'/')==-1){
		//区切り文字が含まれない
		if(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY){
			//unrar32.dllはディレクトリ名の末尾に区切り文字がないため
			//属性にて判断
			//例
			//foo\bar\baz.cpp
			//foo\qux.cpp
			//foo <-
			m_has_dir=true;
		}else{
			//ファイル
			if(!m_file_in_root_dir){
				//最初のファイルを確認
				//二つ目のファイルが見つかれば二重ディレクトリの疑いは晴れる
				m_file_in_root_dir=true;
			}else if(m_file_in_root_dir||m_has_dir){
				m_is_double_dir=false;
				return true;
			}
			return false;
		}
	}

	//以下、ディレクトリパスの処理

	//最上層のディレクトリを取得
	tstring cur_root_dir(path::getRootDirectory(buffer));

	if(m_root_dir.empty()){
		//最上層ディレクトリ保存
		m_root_dir=cur_root_dir;
		m_has_dir=true;
	}else if(m_root_dir!=cur_root_dir){
		//最上層にディレクトリやファイルが複数存在する
		m_is_double_dir=false;
		return true;
	}else{
		//最上層のディレクトリは今のところひとつ
		m_has_dir=true;
	}

	//最上層にファイルとディレクトリが存在する
	if(m_file_in_root_dir)m_is_double_dir=false;

	return m_file_in_root_dir;
}

namespace dirtimestamp{
//配下のファイルを利用してディレクトリのタイムスタンプを復元
bool recover(const TCHAR* dir,const FILETIME& arc_ft,const std::vector<tstring>& recovered_dirs){
	static const FILETIME m_arc_ft=arc_ft;
	static const std::vector<tstring> m_recovered_dirs=recovered_dirs;

	struct l{
		static bool getFILETIME(FILETIME* ft,const TCHAR* dir){
			FileSearch fs;

			for(fs.first(dir);!IS_TERMINATED&&fs.next();){
				if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
					if(!m_recovered_dirs.empty()&&std::binary_search(m_recovered_dirs.begin(),m_recovered_dirs.end(),fs.filepath())){
						File d(fs.filepath().c_str());

						d.getFileTime(ft);
					}else{
						if(!getFILETIME(ft,fs.filepath().c_str())){
							*ft=m_arc_ft;
						}
					}
					return true;
				}else{
					*ft=fs.data()->ftLastWriteTime;
					return true;
				}
			}
			return false;
		}
	};

	if(path::isDirectory(dir)){
		FILETIME ft={};

		if(!l::getFILETIME(&ft,dir)){
			ft=m_arc_ft;
		}

		File d(dir);

		return d.setFileTime(&ft);
	}
	return false;
}

bool addIncompleteDir(const tstring& output_dir,const tstring& name,std::vector<tstring>* incomplete_dirs){
	if(incomplete_dirs==NULL)return false;

	for(size_t pos=0;;++pos){
		pos=name.find_first_of(_T("\\/"),pos);
		tstring dir(name.substr(0,pos));
		if(path::isDirectory((output_dir+dir).c_str())){
			incomplete_dirs->push_back(output_dir+dir);
		}
		if(pos==tstring::npos)break;
	}
	std::sort(incomplete_dirs->begin(),incomplete_dirs->end());
	incomplete_dirs->erase(std::unique(incomplete_dirs->begin(),incomplete_dirs->end()),incomplete_dirs->end());
	return true;
}
}

namespace filter{
//属性フィルタにマッチするか(DIR属性が格納されていない書庫の為に...)
bool matchAttributes(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool delimiter_check){
	//i:ad[ディレクトリに格納されているファイル/空ディレクトリ]
	if(filefilter.attr&FILE_ATTRIBUTE_DIRECTORY){
		//ライブラリによっては主に使用するデリミタとリスト表示のデリミタが異なる場合有り
		if((((delimiter_check)?
			(str::countCharacter(fileinfo.name.c_str(),'/')==0&&
			 str::countCharacter(fileinfo.name.c_str(),'\\')==0):
			true))&&
			!(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY)){
			return false;
		}
	}

	//x:ad
	if(file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
		if((((delimiter_check)?
			(str::countCharacter(fileinfo.name.c_str(),'/')||
			 str::countCharacter(fileinfo.name.c_str(),'\\')):
			false))||
			fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY){
			return false;
		}
	}

	//x:ae
	if(!file_ex_filter.include_empty_dir){
		if((delimiter_check)?
		   fileinfo.isDirectory(delimiter_check):
		   //圧縮時用
		   path::isEmptyDirectory(fileinfo.name.c_str())){
			return false;
		}
	}

	//ディレクトリはチェック済みであるので
	//FILE_ATTRIBUTE_DIRECTORYを取り除く
	DWORD filefilter_attr=filefilter.attr&~FILE_ATTRIBUTE_DIRECTORY;
	DWORD file_ex_filter_attr=file_ex_filter.attr&~FILE_ATTRIBUTE_DIRECTORY;

	if(filefilter_attr!=0&&
	   !(fileinfo.attr&filefilter_attr)){
		return false;
	}

	if(file_ex_filter_attr!=0&&
	   fileinfo.attr&file_ex_filter_attr){
		return false;
	}
	return true;
}

//フィルタにマッチするか
bool match(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	if(!filefilter.empty()||!file_ex_filter.empty()){
		matched=((!filefilter.regex)?
			fileinfo::matchPattern(fileinfo,filefilter,_T("")):
			 fileinfo::matchRegex(fileinfo,filefilter));
		if(matched)matched=((!file_ex_filter.regex)?
			fileinfo::matchExPattern(fileinfo,file_ex_filter,_T("")):
			 fileinfo::matchExRegex(fileinfo,file_ex_filter));

		if(matched)matched=fileinfo::matchSize(fileinfo,filefilter,file_ex_filter);
		if(matched)matched=fileinfo::matchDateTime(fileinfo,filefilter,file_ex_filter);
		//注意::FileInfoのmatchAttributeではディレクトリを扱わない
		if(matched)matched=matchAttributes(fileinfo,filefilter,file_ex_filter);
	}

	return matched;
}
}
