//ApiHook.cpp
//IAT書き換えによるApiHook

#include"../StdAfx.h"
#include"ApiHook.h"
#include<imagehlp.h>

namespace sslib{
namespace apihook{

void replace(PCSTR callee_mod_name,PROC current_pfn,PROC new_pfn,HMODULE mod_caller){
	//モジュールのインポートセクションのアドレスを取得する
	ULONG data_size;

	PIMAGE_IMPORT_DESCRIPTOR import_descriptor=NULL;
	import_descriptor=static_cast<PIMAGE_IMPORT_DESCRIPTOR>(::ImageDirectoryEntryToData(mod_caller,
																				   true,
																				   IMAGE_DIRECTORY_ENTRY_IMPORT,
																				   &data_size));

	if(import_descriptor==NULL){
		//このモジュールにはインポートセクションがない
		//もしくは現在ロードされていない
		return;
	}

	//呼び出し先の関数への参照が含まれているインポート記述子を検索する
	for(;import_descriptor->Name;import_descriptor++){
		PSTR mod_name=reinterpret_cast<PSTR>(reinterpret_cast<PBYTE>(mod_caller)+import_descriptor->Name);

		if(lstrcmpiA(mod_name,callee_mod_name)==0){
			//呼び出し元のIATで呼び出し先関数を取得する
			PIMAGE_THUNK_DATA thunk_data=reinterpret_cast<PIMAGE_THUNK_DATA>(reinterpret_cast<PBYTE>(mod_caller)+import_descriptor->FirstThunk);

			//現在の関数アドレスを新しい関数アドレスと置き換える
			for(;thunk_data->u1.Function;thunk_data++){
				//関数アドレスのアドレスを取得する
				PROC* ppfn=reinterpret_cast<PROC*>(&thunk_data->u1.Function);
				DWORD protect_bak=0;

				if(*ppfn!=current_pfn)continue;

				//保護状態変更
				if(!VirtualProtect(ppfn,sizeof(PROC),PAGE_READWRITE,&protect_bak)){
					return;
				}
				::WriteProcessMemory(::GetCurrentProcess(),ppfn,&new_pfn,sizeof(new_pfn),NULL);
				//保護状態を元に戻す
				::VirtualProtect(ppfn,sizeof(PROC),protect_bak,&protect_bak);
				return;
			}
		}
	}
}

//namespace apihook
}
//namespace sslib
}
