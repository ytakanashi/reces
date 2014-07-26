//DropFiles.cpp
//ドラッグ&ドロップされたファイルの処理

#include"../../StdAfx.h"
#include"../sslib.h"

namespace sslib{

//ファイルパスを取得
tstring DropFiles::getFile(int index){
	std::vector<TCHAR> file_path(MAX_PATHW);

	::DragQueryFile(handle(),index,&file_path[0],file_path.size());
	return &file_path[0];
}



//namespace sslib
}
