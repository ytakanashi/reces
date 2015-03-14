//SplitFile.h

#ifndef _SPLITFILE_H_632786A7_CD87_473a_96ED_0262B064C6D6
#define _SPLITFILE_H_632786A7_CD87_473a_96ED_0262B064C6D6



namespace sslib{
namespace splitfile{

namespace split{
enum RESULT{
	SUCCESS,
	CANNOT_OPEN,
	INVALID_PARAM,
	NOT_NECESSARY,
	TOO_MANY,
	MALLOC_ERR,
	CANNOT_CREATE
};
}

namespace join{
enum RESULT{
	SUCCESS,
	NOT_SPLIT,
	CANNOT_CREATE,
	MALLOC_ERR
};
}

//ファイルを分割する(分割サイズまたは分割数指定による)
//分割サイズは1024b、10k、100mの様に指定(10kb、100mbでも可)
//分割数は6,12の様に単位を付加せず指定すること
split::RESULT splitFile(const TCHAR* file_name,const TCHAR* param,const TCHAR* output_dir=NULL);//,ProgressBar* p_progressbar=NULL);

//分割ファイルの先頭ファイルかどうか
//拡張子が000、001、0000、00001、000000など
//分割ファイルならばその拡張子の桁数を取得
bool isSplitFile(const TCHAR* file_name,int* p_digit=NULL);

//ファイルを結合する
join::RESULT joinFile(const TCHAR* file_name,const TCHAR* output_dir=NULL);

//分割ファイルのリストを作成
bool makeSplitFileList(std::list<tstring>* file_list,const TCHAR* file_name);

//分割ファイルを削除
bool removeSplitFile(const TCHAR* file_name,bool recycle_bin);

//namespace splitfile
}
//namespace sslib
}

#endif //_SPLITFILE_H_632786A7_CD87_473a_96ED_0262B064C6D6
