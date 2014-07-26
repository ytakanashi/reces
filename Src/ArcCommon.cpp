//ArcCommon.cpp
//書庫操作共通

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r21 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcCommon.h"

using namespace sslib;

//プログレスバー更新通知
const UINT ArcCommon::WM_UPDATE_PROGRESSBAR=::RegisterWindowMessage(_T("WM_UPDATE_PROGRESSBAR"));


//属性フィルタを適用(DIR属性が格納されていない書庫の為に...)
bool ArcCommon::applyAttributeFilters(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	//i:ad[ディレクトリに格納されているファイル/空ディレクトリ]
	if(filefilter.attr&FILE_ATTRIBUTE_DIRECTORY){
		//ライブラリによっては主に使用するデリミタとリスト表示のデリミタが異なる場合有り
		if(str::countCharacter(fileinfo.name.c_str(),'/')==0&&
		   str::countCharacter(fileinfo.name.c_str(),'\\')==0&&
		   !(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY)){
			matched=false;
		}
	}

	if(matched){
		//x:ad
		if(file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY){
			if(str::countCharacter(fileinfo.name.c_str(),'/')||
			   str::countCharacter(fileinfo.name.c_str(),'\\')||
			   fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY){
				matched=false;
			}
		}
	}

	if(matched){
		//x:ae
		if(!file_ex_filter.include_empty_dir){
			if(fileinfo.isDirectory()){
				matched=false;
			}
		}
	}

	if(matched){
		DWORD fileinfo_attr=fileinfo.attr;
		//ディレクトリはチェック済みであるので、除外
		DWORD filefilter_attr=filefilter.attr&~FILE_ATTRIBUTE_DIRECTORY;
		DWORD file_ex_filter_attr=file_ex_filter.attr&~FILE_ATTRIBUTE_DIRECTORY;

		if(filefilter_attr!=0&&
		   !(fileinfo_attr&filefilter_attr)){
			matched=false;
		}

		if(file_ex_filter_attr!=0&&
		   fileinfo_attr&file_ex_filter_attr){
			matched=false;
		}
	}
	return matched;
}

//フィルタにマッチするか
bool ArcCommon::matchFilters(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter){
	bool matched=true;

	if(!filefilter.empty()||!file_ex_filter.empty()){
		matched=matchPattern(fileinfo,filefilter,_T(""));
		if(matched)matched=matchExPattern(fileinfo,file_ex_filter,_T(""));

		if(matched)matched=matchSize(fileinfo,filefilter,file_ex_filter);
		if(matched)matched=matchDateTime(fileinfo,filefilter,file_ex_filter);
		//FileInfoのmatchAttributeではi:ad/x:adをチェックしないため
		if(matched)matched=applyAttributeFilters(fileinfo,filefilter,file_ex_filter);
	}

	return matched;
}

//true=break/false=continue
bool ArcCommon::RedundantDir::operator()(const fileinfo::FILEINFO& fileinfo){
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
			if(!m_checked_first_file){
				//最初のファイルを確認
				//二つ目のファイルが見つかれば二重ディレクトリの疑いは晴れる
				m_checked_first_file=true;
			}else if(m_checked_first_file||m_has_dir){
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
	return false;
}

//配下のファイルを利用してディレクトリのタイムスタンプを復元
bool ArcCommon::DirTimeStamp::recover(const TCHAR* dir,const FILETIME& arc_ft,const std::vector<tstring>& recovered_dirs){
	static const FILETIME m_arc_ft=arc_ft;
	static const std::vector<tstring> m_recovered_dirs=recovered_dirs;

	struct l{
		static bool getFILETIME(FILETIME* ft,const TCHAR* dir){
			FileSearch fs;

			for(fs.first(dir);fs.next();){
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

bool ArcCommon::DirTimeStamp::addIncompleteDir(const tstring& output_dir,const tstring& name,std::vector<tstring>* incomplete_dirs){
	if(incomplete_dirs==NULL)return false;

	for(size_t pos=0;;++pos){
		pos=name.find_first_of('\\',pos);
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

//共通パスを取り除く
//解凍レンジ「パス情報を最適化して展開する」相当
//level=-1で共通パスをすべて取り除く
bool ArcCommon::excludeCommonPath(const TCHAR* output_dir,const TCHAR* target_dir,int level){
	if(output_dir==NULL||target_dir==NULL)return false;

	static const int m_level=level;

	struct l{
		static bool search(tstring* result,const TCHAR* dir,int cur_level){
			if(result==NULL||(m_level!=-1&&m_level<=cur_level))return false;
			FileSearch fs;

			fs.first(dir);
			if(!fs.next())return false;
			if(fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY)){
				bool dir_attr=fs.hasAttribute(FILE_ATTRIBUTE_DIRECTORY);
				tstring filename(fs.filepath());

				if(!fs.next()){
					if(dir_attr){
						if(!search(result,filename.c_str(),++cur_level)){
							*result=filename;
						}
						return true;
					}
				}
			}
			return false;
		}
	};

	tstring common_dir(target_dir);
	int cur_level=0;

	l::search(&common_dir,target_dir,cur_level);

	fileoperation::moveDirToDir(common_dir.c_str(),output_dir);

	return true;
}

//処理を中止する
void ArcCommon::abort(){
	if(!isTerminated()){
		terminateApp();
	}
}
