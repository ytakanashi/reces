//SplitFile.cpp
//ファイル分割/結合

#include"../StdAfx.h"
#include"sslib.h"
#include<ctype.h>


namespace sslib{
namespace splitfile{

//splitFile()
const long split_buffer_size=(1<<20)*5;
const long split_max_part=999+26*26*26;


#ifdef _FILE_H_698018EF_AC41_48e5_9B7B_7AF556131869
//ファイルを分割する(分割サイズまたは分割数指定による)
//分割サイズは1024b、10k、100mの様に指定(10kb、100mbでも可)
//分割数は6,12の様に単位を付加せず指定すること
split::RESULT splitFile(const TCHAR* file_name,const TCHAR* param,const TCHAR* output_dir){
	long long split_size=0;
	long long* split_sizes=NULL;
	long long chunks=0;
	bool by_count=false;

	if(file_name==NULL||param==NULL){
		return split::CANNOT_OPEN;
	}

	File src_file;

	if(!src_file.open(file_name)){
		return split::CANNOT_OPEN;
	}

	//分割数指定
	by_count=isdigit(param[lstrlen(param)-1])!=0;

	((!by_count)?split_size:chunks)=str::filesize2longlong(param);

	if((!by_count&&split_size<1)||
	   (by_count&&chunks<2)){
		return split::INVALID_PARAM;
	}

	long long src_file_size;

	src_file_size=src_file.getSize();

	if(src_file_size<=split_size){
		return split::NOT_NECESSARY;
	}

	if(chunks>split_max_part||
	   (chunks==0&&src_file_size/split_size>split_max_part)){
		return split::TOO_MANY;
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
		if(chunks)SAFE_DELETE_ARRAY(split_sizes);
		return split::MALLOC_ERR;
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
			return split::CANNOT_CREATE;
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
	return split::SUCCESS;
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
join::RESULT joinFile(const TCHAR* file_name,const TCHAR* output_dir){
	int digit=0;

	if(!isSplitFile(file_name,&digit))return join::NOT_SPLIT;

	tstring src_path;
	tstring dest_path;
	tstring base_path(file_name);
	int dot_pos=str::locateLastCharacter(file_name,'.');

	if(file_name==NULL||dot_pos==-1){
		return join::NOT_SPLIT;
	}

	base_path=path::removeExtension(base_path);

	if(output_dir!=NULL){
		dest_path=path::addTailSlash(output_dir)+=path::getFileName(base_path);
	}else{
		dest_path=base_path;
	}

	File dest_file;

	if(!dest_file.open(dest_path.c_str(),CREATE_ALWAYS)){
		return join::CANNOT_CREATE;
	}

	TCHAR* file_buffer=new TCHAR[split_buffer_size];
	if(file_buffer==NULL){
		return join::MALLOC_ERR;
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

	return join::SUCCESS;
}
#endif

//分割ファイルのリストを作成
bool makeSplitFileList(std::list<tstring>* file_list,const TCHAR* file_name){
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

#ifdef _FILEOPERATION_H_883BE0AE_F9BD_4775_B57F_1795453033CD
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
			result=fileoperation::moveToRecycleBinSH(ite->c_str());
		}else{
			result=::DeleteFile(ite->c_str())!=0;
		}
	}
	return result;
}
#endif


//namespace splitfile
}
//namespace sslib
}
