//TempFile.h

#ifndef _TEMPFILE_H_8FE1BEFB_D32A_4ade_9463_4A3D34ACB7F5
#define _TEMPFILE_H_8FE1BEFB_D32A_4ade_9463_4A3D34ACB7F5



namespace sslib{
namespace tempfile{

//一意の名前を持つディレクトリ名を取得
tstring generateDirName(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つディレクトリを作成
tstring createDir(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つファイル名を取得
tstring generateName(const TCHAR* prefix,const TCHAR* base_dir=NULL);
//一意の名前を持つファイルを作成
tstring create(const TCHAR* prefix,const TCHAR* base_dir=NULL);

//namespace tempfile
}
//namespace sslib
}

#endif //_TEMPFILE_H_8FE1BEFB_D32A_4ade_9463_4A3D34ACB7F5
