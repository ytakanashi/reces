//PrivateProfile.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r32 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _PRIVATEPROFILE_H_AF26760D_77B2_4C9D_9B67_4F0897D4AAF0
#define _PRIVATEPROFILE_H_AF26760D_77B2_4C9D_9B67_4F0897D4AAF0

#include"FileInfo.h"

enum MODE{
	MODE_RECOMPRESS=1<<0,
	MODE_COMPRESS=1<<1,
	MODE_EXTRACT=1<<2,
	MODE_LIST=1<<3,
	MODE_TEST=1<<4,
	MODE_SENDCOMMANDS=1<<5,
	MODE_VERSION=1<<6,
	MODE_SETTINGS=1<<7,
	MODE_DELETE=1<<8,
	MODE_RENAME=1<<9,
};

struct NODISPLAY{
	//進捗状況を表示しない
	bool no_information;
	//ログを表示しない
	bool no_log;
	//パスワードを表示しない
	bool no_password;
	//エラーメッセージを表示しない
	bool no_errmsg;
	NODISPLAY():no_information(false),no_log(false),no_password(false),no_errmsg(false){}
};

struct REMOVEREDUNDANTDIR{
	//二重ディレクトリを防ぐ
	bool double_dir;
	//同名の二重ディレクトリを防ぐ
	bool same_dir;
	//ファイル単体の場合作成しない
	bool only_file;
	REMOVEREDUNDANTDIR():double_dir(false),only_file(false),same_dir(false){}
};

struct OMITNUMBERANDSYMBOL{
	//ディレクトリ名末尾の数字を削除
	bool number;
	//ディレクトリ名末尾の記号を削除
	bool symbol;
	OMITNUMBERANDSYMBOL():number(false),symbol(false){}
};

struct CREATEDIROPTIMIZATION{
	//更新日時を書庫と同じにする
	bool copy_timestamp;
	//不要なディレクトリを作成しない
	REMOVEREDUNDANTDIR remove_redundant_dir;
	//出力ディレクトリ末尾の数字や記号を削除
	OMITNUMBERANDSYMBOL omit_number_and_symbol;
	CREATEDIROPTIMIZATION():copy_timestamp(false),remove_redundant_dir(),omit_number_and_symbol(){}
};

enum RMSRC{
	RMSRC_DISABLE=1<<0,
	RMSRC_RECYCLEBIN=1<<1,
	RMSRC_REMOVE=1<<2
};

struct RUNCOMMAND{
	//対話形式で指定
	bool interactive;
	//コマンド
	tstring command;
	RUNCOMMAND():interactive(false),command(){}
	bool disable()const{return !interactive&&command.empty();}
};

struct GENERAL{
	//バックグラウンドで動作
	bool background_mode;
	//処理終了後ウインドウを閉じる
	bool quit;
	//ディレクトリ階層を無視して圧縮/解凍
	bool ignore_directory_structures;
	//指定したライブラリ名
	tstring selected_library_name;
	//指定したライブラリのプレフィックス(msのみ)
	tstring selected_library_prefix;
	//処理対象フィルタ
	fileinfo::FILEFILTER filefilter;
	//処理対象除外フィルタ
	fileinfo::FILEFILTER file_ex_filter;
	//パスワード
	tstring password;
	std::list<tstring> password_list;
	//出力先ディレクトリ
	tstring output_dir;
	//カレントディレクトリ基準で'/od','/of'の相対パスを処理
	bool default_base_dir;
	//出力ファイルが重複する場合リネーム
	bool auto_rename;
	//ソースを削除
	RMSRC remove_source;
	//アーカイバに送る文字コード(現在7-zip32.dllのみ対象)
	unsigned int arc_codepage;
	//パスワードリストファイル、リストファイルの文字コード
	sslib::File::CODEPAGE list_codepage;
	//文字をANSI(sjis)で出力
	bool ansi_stdout;
	//ユーザ独自のパラメータ
	tstring custom_param;
	//spiがあるディレクトリ
	tstring spi_dir;
	//b2eがあるディレクトリ
	tstring b2e_dir;
	//wcxがあるディレクトリ
	tstring wcx_dir;

	//初期化
	GENERAL():
		background_mode(false),
		quit(true),
		ignore_directory_structures(false),
		selected_library_name(),
		selected_library_prefix(),
		filefilter(),
		file_ex_filter(),
		password(),
		password_list(),
		output_dir(),
		default_base_dir(false),
		auto_rename(false),
		remove_source(RMSRC_DISABLE),
		arc_codepage(0),
		list_codepage(sslib::File::SJIS),
		ansi_stdout(false),
		custom_param(),
		spi_dir(),
		b2e_dir(),
		wcx_dir(){}
};

struct RECOMPRESS{
	//新しいパスワード
	tstring new_password;
	//コマンド実行
	RUNCOMMAND run_command;
	RECOMPRESS():new_password(),run_command(){}
};

struct B2E{
	//圧縮形式
	tstring format;
	//圧縮メソッド
	tstring method;
	//自己解凍形式指定
	bool sfx;
	B2E():format(),method(),sfx(false){}
};

struct COMPRESS{
	//圧縮形式
	tstring compression_type;
	//個別圧縮
	bool each_file;
	//書庫新規作成
	bool create_new;
	//書庫強制新規作成
	//上書き先を予め削除して作成
	bool force_create_new;
	//基底ディレクトリを含まない
	//解凍の場合-1で共通パスをすべて除く
	int exclude_base_dir;
	//圧縮率
	int compression_level;
	//書庫分割数値
	tstring split_value;
	//出力ファイル名
	tstring output_file;
	//'/of'以下の引数をそのまま使用(拡張子を付加しない)
	bool raw_file_name;
	//更新日時を元書庫と同じにする
	bool copy_timestamp;
	//対象ディレクトリを再帰的検索
	bool recursive;
	//圧縮用b2eスクリプト
	B2E b2e;
	//元ファイルの拡張子を保持
	bool keep_extension;

	COMPRESS():
		compression_type(_T("zip")),
		each_file(false),
		create_new(false),
		force_create_new(false),
		exclude_base_dir(0),
		compression_level(-1),
		split_value(),
		output_file(),
		raw_file_name(false),
		copy_timestamp(false),
		recursive(false),
		b2e(),
		keep_extension(false){}
};

struct EXTRACT{
	//ファイル名が始まるIndex
	int index;
	//ディレクトリを作成する
	bool create_dir;
	//パスワードリストファイル
	tstring password_list_path;
	//'/c'最適化
	CREATEDIROPTIMIZATION create_dir_optimization;
	//ディレクトリのタイムスタンプを復元する
	bool directory_timestamp;
	EXTRACT():index(0),
		create_dir(false),
		password_list_path(),
		create_dir_optimization(),
		directory_timestamp(false){}
};

struct OUTPUTFILELIST{
	//FindFirst()/FindNext()/GetFileName()で出力する
	bool api_mode;

	OUTPUTFILELIST():api_mode(true){}
};

struct RENAME{
	//置換パターン
	typedef std::pair<tstring,tstring> pattern;
	std::list<pattern> pattern_list;
	//正規表現
	bool regex;

	RENAME():pattern_list(),regex(false){}
};

struct CONFIG{
	MODE mode;
	GENERAL general;
	//非表示
	NODISPLAY no_display;
	//再圧縮
	RECOMPRESS recompress;
	//圧縮
	COMPRESS compress;
	//解凍
	EXTRACT extract;
	//一覧出力
	OUTPUTFILELIST output_file_list;
	//リネーム
	RENAME rename;

	CONFIG():
		mode(MODE_RECOMPRESS),
		general(),
		no_display(),
		recompress(),
		compress(),
		extract(),
		output_file_list(),
		rename(){}
};

class Config:public sslib::CfgFile{
private:
	static CONFIG m_cfg;
	static const CONFIG m_default_cfg;

private:
	template<typename T>
	void write(const TCHAR* section,const TCHAR* key,const T value,bool diff){
		if(diff){
			setData(section,key,value);
		}else{
			removeKey(section,key);
		}

		//キーが一つもなければセクションを削除
		if(isEmptySection(section)){
			removeSection(section);
		}
	}
	void write(const TCHAR* section,const TCHAR* key,const tstring& value,bool diff){
		return write(section,key,value.c_str(),diff);
	}
	void write(const TCHAR* section,const TCHAR* key,const long long value,bool diff){
		return write(section,key,sslib::format(_T("%I64d"),value).c_str(),diff);
	}

public:
	Config(){}
	~Config(){}
	//cfgファイルへ書き込む
	bool save();
	//cfgファイルから読み込む
	bool load();

	inline CONFIG& cfg(){return m_cfg;}
};

#endif //_PRIVATEPROFILE_H_AF26760D_77B2_4C9D_9B67_4F0897D4AAF0
