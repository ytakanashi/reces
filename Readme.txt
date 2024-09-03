_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
【 ソフト名 】　reces
【バージョン】　0.00r34
【 製作者名 】　x@rgs
【 動作環境 】　Windows XP以降
【 製作言語 】　C++
【ｿﾌﾄｳｪｱ種別】　フリーソフトウェア
【 配布条件 】　NYSL Version 0.9982
【  連絡先  】	Y.R.Takanashi@gmail.com
【  サイト  】	http://frostmoon.sakura.ne.jp/
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
●はじめに
 この度は、「reces」(以下、本ソフトとも)をダウンロードしていただき、ありがとうございます。
 以下、稚拙な文章が続きますが、本ソフトは予告無しに「大幅な仕様変更」を「頻繁に」行いますので、
 バージョンアップごとに、目を通していただけると幸いです。



●説明
 「reces」は、コマンドプロンプト上で書庫の「再圧縮」「圧縮」「解凍」「内容一覧表示」「検査」「内容削除」「リネーム」を行うソフトウェアです。
 文字化け防止機能やフィルタ(指定文字列、正規表現、更新日時)機能等々を備えています。
 7zやzip書庫内のファイル名を正規表現を用いてリネームすることもできます。
 惹句は「多機能で操作は難解!」



●特徴
 [対応書庫]
 ・zip、rar、7z、lzhファイルなど主要な書庫をサポート。
 ・Susie Plug-in(*.spi)で様々な書庫に対応可能。
 ・Total Commander Plugin(*.wcx)で更に多くの書庫に対応可能。
 ・b2eスクリプト(*.b2e)で書庫操作以外の処理も可能。
 ・UNICODE文字、4GB以上のファイルに対応。*1
 ・長いパス(MAX_PATH以上)のzip,7zファイルの処理が可能。
 ・ヘッダ暗号化(ファイル名暗号化)の7zやrarをサポート。

 [書庫処理]
 ・ファイル名の文字化けが起きにくい。
 ・zipファイルのコードページを指定して解凍することが出来る。
 ・7zやzipファイル内のファイル名を正規表現を用いてリネームすることが出来る。
 ・プログレスバーを自前で表示し、無駄なウインドウを表示させない。
 ・ディレクトリのタイムスタンプ復元が可能。 *2
 ・ファイルの分割/結合が出来る。
 ・二重ディレクトリ防止機能あり。
 ・基底ディレクトリを含まない圧縮が可能。
 ・処理後、元ファイルをごみ箱へ送ることが出来る。

 [その他機能]
 ・引数のほか、リストファイルや標準入力から処理対象を指定可能。
 ・指定した文字、正規表現、サイズ、日付などの各種フィルタを指定することが出来る。
 ・設定ファイルへの書き出し/読み込みに対応。
 ・パスワードリストを用いた総当たり解凍が出来る。
 ・レジストリは一切使用しない。 *3

   *1 対応している書庫ライブラリに限ります。
   *2 タイムスタンプが保持され、かつ取得出来る場合に限ります。
      出来ない場合、配下のディレクトリやファイルから取得します。(r16より、/DIRTSオプションで)
   *3 ライブラリ側が使用する場合もありますのでご注意ください。



●ファイル構成
 7-zip32.dll             ----- 7-zip32.dll文字化け対策版
 NYSL_withfaq.TXT        ----- ライセンスファイル
 Readme.txt              ----- このファイル
 reces.exe               ----- reces本体(32bit版)ファイル
 Src.7z                  ----- recesソースファイル

 ./x64
    7-zip64.dll          ----- 7-zip64.dll文字化け対策版
    b2e64.dll            ----- b2e32.dll(64bit版)ファイル
    reces.exe            ----- reces本体(64bit版)ファイル

 ./b2e
 ./x64/b2e
    exe.msi.b2e          ----- Universal Extractorを使用し、インストーラを解凍するb2eスクリプトファイル
    mp3.b2e              ----- LAMEを使用し、mp3ファイルのエンコード・デコードを行うb2eスクリプトファイル



●インストール
 1.「reces000r34.zip」を適当なディレクトリに解凍してください。
 2.以下の各書庫操作ライブラリをパスの通ったディレクトリへコピーしてください。
   *.spi、*.wcxについては、別途ディレクトリを用意することをお勧めします。

    == ライブラリ ================================================================================
   ||      32bit版      |       64bit版        ||            対応            ||      入手先      ||
   ||-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*||-*-*-*-*-*-*-*-*-*||
   ||    7-zip32.dll    |     7-zip64.dll      ||  再圧縮、圧縮、解凍、削除  ||       同梱       ||
   ||    tar32.dll      |     tar64.dll        ||  再圧縮、圧縮、解凍        ||  *1 64bit版は *2 ||
   ||    UNLHA32.DLL    |     ------------     ||  再圧縮、圧縮、解凍、削除  ||        *1        ||
   ||    unrar32.dll    |     unrar64j.dll     ||  解凍                      ||  *1 64bit版は *5 ||
   ||    UnIso32.dll    |     ------------     ||  解凍                      ||        *1        ||
   ||    XacRett.dll    |     ------------     ||  解凍                      ||        *3        ||
   ||  *.b2e+b2e32.dll  |   *.b2e+b2e64.dll    ||  再圧縮、圧縮、解凍        || *1 64bit版は同梱 ||
   ||       *.spi       |        *.sph         ||  解凍                      ||                  ||
   ||                   |  *.spi+ZBYPASSA.SPH  ||                            ||        *4        ||
   ||       *.wcx       |    *.wcx,*.wcx64     ||  解凍                      ||                  ||
   ||      -------      |     UNBYPASS.DLL     ||  再圧縮、圧縮、解凍        ||        *4        ||
    ==============================================================================================

   *1
      統合アーカイバプロジェクト( http://www.csdinc.co.jp/archiver/ )
   *2
      綾川的趣味之接続集( https://ayakawa.x0.com/ )
      GitHub - Claybird/tar32( https://github.com/Claybird/tar32 )
   *3
      Bonty's HomePage( http://hp.vector.co.jp/authors/VA030681/index.htm )
   *4
      TORO's Library( http://toro.d.dooo.jp/ )
   *5
      Ru^3 Honpo( https://rururutan.github.io/ )

 ・7-zip32.dllについて、現在統合アーカイバプロジェクトで配布されている9.22.00.02は、
   一部ファイル名が文字化けしてしまいます。
   また、書庫内ファイルのリネーム処理を行うことができません。
   そこでrecesでは、「7-zip32.dll/7-zip64.dll文字化け対策版」を同梱しています。
   最新版はFrost Moon Project( http://frostmoon.sakura.ne.jp/ )にて入手することが出来ます。

 ・rar書庫を取り扱うにあたり、必ず「unrar.dll」「unrar64.dll」を最新版にバージョンアップしてください。
   RARLAB( http://www.rarlab.com/ )の「RAR」->「Extras」->「UNRAR.dll」からダウンロードすることが出来ます。

 ・「7z.dll対応版7-zip32.dll/7-zip64.dll」( http://frostmoon.sakura.ne.jp/ )に対応しています。
   reces x64版ではUNLHA32.DLLやXacRett.dllが使用できないため、導入をお勧めします。
   1.「7-Zip x64版」( http://www.7-zip.org/ )をインストール。
   2.「7-zip32.dll/7-zip64.dll/7z.dll文字化け対策版」( http://frostmoon.sakura.ne.jp/ )をダウンロード。
   3.「x64\7z.dll」を7-Zipインストールディレクトリにコピー。
     (以下の3,4はインストーラ版であれば不要です)
   4.「7-zip32-full\7-zip64.dll」をパスの通ったディレクトリにコピー。
   以下の形式に対応しています。
      7z, XZ, BZIP2, GZIP, TAR, ZIP
      AR, ARJ, CAB, CHM, CPIO, CramFS, DMG, EXT, FAT, GPT, HFS, IHEX, ISO, LZH, LZMA, MBR, MSI, NSIS, NTFS,
      QCOW2, RAR, RPM, SquashFS, UDF, UEFI, VDI, VHD, VMDK, WIM, XAR, Z

 ・ライブラリの優先順位は、
      UNLHA32.dll / UNLHA32.dll+UNBYPASS.DLL
      unrar32.dll / unrar64j.dll
      tar32.dll   / tar64.dll
      7-zip32.dll / 7-zip64.dll
      UnIso32.dll
      XacRett.dll
      Susie Plug-in(*.spi / *.sph, *.spi + ZBYPASSA.SPH)
      Total Commander Plugin(*.wcx / *.wcx64)
      b2eスクリプト(*.b2e) + b2e32.dll / *.b2e + b2e64.dll
   です。

 ・Susie Plug-inのデフォルトの読込先はreces.exeがあるディレクトリです。
   別のディレクトリを指定するには「/Ds」オプションを使用してください。

 ・Total Commander Pluginのデフォルトの読込先はreces.exeがあるディレクトリです。
   別のディレクトリを指定するには「/Dw」オプションを使用してください。

 ・b2eスクリプトのデフォルトの読み込み先は「b2e32.dllのあるディレクトリ\b2e」です。
   別のディレクトリを指定するには「/Db」オプションを使用してください。

 ・上記統合アーカイバライブラリをすべて導入すると、以下の拡張子に対応することが出来ます。
   (Susie Plug-inやTotal Commander Plugin、b2eスクリプトの導入により更に対応させることが出来ます。)

      lzh,lha,lzs
      rar
      zip,7z,jar
      tar,tgz,gz,tar.gz,bz2,tbz,tbz2,tar.bz2,xz,txz,tar.xz,lzma,tlz,tlzma,tar.lzma,zst,tar.zst,tzst,tzs,tzstd
      Z,taz,tz,tar.Z,cpio,cpgz,a,lib,rpm,deb
      iso
      ace,arc,arg,arj,asd,b2a,b64,bel,bin,boo,bza,gza,C,cab,cpt,dmg,dz,F,xF,gca,hqx,imp,ish,jak,ntx,pit,pak,wad,pff,rez,sp,spl,uue,xxe,zac,zoo

 3.「reces.exe」「*.dll」をパスの通ったディレクトリへコピーしてください。
 4.コマンドプロンプトを開き、
      reces /mv
   と入力し、各ライブラリが正しくインストール出来ているか確認してください。


●アンインストール
 レジストリは一切使用しないため、ディレクトリごと、若しくはコピーしたファイルを削除してください。
 (レジストリをライブラリ側が使用する場合がありますのでご注意ください。)



●使用方法
 ・「reces [/<options>...] [/@<listfiles>...] [<files>...]」
    に従い実行します。
 ・「<>」で囲まれた項目は必須です。省略できません。
      例: /X<pattern...>
          (「...」は複数指定可能の意味)
       -> /X*.db
 ・「[]」で囲まれた項目は必須ではありません。省略可能です。
      例: /me[library]
       -> /me
      例: /d[1]
       -> /d
 ・各ライブラリのドキュメントファイルも併せてご覧ください。
 ・b2eスクリプト使用時のオプションは大幅に制限されます。
 ・実行中にCtrl+Cで処理を中断します。
 ・実行中にPauseキーで処理を一時停止します。
   Enterキーで再開します。
 ・「|」や「^」等々特殊な記号を引数に含む場合、エスケープ文字「^」を前に挿入する必要があります。
      例: (^|\d+)
       -> (^^^|\d+)

 [オプション]
 /m<r|R|c|C|e|E|l|L|t|d|n|N|s|S|v>
   ・動作モードを指定します。省略すると、/mrzipとして処理します。
        /mr[type|@[option]][:library]        (通常)
          ・再圧縮を行います。解凍後に[type]に圧縮します。
          ・[type]を省略すると、zipとして処理します。
          ・[type]については下記「圧縮形式」リストを参照してください。
          ・'@'が指定された場合、入力書庫と同じ「形式」で処理します。
            圧縮に対応していない形式であればzipとして処理します。
          ・'@'単体ではパスワード付、ヘッダ暗号化、自己解凍書庫が作成出来ません。
            「/mr@pw」「/mr@he」「/mr@sfx」の様にオプション文字列を付加してください。
          ・解凍に用いるライブラリを[library]で指定することが出来ます。
               例: /mrzip:XacRett
                   (XacRett.dllで解凍)
          ・ファイルはデフォルトで元書庫と同階層のディレクトリに出力します。
          ・出力書庫名が元書庫と同名の場合、追加圧縮となります。
            新規作成には「/N」オプションを使用します。
        /mr<b2e|b2esfx>:<format|@>[:method]  (b2eスクリプト)
          ・解凍後、b2eスクリプトを使用して圧縮を行います。
          ・詳しくは「/mc<b2e|b2esfx>:<format>[:method]」を参考にしてください。
          ・'@'が指定された場合の動作は通常と同様です。
            圧縮に対応していない形式であればzip(7-zip32.dll)として処理します。


        /mR[type|@[option]][:library]        (通常)
        /mR<b2e|b2esfx>:<format|@>[:method]  (b2eスクリプト)
          ・ディレクトリ階層を無視した再圧縮を行います。その他は「/mr」と同様です。
          ・「/mE」と「/mc」の組み合わせと同様の処理を行います。
          ・重複するファイルは自動的に上書きされます。
          ・詳しくは「/mC」を参考にしてください。


        /mc[type]                          (通常)
          ・圧縮を行います。[type]を省略すると、zipとして処理します。
               例: /mc7z
                   (7z書庫を作成)
          ・ファイルはデフォルトで対象ファイル/ディレクトリと同階層のディレクトリに出力します。
          ・個別圧縮には「/e」オプションを使用します。
          ・入力対象のディレクトリを再帰的検索するには「/R」オプションを使用します。
        /mc<b2e|b2esfx>:<format>[:method]  (b2eスクリプト)
          ・「b2e」指定で「encode:」セクションを使用します。
               例: /mcb2e:7z
                   (7z書庫をb2eスクリプトで作成)
          ・「b2esfx」指定で「sfx:」セクションを使用します。
               例: /mcb2esfx:7z
                   (7z自己解凍書庫をb2eスクリプトで作成)
          ・「add:」セクションには対応していません。
          ・<method>を省略するとb2eスクリプト内でデフォルト指定されたメソッドで圧縮します。
          ・ファイルを個別に処理する場合は「/e」が必須となります。

        /mC[type]
          ・ディレクトリ階層を無視した圧縮を行います。
          ・重複するファイルがあると失敗します。
          ・b2eスクリプトは対応していません。

          ・「ディレクトリ階層を無視」するとは。
              DIR
              ├─foo
              │  └─bar
              │      │  main.cpp
              │      │
              │      └─bin
              │          └─Release
              │                  main.exe
              │
              └─baz
                  │  Readme.txt
                  │
                  └─qux
                          quux.jpg

              例えば、上記ディレクトリ階層の状態で
              「reces /mC foo baz」
              とすると、以下内容で圧縮されます。

              foo.zip
                main.cpp
                main.exe
                Readme.txt
                quux.jpg


        /me[library]
          ・解凍を行います。[library]で処理を行う書庫操作ライブラリを指定することが出来ます。
              例: /meXacRett
                  (XacRett.dllで解凍)
              例: /meb2e32
                  (b2eスクリプトを用いて解凍)
          ・分割ファイルの解凍では、先頭ファイルのみを指定してください。
          ・入力対象にディレクトリを指定すると、ファイルを再帰的検索します。
          ・デフォルトで元書庫と同階層のディレクトリに解凍します。

        /mE[library]
          ・ディレクトリ階層を無視した解凍を行います。その他は「/me」と同様です。
          ・重複するファイルは自動的に上書きされます。
          ・「ディレクトリ階層を無視」については、「/mC」を参考にしてください。


        /ml[library]
          ・書庫内容の一覧出力を行います。[library]で処理を行う書庫操作ライブラリを指定することが出来ます。
          ・名前のみを表示します。
          ・統合アーカイバライブラリはFindFirst()/FindNext()/GetFileName()を用いて出力します。
          ・Susie Plug-in、Total Commander Pluginはファイル名をreces側で取得して出力します。
          ・ディレクトリ、隠し、読み取り専用などの属性があれば、それぞれ色付き文字で出力されます。
          ・入力対象にディレクトリを指定すると、ファイルを再帰的検索します。
          ・b2e32.dllの場合、動作はb2eスクリプトに依存します。

        /mL[library]
          ・書庫内容の一覧出力を行います。
          ・各書庫操作ライブラリのコマンドを用いて出力します。
          ・各書庫操作ライブラリの実装に依存しますが、多くは冗長表示(verbose)になります。
          ・tar32.dllではフィルタが適用されません。
          ・XacRett.dllは一覧出力コマンドが実装されていないため、
            更新日時、属性、サイズ、名前をreces側で取得して表示します。
          ・Susie Plug-in、Total Commander Pluginもほぼ同様に、
            更新日時、サイズ、名前をreces側で取得して表示します。
          ・b2e32.dllの場合、動作はb2eスクリプトに依存します。


        /mt[library]
          ・書庫をテストします。
          ・7-zip32.dll、UNLHA32.dll、UnIso32.dll以外の統合アーカイバライブラリはCheckArchive()の値を返すだけです。
          ・統合アーカイバライブラリについて、使用するライブラリを指定しなければ、CheckArchive()でライブラリを決定します。
          ・Susie Plug-inは対応していません。
          ・Total Commander Pluginは対応しています。ただし、使用するライブラリを指定する必要があります。
          ・b2eスクリプトは対応していません。


        /md[library]
          ・書庫からファイルを削除します。
          ・現在7-zip32.dll、UNLHA32.dllのみの対応となります。
          ・削除したいファイルはフィルタ「/I」「/X」等々で指定してください。
          ・フィルタ系以外で対応しているオプションは「/t」「/C」のみです。
          ・その他オプション指定は無視されます。
          ・分割ファイルには対応していません。
          ・処理を行う前に、書庫のバックアップをとることをおすすめします。


        /mn[target][:replacement]...
          ・書庫内ファイルをリネームします。
          ・現在7-zip32.dllのみの対応となります。
          ・対応書庫は、7zとzipのみです。
          ・ワイルドカードは使用できません。(「/mN」で正規表現を使うことができます。)
          ・「/mn.jpeg:.jpg;.htm:.html」の様に、「;」を用いて複数指定することが出来ます。
          ・[:replacement]を省略すると、<target>で指定した文字列を削除します。
          ・[target][:replacement]を省略すると、リネーム処理前に入力を求めます。
          ・検索文字列に何も入力せずにEnterで処理を開始します。
          ・書庫内ファイル名はパス付きで処理されます。
            以下の書庫の場合、「bar\001.jpg」が対象文字列となります。
                foo.zip
                 └─bar
                     └─001.jpg
            実行前に、「/ml」で書庫内ファイル名を確認することをおすすめします。
          ・処理によっては不正な書庫を作成してしまいますので、バックアップをとることをおすすめします。
          ・分割ファイルには対応していません。

        /mN[regex_pattern][:replacement]...
          ・正規表現を用いて書庫内ファイルをリネームします。
          ・[regex_pattern][:replacement]を省略すると、リネーム処理前に入力を求めます。
            特殊文字「^」「|」等々のエスケープは不要ですので、引数が煩雑になる場合におすすめです。
          ・検索文字列に何も入力せずにEnterで処理を開始します。
          ・[replacement]に後方参照として変数$nを使用することができます。
            詳細についてはMSDNの「Regular Expressions (C++)」( https://msdn.microsoft.com/en-us/library/bb982727.aspx )をご覧ください。


        /ms[library][:prefix]
          ・各統合アーカイバライブラリを直接コマンドを送信します。
            [library]を省略すると、7-zip32として処理します。
          ・詳細については[直接操作]をご覧ください。
          ・/ms[library][:prefix]以降の文字列をコマンドとして送ります。
          ・ワイルドカードは展開されません。
          ・tar32.dllを操作するには、「--inverse-procresult=1」オプションが必要です。
          ・Susie Plug-in、Total Commander Pluginは対応していません。
          ・recesで対応していないアーカイバライブラリは[prefix]を指定することで扱うことが出来ます。
               例: /mscab32.dll:Cab
                   (cab32.dllを使用)


        /mS<library>[:prefix]
          ・各書庫操作ライブラリの設定ダイアログを表示します。<library>は省略出来ません。
          ・ダイアログ上の操作についてはライブラリのドキュメントを参照してください。
          ・ライブラリによりレジストリ操作が行われる可能性があります。
          ・正しく実装されていないライブラリに対して操作を行うと、強制終了する場合があります。
          ・recesで対応していないアーカイバライブラリは[prefix]を指定することで扱うことが出来ます。
               例: /mSunzip32.dll:UnZip
                   (unzip32.dllの設定を行う)


        /mv [libraries...|b2e|cp]
          ・対応している各書庫操作ライブラリのバージョンを表示します。
          ・指定したファイルのバージョンを表示することも出来ます。
               例: /mv 7-zip32.dll
                   (7-zip32.dllのバージョンを表示)
          ・「/mv b2e」とすると圧縮で使用できるb2eスクリプトの「<format>:<method>」の組み合わせを表示します。
          ・「/mv cp」とすると「/C」で指定できる文字コードの一覧を表示します。


           == '@'のオプション ==============================================
          ||   option   |           作成する書庫           ||   対応形式   ||
          ||-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-||
          || pw         |  パスワード付書庫                ||   7z,zip     ||
          || he         |  ヘッダ暗号化書庫書庫            ||   7z         ||
          || sfx        |  自己解凍書庫                    ||   7z,lzh     ||
          || pwsfx      |  パスワード付自己解凍書庫        ||   7z         ||
          || hesfx      |  ヘッダ暗号化自己解凍書庫書庫    ||   7z         ||
           =================================================================


           == 圧縮形式 ====================================================
          ||    type    |           作成する書庫           ||    拡張子    ||
          ||-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-||
          || lzh        |  lzh書庫                         ||   .lzh       ||
          || lzhsfx     |  lzh自己解凍書庫                 ||   .exe       ||
          || zip        |  zip書庫                         ||   .zip       ||
          || zippw      |  パスワード付zip書庫             ||   .zip       ||
          || 7z         |  7z書庫                          ||   .7z        ||
          || 7zpw       |  パスワード付7z書庫              ||   .7z        ||
          || 7zhe       |  ヘッダ暗号化7z書庫              ||   .7z        ||
          || 7zsfx      |  7z自己解凍書庫                  ||   .exe       ||
          || 7zpwsfx    |  パスワード付7z自己解凍書庫      ||   .exe       ||
          || 7zhesfx    |  ヘッダ暗号化書庫7z自己解凍書庫  ||   .exe       ||
          || tar        |  tar書庫                         ||   .tar       ||
          || gz         |  gz書庫                          ||   .gz        ||
          || tgz        |  tar+gz書庫                      ||   .tgz       ||
          || targzip    |  tar+gz書庫                      ||   .tar.gz    ||
          || bz2        |  bzip2書庫                       ||   .bz2       ||
          || tbz        |  tar+bzip2書庫                   ||   .tbz       ||
          || tarbzip2   |  tar+bzip2書庫                   ||   .tar.bz2   ||
          || xz         |  xz書庫                          ||   .xz        ||
          || txz        |  tar+xz書庫                      ||   .txz       ||
          || tarxz      |  tar+xz書庫                      ||   .tar.xz    ||
          || lzma       |  lzma書庫                        ||   .lzma      ||
          || tlz        |  tar+lzma書庫                    ||   .tlz       ||
          || tlzma      |  tar+lzma書庫                    ||   .tlzma     ||
          || tarlzma    |  tar+lzma書庫                    ||   .tar.lzma  ||
          || zst        |  zstd書庫                        ||   .zst       ||
          || tzs        |  tar+zstd書庫                    ||   .tzs       ||
          || tzst       |  tar+zsdt書庫                    ||   .tzst      ||
          || tzstd      |  tar+zstd書庫                    ||   .tzstd     ||
          || tarzst     |  tar+zstd書庫                    ||   .tar.zst   ||
           =================================================================


 /n<options...>
   ・各情報を表示しません。
   ・オプションは「/nlp」の様に複数指定が可能です。
        /ni
          ・進捗状況を非表示にします。
          ・プログレスバーも表示されません。

        /nl
          ・ログを非表示にします。

        /np
          ・パスワード入力の際、パスワードを非表示にします。

        /ne
          ・エラーメッセージを非表示にします。

        /na
          ・進捗状況、ログ、タイトル、パスワードを非表示にします。
          ・エラーメッセージは表示されます。
          ・パスワード入力メッセージも表示されます。


 /c[options...]
   ・ディレクトリを新規作成します。 {mr/me}
   ・オプションは「/c12」の様に複数指定が可能です。
   ・作成するディレクトリ名を指定したい場合はこのオプションではなく、「/od」オプションを使用します。
   ・b2eスクリプトでは「/c1」「/c2」「/c3」は使用できません。
        /c1
          ・ファイルが一つであれば、ディレクトリを作成しません。

        /c2
          ・二重ディレクトリになるようであれば、ディレクトリを作成しません。

        /c3
          ・同名の二重ディレクトリになるようであれば、ディレクトリを作成しません。
          ・7-Zipの「Eliminate duplication of root folder」(ルートフォルダーの重複を回避)オプションに相当します。

            例えば、以下の構造の書庫では「/c2」「/c3」共にディレクトリは作成されません。
                foo.zip
                 └─foo
                     └─001.jpg

            しかし、以下の構造の書庫では「/c3」ではディレクトリが作成されます。
                foo.zip
                 └─bar
                     └─001.jpg


        /cn
          ・ディレクトリ名末尾から数字を取り除きます。

        /cs
          ・ディレクトリ名末尾から記号を取り除きます。

        /ct
          ・ディレクトリの更新日時を書庫と同じにします。


 /d[1|2]
   ・処理終了後、入力ファイルを削除します。 {mr/mc/me}
        /d[1]
          ・ごみ箱へ送ります。

        /d2
          ・完全に削除します。
          ・recesでは復元できませんので取り扱いには十分ご注意ください。


 /e
   ・ファイル/ディレクトリを一つずつ圧縮します。 {mr/mc}


 /eb
   ・作成する書庫から基底ディレクトリを除外します。 {mr/mc}
   ・Rar.exeの-ep1 -r DIR\、Archon2の-o1オプションに相当します。
   ・例えば、下記ディレクトリ階層の書庫について、
     「reces /mc DIR」とするとDIRを含めた書庫が作成されますが、
     「reces /mc /eb DIR」とすると「001.jpg」「002.jpg」のみを含む書庫となります。

      DIR
       ├─001.jpg
       └─002.jpg

 /eb<[level]|x>
   ・共通するパスを除外して解凍します。 {mr/me}
   ・解凍レンジの「パス情報を最適化して展開する」や、tarの「--strip-components」に相当します。
   ・levelで除外するパスの数を指定します。
        例: /eb2
            (パスを2階層分除外)
   ・「/ebx」で共通するパス全てを除外します。
   ・統合アーカイバライブラリでは、解凍後にパスの除外を行います。
     Susie Plug-in、Total Commander Pluginであれば解凍時に行います。
   ・「/c」オプション単体との併用を強く推奨します。

    foo.zip
     └─DIR01
         └─DIR02
             └─DIR03
                 └─DIR04
                     ├─DIR05
                     │  └─FILE01
                     ├─FILE02
                     └─FILE03

     例えば、上記ディレクトリ階層の書庫について、

     「/eb2」とすると、
                 DIR03
                 └─DIR04
                     ├─DIR05
                     │  └─FILE01
                     ├─FILE02
                     └─FILE03
     となり、

     「/ebx」とすると、
                     DIR04
                     ├─DIR05
                     │  └─FILE01
                     ├─FILE02
                     └─FILE03
     が解凍されます。


 /D<d|b|s|w><directory>
   ・特殊なディレクトリを指定します。
        /Dd<directory>
          ・統合アーカイバライブラリ(*.dll)のあるディレクトリを指定します。 {mr/mc/me/ml/mt/ms/mv}
          ・システムディレクトリに*.dllがあれば不要ですが、カレントディレクトリにある7-zip32.dllを利用する場合、
　　　　　　7-zip32.dll側(19.00以降)でSetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32|LOAD_LIBRARY_SEARCH_USER_DIRS)が呼び出されるため、
　　　　　　再圧縮や連続解凍に失敗してしまいます。
　　　　　　このオプションを活用し、「/Dd.」とすることで、カレントディレクトリにある7-zip32.dllで処理することができます。

        /Db<directory>
          ・b2eスクリプト(*.b2e)のあるディレクトリを指定します。 {mr/mc/me/ml/ms/mv}
          ・指定しなければ、「(b2e32.dllのあるディレクトリ)\b2e」から読み込みます。
          ・B2ESetScriptDirectory()を呼び出します。

        /Ds<directory>
          ・Susie Plug-in(*.spi)のあるディレクトリを指定します。 {mr/me/ml/mv}
          ・指定しなければ、reces.exeのあるディレクトリから読み込みます。
          ・Susie Plug-inを読み込みたくなければ、「/Ds:」など無効なディレクトリ名を指定してください。

        /Dw<directory>
          ・Total Commander Plugin(*.wcx)のあるディレクトリを指定します。 {mr/me/ml/mt/mv}
          ・指定しなければ、reces.exeのあるディレクトリから読み込みます。
          ・Total Commander Pluginを読み込みたくなければ、「/Dw:」など無効なディレクトリ名を指定してください。


 /N
   ・書庫を新規作成します。 {mr/mc}
   ・同名書庫ファイルが存在する状態での再圧縮/圧縮動作は、追加圧縮扱いとなります。
     新規圧縮(上書き作成)にはこの「/N」オプションを指定してください。
   ・上書き元の書庫はrecesでは復元できません。
   ・既存の同名書庫ファイルが作成する書庫と違う形式である場合、
        例: 作成したい書庫: arc.zip[zip形式]
            同名の書庫    : arc.zip[lzh形式]
     「/N」では書庫を作成することが出来ません。
     「/NF」を使用してください。
   ・b2e32.dllについて、動作はb2eスクリプトに依存します。

 /NF
   ・書庫を強制的に新規作成します。 {mr/mc}
   ・同名書庫ファイルが存在する場合、圧縮前に削除します。
   ・圧縮処理中にキャンセルしても元書庫は復元できません。
   ・b2e32.dllでは出力書庫名の取得が行えないため、使用できません。


 /I<pattern...>
   ・処理対象にしたいファイル/ディレクトリを指定します。 {mr/mc/me/ml/md/mn}
   ・ワイルドカード「*」「?」を使用することが出来ます。
        例: /I*.jpg
            (jpgファイルのみを対象とする)
   ・サブディレクトリも検索します。
   ・「/I*.exe;*.bat」の様に、「;」を用いて複数指定することが出来ます。
   ・コマンドラインオプションの中に「/i:r」「/I:r」が含まれると正規表現として処理されます。
        例: /Iimg_\d{3}_ /I:r

 /i<pattern...>
   ・サブディレクトリは検索しません。
   ・パスの指定が無ければルート直下のファイルのみを対象とします。
     (ただし、正規表現の場合は除きます。/i:r[pattern...]を参照)
   ・通常は「/I」オプションを使用することをお勧めします。
   ・コマンドラインオプションの中に「/I」が含まれると、
     /i以下も含め、全てサブディレクトリ検索の対象となります。

 /i:<...>
   ・処理対象にしたいファイル/ディレクトリの名前以外の情報を指定します。 {mr/mc/me/ml/md/mn}
   ・書庫がその情報を持っている必要があります。
   ・「@」以外は「/i:」も「/I:」と同様にサブディレクトリを検索します。
   ・以下のオプションは「:」で区切って複数指定することが出来ます。
     (「/i:r /i:s320k」の様に別々に指定することも出来ます。)
        /I:r[pattern...]
          ・処理対象にしたいファイルを正規表現で指定します。
         ・「;」で区切って複数指定することができます。
          ・「/I」以下でも指定可能です。(/I<pattern...>を参照)
        /i:r[pattern...]
          ・処理対象にしたいファイルを正規表現で指定します。
          ・「/i<pattern...>」と異なり、サブディレクトリ以下も検索し、
            また、その際はパスを除くファイル名でマッチします。

                     foo.zip
                      ├─DIR
                      │  └─001.jpg
                      └─001.jpg

             例えば、上記ディレクトリ階層の書庫について、

                「/i001.jpg」とすると、
                          001.jpg
                が解凍され、
                「/i:r001.jpg」とすると、
                     foo.zip
                      ├─DIR
                      │  └─001.jpg
                      └─001.jpg
                が解凍されます。

          ・「/i」以下でも指定可能です。(/I<pattern...>を参照)
        /i:s<value>
          ・指定サイズ以上のファイルを処理対象にします。
            (数値の指定方法については「/s」オプションを参照)
        /i:S<value>
          ・指定サイズ以下のファイルを処理対象にします。
          ・ファイル分割を行う「/s」とは一部異なり、
            末尾に<b|k|m|g|t>がなければバイト単位として処理します。

        /i:d<DateTime>
          ・指定日時以降のファイルを処理対象にします。
        /i:D<DateTime>
          ・指定日時以前のファイルを処理対象にします。
          ・日時は「yyyymmddhhmmss」で指定してください。
          ・ただし、yyyy以外は省略可能です。
               例: /i:D20120103
               (2012年1月3日以前のファイルを対象にする)

        /i:a<d|h|r|s>
          ・指定属性を含むファイルを処理対象にします。
          ・dirコマンドと同様に複数指定することが出来ますが、and条件ではなくor条件となります。
          ・「d」は「サブディレクトリ及びそれ以下のファイルのみを対象とする」と言う意味です。
          ・Susie Plug-inは「d」のみの対応となります。

        /I:@<filename>
          ・処理対象フィルタのリストを指定します。
          ・サブディレクトリも検索します。
          ・Shift-JIS、UTF-8、UTF16-LE、UTF16-BEに対応しています。
          ・BOM無しのUTF-8/16ファイルは「/C:@」で文字コードを指定してください。

        /i:@<filename>
          ・サブディレクトリは検索しません。(パスの指定が無ければルート直下のファイルのみ)



 /X<pattern>
   ・処理対象から除外したいファイル/ディレクトリを指定します。 {mr/mc/me/ml/md/mn}
   ・ワイルドカード「*」「?」を使用することが出来ます。
        例: /I*.exe
            (exeファイルを対象から除外する)
   ・サブディレクトリも検索します。
   ・「/X*.exe;*.bat」の様に、「;」を用いて複数指定することが出来ます。
   ・コマンドラインオプションの中に「/x:r」「/X:r」が含まれると正規表現として処理されます。
        例: /Ximg_\d{3}_ /X:r

 /x<pattern>
   ・サブディレクトリは検索しません。
   ・パスの指定が無ければルート直下のファイルのみを対象とします。
     (ただし、正規表現の場合は除きます。/x:r[pattern...]を参照)
   ・通常は「/X」オプションを使用することをお勧めします。
   ・コマンドラインオプションの中に「/X」が含まれると、
     /x以下も含め、全てサブディレクトリ検索の対象となります。

 /x:<...>
   ・処理対象から除外したいファイル/ディレクトリの名前以外の情報を指定します。 {mr/mc/me/ml/md/mn}
   ・書庫がその情報を持っている必要があります。
   ・「@」以外は「/x:」も「/X:」と同様にサブディレクトリも検索します。
   ・以下のオプションは「:」で区切って複数指定することが出来ます。
     (「/i:r /i:s320k」の様に別々に指定することも出来ます。)
        /X:r[pattern...]
          ・処理対象にしたいファイルを正規表現で指定します。
         ・「;」で区切って複数指定することができます。
          ・「/X」以下でも指定可能です。(/X<pattern...>を参照)
        /x:r[pattern...]
          ・処理対象にしたいファイルを正規表現で指定します。
          ・「/x<pattern...>」と異なり、サブディレクトリ以下も検索し、
            また、その際はパスを除くファイル名でマッチします。(/i:r[pattern...]を参照)
          ・「/x」以下でも指定可能です。(/X<pattern...>を参照)
        /x:s<value>
          ・指定サイズ以上のファイルを処理対象外にします。
            (数値の指定方法については「/s」オプションを参照)
        /x:S<value>
          ・指定サイズ以下のファイルを処理対象外にします。
          ・ファイル分割を行う「/s」とは一部異なり、
            末尾に<b|k|m|g|t>がなければバイト単位として処理します。

        /x:d<DateTime>
          ・指定日時以降のファイルを処理対象外にします。
        /x:D<DateTime>
          ・指定日時以前のファイルを処理対象外にします。
          ・日時は「yyyymmddhhmmss」で指定してください。
          ・ただし、yyyy以外は省略可能です。

        /x:a<d|e|h|r|s>
          ・指定属性を含むファイルを処理対象外にします。
          ・dirコマンドと同様に複数指定することが出来ますが、and条件ではなくor条件となります。
              例えば、「/x:ahr」は「隠し属性か読み取り専用属性であれば除外する」と言う意味です。
          ・「d」は「最上層のファイルのみ処理する」と言う意味です。
          ・「e」は「空ディレクトリを除外する」と言う意味です。
          ・Susie Plug-inは「d」「e」のみの対応となります。

        /X:@<filename>
          ・処理対象除外フィルタのリストを指定します。
          ・サブディレクトリも検索します。
          ・Shift-JIS、UTF-8、UTF16-LE、UTF16-BEに対応しています。
          ・BOM無しのUTF-8/16ファイルは「/C:@」で文字コードを指定してください。

        /x:@<filename>
          ・サブディレクトリは検索しません。(パスの指定が無ければルート直下のファイルのみ)


 /l[<0-9|x>]
   ・圧縮率を指定します。 {mr/mc}
        /l<0-9>
          ・圧縮率を数字で圧縮します。
          ・指定された数字は、そのまま書庫操作ライブラリに送ります。
               例: /l7
                   (圧縮レベル「7」で圧縮)

        /l
          ・最低圧縮率で圧縮します。

        /lx
          ・最大圧縮率で圧縮します。


 /s<value>
   ・書庫を分割します。 {mr/mc}
   ・7z/zip書庫の場合もrecesで分割/結合を行います。
   ・分割サイズ指定は末尾に<b|k|m|g|t>を付加します。
        例: /s10m
            (10MBで分割)
   ・末尾に<b|k|m|g|t>がなければ、分割数指定として処理します。
        例: /s12
            (12個に分割)


 /od[directory]
   ・出力先ディレクトリを指定します。 {mr/mc/me}
        /od
          ・デスクトップに出力します。
        /od<directory>
          ・指定された<directory>に出力します。
               例: /odD:\Documents
               (D:\Documentsに出力)
          ・相対パスは処理元ファイルがあるディレクトリを基準として処理します。
            「/oob」を指定すると、カレントディレクトリが基準となります。
               例: /oob /od.
               (カレントディレクトリに出力)


 /of<filename>
  ・出力ファイル名を指定します。 {mr/mc}
  ・「/od」と同時に指定すると、結合可能であれば結合したパス、不可能であれば「/of」を優先します。
       例: /odD:\Documents /ofarc.zip
           (D:\Documents\arc.zipとして出力)
  ・拡張子は[type]に対応するものが自動的に付加されます。

 /oF<filename>
  ・出力ファイル名を指定します。 {mr/mc}
  ・拡張子の付加を行いません。


 /oo<b|r>
  ・出力オプション。
        /oob {mr/mc/me}
          ・カレントディレクトリ基準で'/od','/of'の相対パスを処理します。
          ・デフォルトでは処理元ファイルがあるディレクトリ基準となります。

        /ooke {mc}
          ・元ファイルの拡張子を保持して圧縮します。
          ・例えば、「xxx.txt」をzip圧縮すると「xxx.txt.zip」として出力されます。

        /oor {mr/mc}
          ・出力ファイル名の重複を避けるため自動的にリネームします。
          ・例えば、name.zipが既に存在する場合、name_1.zipとして出力します。


 /pw<password>
   ・パスワードを指定します。 {mr/mc/me/ml}
        例: /pwFoo /pwBar
            (パスワード候補を複数指定)
   ・入力を省略すると、処理時に入力を求めます。
   ・設定ファイルには保存されません。
   ・一度入力したパスワードはreces終了時まで保持されます。
     つまり、同じパスワードであれば、複数書庫があっても一度の入力で済みます。

 /pf<filename>
   ・パスワードリストを指定します。 {mr/mc/me/ml}
   ・Shift-JIS、UTF-8、UTF16-LE、UTF16-BEに対応しています。
   ・BOM無しのUTF-8/16ファイルは「/C:@」で文字コードを指定してください。
         例: /C:@utf16 /pfPasswords.txt
             (Password.txtをUTF16-LEで読み込む)
   ・リスト内では特殊文字をエスケープする必要はありません。

 /pn<password>
   ・再圧縮後書庫の新しいパスワードを指定します。 {mr}
   ・二重引用符「"」を指定するには「\"」の様にエスケープする必要があります。


 /P<param>
   ・ユーザ独自のパラメータを指定します。 {mr/mc/me/ms}
         例: /P-m0=ppmd
             (PPMd方式で7z書庫を作成)
   ・ライブラリへ送る引数に追加するだけです。チェックなどは行いません。
   ・削除コマンド使用時には付加されません。
   ・Susie Plug-in、Total Commander Pluginは対応していません。
   ・直接操作では/ms以下のパラメータ先頭への追加となります。


 /t
   ・書庫の更新日時を元ファイルと同じにします。 {mr/mc/md/mn}
   ・複数ファイル処理時は、先頭のファイルのタイムスタンプを使用します。
   ・b2eスクリプトでは動作しない場合があります。


 /T
   ・テスト実行します。 {mn}
   ・実際の書庫処理は行いません。
   ・現在は「/mn」のみの対応で、リネーム処理前後のファイル名を表示します。


 /DIRTS
   ・ディレクトリのタイムスタンプを復元します。 {mr/me}
   ・同梱の7-zip32.dllであれば、このオプションが無くとも復元します。
   ・タイムスタンプを持たない場合や、ライブラリの仕様により取得が出来ない場合、
     そのディレクトリ以下のファイルのタイムスタンプを採用します。
   ・解凍後に再度書庫ファイル情報の取得を行うため、
     ファイル数が多い程処理に時間を要します。


 /r[command]
   ・再圧縮時の解凍と圧縮の間に実行するコマンドを指定します。 {mr}
        例: /r"ren a.txt b.txt"
            (a.txtをb.txtにリネーム)
   ・カレントディレクトリは一時ディレクトリです。
   ・二重引用符「"」を指定するには「\"」の様にエスケープする必要があります。
   ・コマンドを省略すると解凍後に入力を求めます。
   ・何も入力せずにEnterで圧縮処理を開始します。
   ・system()に直接投げるだけですので、実行するとややこしいことになるコマンドはご遠慮ください。
   ・入力ファイルパスを環境変数%FILEPATH%及び%FILEPATH[i]%に、
     ファイル数を%FILECOUNT%に代入しています。
   ・%FILEPATH[0]%は%FILEPATH%と同じファイルパスとなります。


 /R
   ・対象ディレクトリを再帰的に検索します。 {mc}
   ・ディレクトリを受け付けないb2eスクリプトによる圧縮を行う場合に必要となります。


 /b
   ・バックグラウンドで処理します。
   ・統合アーカイバライブラリのSetBackGroundMode()は呼び出しません。
   ・SetPriorityClass()を呼び出します。


 /C<CodePage|Charset>
   ・書庫ファイルの文字コードを指定します。 {mr/me/ml/mt/md/mn}
   ・コードページか文字セットを指定してください。
        例: /C936
            (簡体字中国語(GB2312)で処理)
        例: /Ceuc-jp
            (日本語(EUC)で処理)
   ・現在、同梱の7-zip32.dll文字化け対策版によるzip書庫操作のみの対応となります。
   ・「/mv cp」で指定できる文字コードの一覧を表示します。
     「コードページ:説明:文字セット」の順で表示されます。
   ・findstrで「/mv cp」の結果を抽出を行うには「/C:oa」が必要です。
        例: /mv /C:oa cp | findstr "日本語"
            (「/C」で指定できる日本語コードページ表示)


 /C:@<Shift-JIS|UTF-8|UTF16-LE|UTF16-BE>
   ・リストファイルの文字コードを指定します。
   ・リストファイル指定オプションより前に記述する必要があります。
   ・Shift-JIS、UTF-8、UTF16-LE、UTF16-BEの何れかを指定してください。
        例: /C:@UTF-8 /@list
            (listをUTF-8で読み込み)
   ・大文字小文字の区別はしません。
   ・「shiftjis」や「sjis」としたり、「utf16」や「Unicode」、「utf-16le」としても読み込みます。
     それらしいものであれば何とかします。


 /C:oa
  ・文字をANSIに変換し出力します。
  ・リダイレクト先がUnicode文字を受け付けない場合に必要となります。
        例: /mv /C:oa cp | findstr "日本語"
            (「/C」で指定できる日本語コードページ表示)


 /{<filename>
   ・設定をファイルから読み込みます。

 /}<filename>
   ・設定をファイルに書き出します。


 /q[-|0|1]
   ・処理完了後recesを終了します。
        /q<-|0>
          ・無効 (一時停止します。)

        /q[1]
          ・有効 (デフォルトです。)


 /qe
   ・エラーが発生した場合、次のファイルの処理に進まずに中断します。
   ・そのまま継続するか、recesを終了するかを選択することができます。


 //
   ・オプション解析を終了します。
   ・ファイル名の先頭に「-」「/」を含む書庫を処理する際は必須となります。


 /@<listfiles...>
   ・処理したいファイル/ディレクトリが書かれたリストファイルを指定します。
   ・Shift-JIS、UTF-8、UTF16-LE、UTF16-BEに対応しています。
   ・BOM無しのUTF-8/16ファイルは「/C:@」で文字コードを指定してください。
        例: /C:@UTF-8 /@list
            (listをUTF-8で読み込み)



●使用例
 [再圧縮]
 例1.lzhファイルをzipファイルに再圧縮する。
     ・動作モードと圧縮形式を省略すると/mrzipとして処理されます。
     reces foo.lzh

 例2.パスワード付rarファイルを、パスワードを引き継いで7zファイルに再圧縮する。
     ・元書庫と同様にパスワード付書庫とするには、圧縮形式に'pw'が必要であることに注意してください。
     reces /mr7zpw /pwPassword foo.rar

 例3.パスワード付7zファイルを、新たにパスワードを設定した7zファイルに再圧縮する。
     reces /mr@pw /N /pwPassword /pnNewPassword foo.7z

 例4.7zファイルやtar.xzファイルの中身をまとめてzipファイルに再圧縮する。
     reces foo.7z bar.tar.xz

 例5.zipファイルから、「*.db」にマッチするファイルを除き、
     圧縮率を最高に、全てのファイルを最上層に移動させて、入力書庫と同じzipに再圧縮する。
     また、タイムスタンプは元書庫と同じにする。
     reces /mR@ /N /X*.db /lx /e /t foo.zip bar.zip

 例6.書庫の中身をエクスプローラで編集する。(誇張表現--;)
     reces /mr@ /N /r"explorer .&pause" foo.zip

 例7.書庫内のファイル名から「_thumb」を取り除く。(foo_thumb.jpg->foo.jpg)
     ・ただし、この置換にはperl 5.14以降が必要です。
     reces /mr@ /N /r"perl -MFile::Find -e \"find sub{rename($_,s/_thumb//r)if -f},@ARGV\" ." foo.zip
     ・reces Ver.0.00r31以降では、「/mn」の使用を推奨します。
     reces /mn_thumb foo.zip

 例8.書庫内のファイル名の拡張子txtをdatにリネームする。(foo.txt->foo.dat)
     reces /mr@ /N /r foo.zip
     @echo off&ren *.txt *.dat&for /r /d %i in (*) do (if exist "%i\*.txt" (ren "%i\*.txt" "*.dat"))
     ・reces Ver.0.00r31以降では、「/mn」の使用を推奨します。
     reces /mn.txt:.dat foo.zip

 例9.通常のパスワード付7z書庫をヘッダ暗号化7z書庫に再圧縮する。
     reces /mr@he /N /pwPassword foo.7z

 例10.書庫から基底ディレクトリを削除する。(foo\bar.txt->bar.txt)
     reces /mr@ /N /eb foo.zip

 例11.書庫内の画像を全てリサイズする。
      ・リサイズソフトにはnilpo氏( http://nilposoft.info/ )のRalphaを使用します。
      ・各種設定を済ませ、iniファイルに書き出しておいてください。
      ・次回以降の仕様も考慮し、recesの設定ファイルとショートカットファイルを利用します。
     [1]設定ファイルに書き出す。(':'指定は処理を行わない様にするため)
      ・例として、Ralphaの設定ファイル名はresize.iniとします。
      ・Ralphaのパスも環境に合わせて変更してください。
       reces /mr@ /N /e /t /r"\"C:\Program Files (x86)\Ralpha\Ralpha.exe\" /ini=resize.ini ." /}"C:\cfg\resize.cfg" :
     [2]reces.exeのショートカットを作成し、「リンク先」にrecesの設定ファイル名を追加します。
       リンク先:～\reces.exe /{"C:\cfg\resize.cfg"
     [3]縮小したい画像の書庫をショートカットにドラッグ&ドロップ!

 例12.作業(一時)ディレクトリを指定する。
     一時ディレクトリとして割り当てたRAMディスクの容量以上の大きさのファイルを処理する際などに必要となります。
     (圧縮中に一時ディレクトリの空き容量が0となると、修復が難しい不正な書庫が作成される可能性があります。)
     set TMP=D:\tmp_dir
     reces /mr7z /N /t /X*.bak arc.zip

 例13.複数の書庫を一つのrar書庫に再圧縮する。
     ・圧縮形式は存在しないものであれば何でも良いので「/mr^q^」でも処理可能です。
     reces /mrr /r"for /F %F in ('echo %FILEPATH%') do \"C:\Program Files\WinRAR\Rar.exe\" a %~dpnF.rar -r *" arc1.zip arc2.7z arc3.lzh

 例14.zip書庫をarc書庫に再圧縮する。
     ・「arc.b2e」を用意する必要があります。
     ・今回はめもめも様( https://web.archive.org/web/20181105083100/http://www.geocities.jp/home_page22/b2e/FreeArc.html )の「arc.b2e」を使用します。
     ・b2eスクリプトの既定ディレクトリは「b2e32.dllのあるディレクトリ\b2e」です。
          例: C:\WINDOWS\System32\b2e32.dll
                なら
              C:\WINDOWS\System32\b2e
       「/Db」でb2eスクリプトがあるディレクトリを指定することが出来ます。
     reces /mrb2e:arc foo.zip
     ・圧縮率を指定するには以下のように<method>を追加してください。
     reces /mrb2e:arc:最高圧縮 foo.zip


 [圧縮]
 例1.zipファイルに圧縮する。
     reces /mc foo.dat

 例2.圧縮元と同じタイムスタンプを持つzipファイルに圧縮する。
     reces /mc /t foo.dat

 例3.各ファイルを7zファイルに圧縮する。
     reces /mc7z /e foo.dat bar.txt

 例4.ファイルをパスワード付でzipファイル圧縮し、指定したファイル名で出力。
     reces /mczippw /pwPassword /ofC:\foo\bar\baz.zip qux.txt

 例5.カレントディレクトリにある全てのディレクトリについて、
     「*.jpg」にマッチするファイルのみを対象にそれぞれzipファイルに最高圧縮率で圧縮。
     dir /b /ad | reces /mc /lx /e /I*.jpg

 例6.基底ディレクトリを格納せずに圧縮する。(Rar.exeの-ep1 -r DIR\、Archon2の-o1相当)
     reces /mc /eb foo

 例7.既存の書庫のルートにファイルを追加する。
     reces /mc /ofC:\foo.zip bar.jpg

 例8.既存書庫のサブディレクトリ以下にファイルを追加する。
    foo.zip
     ├─aaa
     │  └─foo.jpg
     └─bbb
         └─ccc
             └─bar.jpg
     上記書庫「foo.zip」内ディレクトリ「ccc」に「baz.jpg」を追加するには、
     対象ディレクトリまでのパス(bbb\ccc)と同じになる様に、ディレクトリを作成&ファイルを配置し、
         bbb
         └─ccc
             └─baz.jpg
     reces /mc /ofC:\foo.zip bbb

 例9.PPMd方式で7z書庫ファイルを作成する。
     reces /mc7z /P-m0=ppmd *.txt

 例10.無圧縮zipアルバム(*.zip.mp3)を作成する。
     reces /mc /l /eb /oFAlbum.zip.mp3 album

 例11.lz4書庫を作成する。
     ・「lz4.b2e」を用意する必要があります。
     ・今回はClaybird様( http://claybird.sakura.ne.jp/index.html )の「lz4.b2e」を使用します。
     reces /mcb2e:lz4 /ooke file

 例12.LAMEでmp3にエンコードする。
     ・「mp3.b2e」は同梱しています。
     ・「lame.exe」と「lame_enc.dll」を「b2e」ディレクトリにコピーします。
     reces /mcb2e:mp3 /Dbb2e /e a.wav b.wav
     ・音質を指定にするには以下のように<method>を追加してください。
     reces /mcb2e:mp3:insane /Dbb2e /e a.wav b.wav


 [解凍]
 例1.lzhファイルを解凍する。
     reces /me foo.lzh

 例2.新規作成したディレクトリにzipファイルを解凍する。
     解凍後はzipファイルをごみ箱へ送る。
     reces /me /c12t /d foo.zip

 例3.パスワードリストファイルを利用して、7zファイルを解凍する。
     reces /me /pflist.txt foo.7z

 例4.zipファイル内のcppファイルのみ解凍する。
     reces /me /I*.cpp foo.zip

 例5.3つに分割されたzipファイルを解凍する。(先頭のファイルのみを入力します。)
     reces /me foo.zip.001

 例6.書庫内ファイルを全て同じ階層に解凍する。
     reces /mE foo.zip

 例7.XacRett.dllを使用して書庫を解凍する。
     reces /meXacRett arc.zip

 例8.共通するパスを除外して解凍する。
     (解凍レンジの「パス情報を最適化して展開する」相当)
     reces /me /c /ebx arc.zip

 例9.7z.dllを利用して解凍する。
     ・「7z.dll対応版7-zip32.dll/7-zip64.dll」( http://frostmoon.sakura.ne.jp/ )を利用します。
     ・x64版ではUNLHA32.DLLが直接利用できないため、「UNBYPASS.DLL」もしくは「7z.dll対応版7-zip64.dll」の導入をお勧めします。
     ・下記拡張子の解凍が出来ます。
            7z, XZ, BZIP2, GZIP, TAR, ZIP
            AR, ARJ, CAB, CHM, CPIO, CramFS, DMG, EXT, FAT, GPT, HFS, IHEX, ISO, LZH, LZMA, MBR, MSI, NSIS, NTFS,
            QCOW2, RAR, RPM, SquashFS, UDF, UEFI, VDI, VHD, VMDK, WIM, XAR, Z
            (各種自己解凍形式含む)
     reces /me7-zip64 /c arc.lzh

 例10.arc書庫(FreeArc)を解凍する。
     ・「multiarc.wcx」( http://wcx.sourceforge.net/ )とFreeArc付属の「freearc.addon」を利用します。
          reces /mSmultiarc.wcx
          「Import Addon」->「Addons/TotalCommander MultiArc plugin/freearc.addon」を選択。
          reces /memultiarc.wcx /c arc.arc
     ・b2e32.dllとb2eファイルの組み合わせでも解凍可能です。
     ・めもめも様( https://web.archive.org/web/20181105083100/http://www.geocities.jp/home_page22/b2e/FreeArc.html )の「arc.b2e」がおすすめです。

 例11.解凍対象ファイルを正規表現を用いて指定する。
     ・「img_1_s.jpg」...「img_23_s.jpg」...「img_123_s.jpg」...「img_1234_s.jpg」とあるうち、
       数字三桁のファイルのみを対象とする。
       reces /me /i:rimg_\d{3}_ arc.zip

 例12.インストーラを解凍する。
     ・「exe.msi.b2e」は同梱しています。
     ・「b2e」ディレクトリにUniversal Extractor( http://legroom.net/software/uniextract もしくは https://github.com/Bioruebe/UniExtract2 )の
       「UniExtract.exe」と「bin」をコピーしてください。
          reces /meb2e32 /c /Dbb2e setup.exe
     ・「7z.dll対応版7-zip32.dll/7-zip64.dll」( http://frostmoon.sakura.ne.jp/ )でも一部インストーラを解凍することができます。

 例13.文字化けするzipファイルに対して正しい文字コード(ここではEUC-JP)を設定して解凍する。
       reces /me /Ceuc-jp arc.zip


 [書庫内ファイル一覧表示]
 例1.zipファイルに含まれるファイル名を表示する。
     reces /ml foo.zip

 例2.zipファイルに含まれるファイルの詳細な情報を表示する。
     reces /mL foo.zip

 例3.zipファイルに含まれるファイルをリストとして出力する。
     reces /ml /na foo.zip > foo.zip.lst
     ・ただし、BOMが付加されないので、
       予めバイナリエディタでFF FEとだけ入力したファイルに対しリダイレクトすることをお勧めします。
     ・もしくはnkfを用いて出力後にBOMを付加してください。
          nkf --ic=UTF-16LE --oc=UTF-16LE-BOM foo.zip.lst
     ・Unicode文字が含まれないのであれば、「/C:oa」を指定しANSIで出力することで、BOMの付加が不要となります。
          reces /ml /C:oa /na foo.zip > foo.zip.lst


 [テスト]
 例1.非対応、パスワード間違い、壊れている書庫を表示。
     ----- test.bat -----
     ----- ここから -----
     @echo off
     :loop
     if "%~1" == "" goto end
     C:\～\reces.exe /mt /nae "%~1"
     if ERRORLEVEL 1 echo "%~1"
     shift
     goto loop
     :end
     ----- ここまで -----
     test.bat foo.zip bar.lzh baz.rar


 [削除]
 例1.書庫から「*.db」にマッチするファイルを削除する。
     reces /md /X*.db arc.zip


 [書庫内ファイルのリネーム]
 例1.書庫内ファイル名から指定文字列を削除する。
     ・Compress JPEG Images Online( http://compressjpeg.com/ )で作成した「compressjpeg.zip」について、
       ファイル名に付加される「-min」を削除します。
     reces /mn-min compressjpeg.zip

 例2.書庫内ファイル名の数字部分を0で埋めて3桁に揃える。
     ・「IMG_1.jpg」「IMG_2.jpg」...「IMG_10.jpg」「IMG_11.jpg」...「IMG_100.jpg」...が含まれる書庫について、
       数字部分先頭に「0」を付加して3桁で揃える。
     reces /mN(^^^|\D+)(\d+):$01000$02;(^^^|\D+)\d+(\d{3}):$01$02 arc.zip
       もしくは
     reces /mN arc.zip
     リネーム条件入力(正規表現):
     検索文字列: (^|\D+)(\d+)
     置換後文字列: $01000$02
     検索文字列: (^|\D+)\d+(\d{3})
     置換後文字列: $01$02
     検索文字列: <何も入力せずにEnter>


 [直接操作]
 ・書式は以下の通りです。
     reces [/<options(reces)>...] /ms<library>[:prefix] <command> [<switches>...] <archive_name> [<file_names>...] [<@listfiles...>]
 ・「/ms<library>[:prefix]」以降はライブラリに渡されます。recesのオプションは/msの前に指定してください。
     reces /q0 /ms7-zip32 x xxx.zip
 ・ワイルドカードは展開されずにライブラリに渡されます。
 ・tar32.dllを操作するには、「--inverse-procresult=1」オプションが必要です。
 ・recesでプログレスバーを出力するには、各ライブラリのダイアログ非表示オプションを有効にしてください。

 例1.7-zip32.dllを使用してzipファイルを解凍する。
     reces /ms7-zip32 x foo.zip

 例2.(recesでは非対応の)cab32.dllを使用して書庫を作成。
     reces /mscab32:Cab -a -mz foo.cab C:\dir\ file

 例3.ZIP32J.DLLを用いて壊れたzipファイルを修復する。
     ・詳細についてはZIP32J.DLL付属のドキュメントをご覧ください。
     ・Unicode文字は扱えません。
     [通常]
        reces /mszip32j:Zip -F arc.zip
     [重度の破損](書庫サイズを信用せずに修復)
        reces /mszip32j:Zip -FF arc.zip

 例4.ZIP32J.DLLを用いて日本語パスワード付zipファイルを作成する。
     reces /mszip32j:Zip -P パスワード arc.zip file



●Q&A風備忘録
 Q1.Susie Plug-inを使いたくない。
 A1.「/Ds:」とすると読み込みません。
 Q2.PPMd方式7z書庫で圧縮するには如何すれば良いのか。
 A2.「/P」オプションでパラメータを追加してください。
       例: reces /mc7z /P-m0=PPMd file
 Q3.オプション入力でTabキー補完を使いたい。
 A3.補完したい文字列の前に二重引用符を入力してください。(終端の二重引用符も自動で補完されます。)
       例: reces /me /od"d
           <Tabキー押下>
           reces /me /od"dir"
 Q4.正規表現で「^」が使用できない。
 A4.コマンドプロンプト上では、「^」(ハット)は特殊記号扱いとなります。
    「^^」とエスケープすることで「^」そのものを表します。
       例: /X:rDir\\[^^\\]+\.txt



●注意
 [全般]
 ・バグだらけです。

 [ファイル入力]
 ・Unicode文字を含むファイル名を取り扱えるのは現在、
      ・7-zip32.dll
      ・UNLHA32.dll
      ・unrar32.dll
      ・UnIso32.dll
      ・Total Commander Plugin(プラグイン側が対応している場合)
   のみです。
   現在、Susie Plug-inでUnicode文字は扱えません。
 ・分割rar書庫の結合はunrar32.dllで行います。
 ・recesでは対象ファイルを「ワイルドカードを用いて」再帰的に検索することはできません。
   dirコマンドとパイプを使用してください。
   例.カレントディレクトリ以下の全てのzipファイルを、新規ディレクトリを作成して解凍する。
     「dir /a-d /b /s *.zip | reces /me /c12」
   例.カレントディレクトリ以下の全てのファイルを、新規ディレクトリに解凍する。
      ・エラーメッセージ非表示オプション「/ne」を併用することをお勧めします。
     「reces /me /c12 /ne .」
 ・7-zip32.dllはVer.9.22.00.01よりMAX_PATH以上のパスに対応していますが、recesでは完全に対応出来ているかは未確認です。

 [再圧縮/圧縮]
 ・同名書庫ファイルが存在する状態での再圧縮/圧縮動作は、追加圧縮扱いとなります。
   新規圧縮(上書き作成)には「/N」オプションを指定してください。
 ・現在7-zip32.dllによるzip圧縮でパスワードにマルチバイト文字(日本語など)は使用出来ません。
 ・パスワード付書庫を再度パスワード付書庫として圧縮するには、
  「/mr@pw」「/mrzippw」など再圧縮オプション「/mr」に「pw」を付加したものを指定してください。
 ・自己解凍書庫作成と書庫分割は同時に指定することが出来ません。
 ・パスワード付7z自己解凍書庫の作成(/mc7zpwsfx,/mc7zhesfx)には7-zip32.dllと同じ階層に7z.sfxを用意する必要があります。
   「7z.sfx」は7-Zipをインストールしたディレクトリにあります。
 ・tar32.dllの追加圧縮は内容を一度解凍してから行うため、書庫によっては時間が掛かります。
 ・tar32.dllでフィルタを用いて圧縮を行うと、ディレクトリのタイムスタンプ格納が不完全な書庫が作成されてしまいます。

 [解凍]
 ・「/c1」「/c2」のチェックはフィルタ適用前に行われるため、
   「foo.txt」「bar.doc」を含む書庫において、
   「/me /c1 /X*.txt」としてもディレクトリが作成されてしまいます。
 ・XacRett.dllを用いてのパスワード付gca書庫の解凍は現在対応していません。

 [その他]
 ・b2eスクリプト使用時は利用できるオプションが大幅に制限されます。
 ・b2eスクリプトではフィルタの使用ができません。
 ・b2eスクリプトでは「/of」「/oF」で指定したファイル名で出力されない場合があります。
 ・Susie Plug-in、b2eスクリプトでの対応書庫確認は、ファイルの拡張子で行います。
 ・サイズ指定にはTBを表す「t」が使用可能ですが、テストを行っていませんので、十分注意してください。
 ・UNBYPASS.DLLは現在UNLHA32.dllとの組み合わせのみ対応しています。
 ・元のファイルと異なるライブラリ名を指定することは出来ません。(/ml8-zip32.dll)
 ・一部書庫についてXacRett.dllではリスト表示が正しく処理されません。
   そのため、二重ディレクトリ防止も正しく処理されません。



●開発環境
 OS:Microsoft Windows 10 Home Premium 64-bit
 CPU:Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz 3.41GHz
 memory:16.0GB RAM
 compiler/debugger:Microsoft Visual Studio Community 2022 (64 ビット) - Current Version 17.3.1
 editor:xyzzy version 0.2.2.235



●謝辞
 本ソフト開発にあたり参考/使用させて頂いたソフトウェアや、
 各書庫操作ライブラリの作者様方...
   k.inaba氏(Noah,UnDller)
   Claybird氏(LhaForge,b2e32.dll,tar32.dll,tar64.dll,unrar32.dll)
   鬼束 裕之氏(Explzh)
   Bonty氏(Archon2,XacRett.dll)
   高田 謙氏(undll.exe,UnIso32.dll)
   秋田 稔氏(7-zip32.dll)
   市丸 剛氏(7-zip64.dll,tar64.dll)
   吉岡 恒夫氏(tar32.dll)
   Ｍｉｃｃｏ氏(UNLHA32.dll)
   Alexander Roshal氏(unrar.dll,unrar64.dll)
   亀井 哲弥氏(unrar32.dll)
   RuRuRu氏(unrar32.dll x64/ユニコード対応版)
   TORO氏(UNBYPASS.DLL,ZBYPASSA.SPH)
 ...他多数!!!
 大変可愛らしいアイコンを提供していただいた「なつだ」氏、
 そしてユーザの皆様方に、
 この場を借りて御礼申し上げます。



●ライセンス
 ・本ソフトはNYSL Version 0.9982に準拠して配布されています。
   ライセンスの詳細は同梱の「NYSL_withfaq.TXT」をご覧ください。
 ・「7-zip32.dll/7-zip64.dll文字化け対策版」( http://frostmoon.sakura.ne.jp/ )は
   GNU Lesser General Public License (LGPL)の下で配布されています。
   ライセンスの詳細は下記URLをご覧ください。
      http://www.gnu.org/copyleft/lesser.ja.html
 ・「b2e64.dll」はClaybird氏による「b2e32.dll」を64bit化したものです。
   「b2e32.dll」はNYSL ver 0.9982が適用されています。
   ライセンスの詳細は「b2e32.dll」に同梱されている「b2e32.txt」をご覧ください。



●その他
 サポート(ご意見、ご感想、不具合のご報告、ご要望等)は
 Y.R.Takanashi@gmail.com
 もしくは
 BBS(以下URLよりアクセス可能)からお願いします。
 最新版は
 http://frostmoon.sakura.ne.jp/
 から入手することができます。



●開発履歴
 ○Ver.0.00r34 - 2024/09/03
 ・7-zip32.dll及びunrar32.dllで処理する書庫ファイルの結合処理をreces側で行なわないように。
 ・lzh等一部分割ファイルの処理が行えない不具合を修正。
 ・「～.part1.rar」及び「～.rar」「～.r00」に関する処理を追加。処理後削除等が正しく処理できるように。(Special Thanks!:通りすがりの人様)
 ・一部Susie Plug-inでサイズ0のファイルが出力される不具合を修正。(Special Thanks!:kiyohiro様)
 ・「ZBYPASSA.SPH」が動作しない不具合を修正。(Special Thanks!:kiyohiro様)
 ・統合アーカイバライブラリ(*.dll)のあるディレクトリを指定する「/Dd」を追加。(Special Thanks!:通りすがりの人様、kiyohiro様)
 ・tar32/tar64.dllによるZSTD書庫の処理に対応。(Special Thanks!:kiyohiro様)
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.24.08.00.01に更新。


 ○Ver.0.00r33 - 2017/09/26
 ・7-Zipの仕様変更('--'以降のリストファイル指定を処理しない)に対応。
 ・複数の再圧縮処理時にエラー書庫も削除してしまう不具合を修正。(Special Thanks!:通りすがりの人様)
 ・一部のTotal Commander Plugin(*.wcx)を用いた解凍処理で強制終了してしまう不具合を修正。(Special Thanks!:通りすがりの人様)
 ・複数ファイルの処理でエラーが発生した際、処理を中断し、続行か終了を選択出来る「/qe」オプションを追加。(Special Thanks!:kiyohiro様)
 ・名前に"%n"を含むファイルを処理する際に強制終了してしまう不具合を修正。(Special Thanks!:kiyohiro様)
 ・「lzhsfx」でLZH自己解凍書庫の作成ができない不具合を修正。(Special Thanks!:kiyohiro様)
 ・テスト実行する「/T」を実装。現在は「/mn」のみの対応で、リネーム処理前後のファイル名を表示します。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.17.01.00.01 betaに更新。


 ○Ver.0.00r32 - 2016/05/23
 ・圧縮対象がディレクトリであればディレクトリ名を考慮せずに拡張子を付加するように。(Special Thanks!:kiyohiro様)
 ・「/nl」を付加してb2e32.dllを呼び出すと強制終了してしまう不具合を修正。
 ・正規表現を用いるとパターンによってはフリーズしてしまう不具合を修正。
 ・正規表現ライブラリをSRELLからVC++のregexに変更。
 ・「/mn」「/mN」後の置換条件を省略すると、処理前に入力を求めるように。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.16.02.00.01 betaに更新。


 ○Ver.0.00r31 - 2016/01/23
 ・標準入力(パスワード入力、コマンド入力、リダイレクト)で日本語が使用できない不具合を修正。(Special Thanks!:kiyohiro様)
 ・Ctrl+Cを押下してもプロセスが終了しない不具合を修正。
 ・パスワード入力時にEnterのみの押下で処理がキャンセルできない不具合を修正。
 ・書庫内ファイルをリネームする「/mn」、その際に正規表現を用いる「/mN」を実装。
 ・引数分析で複数指定された文字列をソートしないように。
 ・gui4recesでログウインドウを表示すると正しくパスワードが入力できない不具合を修正。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.15.14.00.01 betaに更新。


 ○Ver.0.00r30 - 2015/10/21
 ・一部環境でフリーズする不具合を修正。(Special Thanks!:kiyohiro様)
 ・圧縮時拡張子を保持する「/ooke」を実装。
 ・ライブラリの優先順位について、「7-zip32.dll」を「UnIso32.dll」より優先するように。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.15.09.00.01 betaに更新。
 ・7z.dll対応版7-zip32.dll/7-zip64.dllに対応。
 ・マルチスレッド(/MT)でビルドするように。


 ○Ver.0.00r29 - 2015/08/18
 ・一部書庫で二重ディレクトリ判定が正しく行われない不具合を修正。
 ・同名の二重ディレクトリを防ぐ「/c3」オプションを追加。
 ・「/q /q」のように同じオプションを続けると解析終了の「//」と同じ扱いになる不具合を修正。
 ・「/c」での新規ディレクトリ作成時に末尾の' 'と'.'を削除するように。(Special Thanks!:kiyohiro様)
 ・7-zip32.dll/UnIso32.dllで出力先ディレクトリに2つ以上のスペースが含まれると1つに削られてしまう問題に対応。(Special Thanks!:kiyohiro様)
 ・7-zip32.dllを使用する形式で「/mC」するとディレクトリ構造を無視した内容と構造を維持した内容が重複して圧縮される不具合を修正。
 ・出力書庫ファイル名への拡張子付加処理について、圧縮形式拡張子ではなくライブラリ対応形式拡張子で要不要を確認していた不具合を修正。(Special Thanks!:kiyohiro様)
 ・圧縮時対象ディレクトリを再帰的に検索する「/R」オプションを追加。
 ・「/mr」「/mc」「/me」「/ml」でb2e32.dllに対応。ただしオプションは大幅に制限されます。
 ・「/Db」で相対パスも指定できるように。
 ・「/mv b2e」で使用できるb2eスクリプトの「<format>:<method>」の組み合わせを表示するように。
 ・「b2e64.dll」を同梱するように。
 ・「b2e/exe.msi.b2e」「b2e/mp3.b2e」を同梱するように。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.15.06.00.02 betaに更新。
 ・書庫のコードページを指定する「/C」オプションを実装。これに伴い、従来の「/C」は「/C:@」に変更。
 ・「/mv cp」で「/C」で指定できる文字コードの「コードページ:説明:文字セット」一覧を表示するように。
 ・リストの文字コードを指定する「/C:@」オプションの値を設定ファイルに保存できるように。
 ・文字をANSIに変換し出力する「/C:oa」オプションを実装。
 ・コンパイラをMicrosoft Visual C++ 2015に変更。


 ○Ver.0.00r28 - 2015/07/02
 ・rar書庫でディレクトリ階層を無視した解凍が出来なかった不具合を修正。
 ・rar書庫の解凍でダイアログが表示されていた不具合を修正。
 ・プログレスバーの更新が止まる場合がある不具合を修正。
 ・書庫の構造によってはフィルタ処理で関係のないディレクトリが除外される不具合を修正。(Special Thanks!:kiyohiro様)
 ・統合アーカイバライブラリについて/mtでライブラリが指定されなければ、CheckArchive()で決定するように。(Special Thanks!:kiyohiro様)
 ・既存ファイルがあれば圧縮前に予め削除する「/NF」オプションを実装。(Special Thanks!:kiyohiro様)
 ・unrar32.dll/unrar64j.dllの同梱を中止。
 ・7-zip32.dll/7-zip64.dll文字化け対策版 Ver.15.05.00.02 betaに更新。
 ・Code::Blocks用プロジェクトファイルの同梱を中止。


 ○Ver.0.00r27 - 2015/05/15
 ・「/UESC」オプションを削除。
 ・プログレスバーの更新頻度を下げ、処理を高速化。
 ・再圧縮時に2ファイル目以降の処理で指定したライブラリが読み込まれない不具合を修正。
 ・7-zip32.dllによる削除コマンドでのファイル削除が出来なくなっていた不具合を修正。
 ・書庫からファイルを削除する「/md」を実装。
 ・「/mr」では削除コマンドを使用しないように。
 ・ヘッダ暗号化7z書庫が「/ml」で一覧表示できない不具合を修正。


 ○Ver.0.00r26 - 2015/03/14
 ・ファイルやディレクトリ検索中にもCtrl+Cによる処理の中断を受け入れるように。
 ・/LARGEADDRESSAWAREオプションを付加してビルドすることで、32bit版で2GB以上のメモリが使用出来るように。
 ・lzhファイルへのフィルタ適用が逆であった不具合を修正。
 ・tar32.dll/unrar32.dll/XacRett.dllで/i:ad及び/x:adが反映されない不具合を修正。
 ・unrar32.dllのフィルタを厳密に適用するように。
 ・7-zip32.dllの/x:adで空のディレクトリが作成されてしまう不具合を修正。
 ・UnIso32.dllでディレクトリ階層無視の解凍が出来ない不具合を修正。
 ・圧縮時に/i:ad,/x:adが機能しない不具合を修正。
 ・設定ファイルから/x:aeを読み込めなかった不具合を修正。
 ・正規表現フィルタオプション「/I:r」「/i:r」「/X:r」「/x:r」を実装。
 ・「/i:」「/x:」以下のオプションを「:」で分割するように。
 ・フィルタ適用の結果、空となるディレクトリを作成しないように。
 ・プログレスバーの動作を改善。


 〇Ver.0.00r25 - 2015/02/09
 ・LMZIP32.dllのサポートを中止。
 ・64bit版を同梱するように。
 ・RuRuRu氏によるunrar64j.dll x64/ユニコード対応版を同梱するように。
 ・7-zip32.dll/7-zip64.dll文字化け対策版を同梱するように。
 ・UNBYPASS.DLLに対応(現在、UNLHA32.dllのみ)。
 ・ZBYPASSA.SPHに対応。
 ・/DIRTS(ディレクトリタイムスタンプ復元)をデフォルトで行わないように。
 ・/DIRTSと/UESCを値指定しないオプションに変更。
 ・Ctrl-Cでクラッシュしてしまう不具合を(極力)修正。
 ・進捗状況(処理中|済ファイル数/全ファイル数)を表示するように。
 ・アイコンファイルの容量を削減。


 〇Ver.0.00r24 - 2014/12/23
 ・/mtで対象書庫にライブラリが対応しているかの確認を行わないように。
 ・/msで先頭の引数がライブラリに渡されない不具合を修正。
 ・処理中メッセージで圧縮か解凍かを表示するように。
 ・/mr@で統合アーカイバ以外のライブラリを指定すると圧縮されない不具合を修正。
 ・/r時に入力ファイルパスを環境変数%FILEPATH%及び%FILEPATH[i]%に、ファイル数を%FILECOUNT%に代入するように。
 ・一部書庫で二重ディレクトリ判定が正しく行われない不具合を修正。
 ・フィルタ使用時に強制終了する不具合を修正。


 〇Ver.0.00r23 - 2014/10/13
 ・一部Susie Plug-inで対応しているにも関わらず処理されない不具合を修正。
 ・バックグラウンドモードを他のライブラリでも設定できるように。
   また、統合アーカイバライブラリの*etBackGroundMode()を使用しないように。
 ・書庫用Total Commander Plugin(*.wcx)に対応。
 ・ディレクトリ作成処理を改善。
 ・Susie Plug-in、Total Commander Pluginでの共通パス除外は解凍時に行うように。


 〇Ver.0.00r22 - 2014/08/31
 ・「/}」オプションで、ファイルが存在しないと出力出来ない不具合を修正。(Special Thanks!:kiyohiro様)
 ・一括圧縮で「/d[1|2]」オプションを使用すると先頭ファイル以外が削除されない不具合を修正。(Special Thanks!:kiyohiro様)
 ・「/r」のコマンド入力時、Ctrl+Cを入力すると不正終了する不具合を修正。
 ・圧縮時に対象と出力ファイル名が同名であれば削除しないように。


 〇Ver.0.00r21 - 2014/07/27
 ・ライブラリの設定ダイアログを表示する「/mS」オプションを実装。
 ・ライブラリをフルパスで指定した場合、プレフィックスの指定が行えない不具合を修正。
 ・一部ライブラリの解放し忘れを修正。
 ・検索対象外のspiを直接指定できない不具合を修正。
 ・再圧縮時に戻り値を設定していなかった不具合を修正。(Special Thanks!:胚芽様)
 ・Susie Plug-inによる解凍でログを表示するように。
 ・/mtでも戻り値を返すように。
 ・return codeを/niで非表示に出来るように。
 ・/mtのログも/nlで非表示にできるように。
 ・入力ファイルに対してのMAX_PATH確認を廃止。
 ・7-zip32.dllによる一部処理でMAX_PATH以上のパスが扱えるように。(7-zip32.dll Ver.9.22.00.01以上が必要)
 ・/me,/ml,/mtで入力ファイルがMAX_PATH以上のパスであれば7-zip32.dllを使用するように。
 ・DlgHook.dllを一時ディレクトリに書き出すように。
 ・書庫用Susie Plug-inかどうかの判定をファイル名ではなくGetPluginInfo()で行うように。
 ・解凍などでSusie Plug-inが指定された場合、/Dsで指定されたディレクトリも検索対象とするように。
 ・/me,mlで指定するライブラリ名に拡張子が含まれると処理されない不具合を修正。
 ・/oFで書庫の拡張子が含まれる場合、/oorによる「_[数字]」付加をファイル名部分に行うように。


 〇Ver.0.00r20 - 2014/04/26
 ・二重引用符を含むコマンドライン引数処理の不具合を修正。(Special Thanks!:kiyohiro様)
 ・unrar32.dll使用時のプログレスバー処理を高速化。
 ・「/eb[level]」オプション使用時、一部ディレクトリが正しく解凍されない不具合を修正。(Special Thanks!:Kazumi様)
 ・「/eb[level]」オプション使用時にディレクトリタイムスタンプの復元やUnicodeエスケープシーケンスのデコードが行われない不具合を修正。
 ・一部ライブラリで「/DIRTS0」指定が処理されない不具合を修正。
 ・LMZIP32.dllの書庫テスト機能を追加。


 〇Ver.0.00r19 - 2014/03/25
 ・書庫処理でエラーが発生した場合、メッセージを表示するように。
 ・ディレクトリのタイムスタンプ復元において、recesが落ちる場合がある不具合を修正。
 ・LMZIP32.dllに対応。これにより、7-zip32.dllでは文字化けする書庫も正しく解凍出来るように。ただし、圧縮処理は未実装。
 ・デフォルトでUnicodeエスケープシーケンスをデコードしないように。
 ・誤ったライブラリを指定するとフリーズしてしまう不具合を修正。
 ・再圧縮時、解凍に用いるライブラリを指定できるように。
 ・ディレクトリのタイムスタンプ復元を設定できる「/DIRTS」オプションを追加。
 ・Susie Plug-inの列挙において、関係のないファイルを含めてしまう不具合を修正。
 ・処理後終了ステータスを表示するように。
 ・LMZIP32.dll対応に伴い、ロケール識別子を指定する「/LCID」オプションを削除。


 〇Ver.0.00r18 - 2014/02/21
 ・Susie Plug-inの対応拡張子を一部取得出来ていなかった不具合を修正。
 ・複数書庫の個別再圧縮時に、前回の一時ファイルが含まれてしまう事がある不具合を修正。
 ・ディレクトリのタイムスタンプ復元において、recesが落ちる場合がある不具合を修正。


 ○Ver.0.00r17 - 2014/02/08
 ・XacRett.dllを用いた処理で一時ディレクトリのパスに空白が含まれる(XPなど)場合フィルタが使用できない不具合を修正。
 ・UNLHA32.dllを用いた解凍でフィルタの効果が逆になる不具合を修正。
 ・「/pw」オプションによるパスワード指定で二重引用符が正しく扱えない不具合を修正。
 ・フィルタをリストファイルから指定できる「/i:@」「/I:@」「/x:@」「/X:@」追加。
 ・Readme.txtにおける表記ミスを幾つか修正。
 ・カレントディレクトリのファイルに対してワイルドカードが使用できない不具合を修正。
 ・XacRett.dllを用いての解凍で処理ごとにrecesが最前面に表示される不具合を修正。


 〇Ver.0.00r16 - 2014/01/19
 ・複数の二重引用符+末尾'\'パスが処理出来ない不具合を修正。(Special Thanks!:kiyohiro様)
 ・「/oF」オプション指定でも拡張子を自動付加してしまう不具合を修正。
 ・出力ファイルが重複する場合リネームする「/oor」オプションを追加。
 ・「/ms」でライブラリのプレフィックスを指定出来るようし、対応外の統合アーカイバライブラリも扱えるように。
 ・Unicodeエスケープを含まない通常書庫の処理速度を改善。
 ・Unicodeエスケープを含むディレクトリのタイムスタンプ復元が出来ない不具合を修正。
 ・タイムスタンプを保持していないディレクトリは配下のファイルのタイムスタンプを利用して復元する様に。
 ・処理対象フィルタが正しく処理されない不具合を修正。
 ・7-zip32/UNLHA32を用いた圧縮でフィルタによって余計なディレクトリまで圧縮される不具合を修正。
 ・Susie Plug-inを用いた解凍でタイムスタンプ復元が正しく行われない不具合を修正。
 ・解凍時にも「/eb」オプションが使用できるように。
 ・共通パスを除外する「/eb[level|x]」オプションを追加。


 〇Ver.0.00r15 - 2014/01/03
 ・一部書庫において誤って二重ディレクトリと判定されてしまう不具合を修正。
 ・ロケール識別子を指定する「/LCID」オプションを追加、文字化け書庫対策。
 ・カレントディレクトリ基準で'/od','/of'の相対パスを処理する「/oob」オプションを追加。
 ・再圧縮時でも「/c」オプションが使用出来るように。(Special Thanks!:kiyohiro様)
 ・XacRett.dllによる解凍でもプログレスバーが表示できるように。
 ・XacRett.dllによる解凍でもログを表示するように。
 ・プログレスバーで一部ファイル名が表示されない不具合を修正。
 ・ディレクトリ指定で二重引用符+末尾'\'でも処理できるように。(Special Thanks!:kiyohiro様)
 ・rarの書庫判定処理を改善。
 ・7-zip32以外の書庫判定処理を簡易モードに変更。
 ・「/c1」「/c2」使用時に一部ディレクトリのタイムスタンプ復元が行われない不具合を修正。
 ・「/UESC」オプション追加。解凍/一覧出力時、Unicodeエスケープシーケンスのデコードが行えるように。
 ・7-zip32/uniso32でのディレクトリのタイムスタンプ復元を自前で行うように。
 ・ディレクトリのタイムスタンプ復元が正しく行われない不具合を修正。
 ・ヘッダ暗号化7z書庫の解凍を行うとフリーズ若しくは強制終了する不具合を修正。
 ・パスワードの入力で何も入力せずにEnterするとフリーズする不具合を修正。
 ・パスワードの入力前にウインドウのフォーカスが奪われる不具合を(極力)修正。
 ・作成する書庫と同名のディレクトリが存在する場合、/oFを無効にするように。


 〇Ver.0.00r14 - 2013/12/08
 ・設定ファイルに圧縮形式が保存されない不具合を修正。
 ・一部ファイルの圧縮について、「/t」によるタイムスタンプ保持が行われない不具合を修正。
 ・設定ファイルからタイムスタンプの保持設定「/t」(圧縮時のみ)が読み込めない不具合を修正。
 ・設定ファイルからライブラリのコマンドを用いての書庫内容出力設定「/mL」が読み込めない不具合を修正。
 ・tar32.dllとunrar32.dllでプログレスバーが正しく処理されない不具合を修正。
 ・同梱のunrar32.dll x64/ユニコード対応版をversion 0.16にバージョンアップ。
 ・サイズ指定でTBを表す「t」を追加。
 ・設定ファイルへの書き出しでUnicode文字を含む文字列を取り扱えるように。
 ・設定ファイルの既存のアイテムに対し、デフォルト値での上書きが行えない不具合を修正。
 ・b2eスクリプトがあるディレクトリを指定する「/Db」オプションを実装。
 ・ユーザ独自のパラメータを指定できる「/P」オプションを「/ms」でも使用出来るように。
 ・直接指定したライブラリ名を設定ファイルに「LibraryName」として書き出すように。
 ・日付フィルタで用いる数値の表現方法を変更。
 ・設定ファイルへの書き出しで落ちる不具合を修正。
 ・設定ファイルの「SplitByCount」キーを削除。
 ・書庫内ファイルの列挙に際して、一部ファイルが抜け落ちる場合がある不具合を修正。
 ・フィルタ使用時の処理を改善。(書庫によっては10倍高速化!?)
 ・拡張子から書庫形式を推測する様に。(r12,r13で実装し忘れ)
 ・「/!ca」オプションを削除。
 ・ダイアログのフック処理を改善。
 ・zip/7z書庫判定処理を改善。
 ・設定ファイルの「SameCompressionType」キーを削除。
 ・設定ファイルで指定したパスワードリストを読み込まない不具合を修正。
 ・圧縮/書庫内容表示時にパスワードリストを指定した場合、リスト先頭のパスワードを採用するように。
 ・分割ファイルでもタイムスタンプの保持が出来る様に。
 ・終了時「DlgHook.dll」が削除されない不具合を(極力)修正。
 ・7-zip32.dll系ライブラリで扱う一部書庫に就いて、構造を無視する設定でもディレクトリが作成されてしまう不具合を修正。
 ・ディレクトリ階層無視の解凍であれば、二重ディレクトリ防止機能を無効とするように。
 ・C-c押下時異常終了となる不具合を(極力)修正。
 ・パスワード付rar書庫の解凍に失敗すると、以降の書庫処理が行えない不具合を修正。


 ○Ver.0.00r13 - 2013/07/10
 ・メモリリークの不具合を修正。(Special Thanks!:kiyohiro様)
 ・/d2でディレクトリが削除できない不具合を修正。(Special Thanks!:kiyohiro様)
 ・「/S」オプションを削除。
 ・書庫をテストする「/mt」を実装。
 ・7-zip32.dllでヘッダ暗号化書庫が正しく取り扱えない不具合を修正。
 ・「/ms」でのみb2e32.dllに対応。
 ・「/m<r|e|l|t|s>」のライブラリ指定でパスを取り扱えるように。
 ・「/ml」で「i:ad」「x:ad」が正しく動作しない不具合を修正。
 ・書庫用Susie Plug-inに対応。
 ・Susie Plug-inがあるディレクトリを指定する「/Ds」オプションを実装。
 ・「/d」オプションを有効にすると誤って「/e」オプションも有効になる不具合を修正。(Special Thanks!:kiyohiro様)
 ・一括再圧縮で、「/t」で使用するタイムスタンプを先頭のファイルから取得していなかった不具合を修正。
 ・圧縮時でも「/t」オプションが使用できるように。これに伴い、設定ファイルのセクション名をCompressに変更。(Special Thanks!:kiyohiro様)
 ・再圧縮後の削除対象が元書庫と同じパスであれば削除しないように。(Special Thanks!:kiyohiro様)
 ・再圧縮で拡張子が小文字でなければ削られない不具合を修正。
 ・「/ms」でのコマンドライン解析処理の不具合を修正。
 ・ArcXacRettの対応拡張子にcabが含まれていなかったのを修正。(Special Thanks!:kiyohiro様)
 ・「/mt」でXacRett.dllを使用しないように。
 ・「/ms」でライブラリが負の値を返した場合もログを表示するように。
 ・CheckArchive()のモードを指定する「/!ca」オプションを追加。
 ・エラーメッセージを修正。
 ・コンパイラをMicrosoft Visual C++ 2010 Expressに変更。


 ○Ver.0.00r12 - 2013/06/19
 ・ソースコードを全て書き直しました。
 ・リストファイルからの読み込みが正しく行われない不具合を修正。
 ・プログレスバー表示時にカーソルを非表示にするように。
 ・recesからReadme.txtを開かないように。
 ・ライブラリのバージョン表示でメジャーバージョンかマイナーバージョンが0の場合バージョンが表示されず「OK」となる不具合を修正。
 ・tar32でのlzma圧縮コマンドを誤っていた不具合を修正。
 ・拡張子「jar」の解凍を7-zip32.dllで行うように。
 ・unrar32.dllの処理でプログレスバーが表示されない不具合を修正。
 ・書庫処理全体に対するプログレスバーを表示するように。
 ・「/ni」有効時も書庫ライブラリのコールバックを利用するように。
 ・tar32.dllによる圧縮で、「/l」オプションを使用しなければ全てtarのみのアーカイブ化となっていた不具合を修正。
 ・「/oF」「/s」オプションを同時に使用し圧縮した場合、階層が誤って出力されるなどの不具合を修正。
 ・「/oF」で拡張子を付加せず、指定されたファイル名のまま出力出来るように。
 ・ライブラリに送るコマンドを表示しないように。
 ・メッセージを全て日本語で表示するように。
 ・「DlgHook.dll」をreces本体に埋め込み、起動時に書き出すように。
 ・分割書庫の再圧縮で、拡張子を削り忘れていたのを修正。
 ・各書庫操作ライブラリにコマンドでパスワードを直接渡さないように。
 ・7-zip32.dll/UNLHA32.dllで作成される書庫にディレクトリ情報が含まれない不具合を修正。(r9までは含まれていました)
 ・UNLHA32.dllによる階層無視圧縮で「/i:ad」フィルタが指定されている場合、ファイルの検索に失敗する不具合を修正。
 ・解凍処理でディレクトリのタイムスタンプが含まれていれば復元するように。
 ・二重ディレクトリ判定処理を改善。
 ・cab32.dllのサポートを中止。
 ・プログレスバー表示処理を改善。
 ・フィルタオプションの指定を「:」なしでも処理できるように。
 ・ワイルドカード処理を変更。
 ・同名書庫が既に存在する場合、デフォルトで追加圧縮するように。
 ・同名書庫が既に存在する場合、新規作成(上書き)を行う「/N」オプションを追加。
 ・基底ディレクトリを含まない圧縮を行う「/eb」オプションを追加。
 ・「/of」でパス情報も取り扱えるように。
 ・ライブラリの直接操作を行う名称を「Direct」から「SendCommands」に変更。これに伴い、従来の「/md」は「/ms」に変更。
 ・「/ms」(旧:「/md」)でライブラリに送るコマンドに含まれるワイルドカードを展開しないように。
 ・「/nt」オプションを廃止、usage表示時のみタイトルを表示するように。
 ・サイズを表す文字が含まれていようと分割数指定として処理する「/S」オプションを追加。
 ・作成不可能なzip自己解凍書庫の指定を受け付けないように。
 ・パスワード付7z自己解凍書庫作成の際は7z.sfxを使用するように。
 ・エラーメッセージを非表示にする「/ne」オプション追加。
 ・/m[r|R]@で対象ファイル拡張子からの推測ではなく、書庫形式を取得して処理するように。
 ・ヘッダが暗号化された書庫の解凍を正常に行えるように。
 ・7-zip32.dllを使用した複数ディレクトリ/ファイルの圧縮を一度に行えるように。
 ・Ctrl+Cで処理を中断させた場合の後始末を改善。
 ・パスワードの入力がキャンセルされた旨のメッセージを可能であれば表示するように。
 ・対象ファイルをソートせず指定された順番で処理するように。
 ・パスワードダイアログのフック処理を改善、recesがフォアグランドになるまでのループ処理を削除。
 ・「/mV」オプションを廃止。
 ・色付文字出力時に背景色を変更しないように。
 ・入力ファイルに対してMAX_PATHを超えていないか確認するように。
 ・新規作成するディレクトリの更新日時を書庫と同じにする「/ct」オプションを追加。
 ・再圧縮後書庫の更新日時を元書庫と同じにする「/t」オプションを追加。
 ・「/ml」オプションでもディレクトリを表示するように。
 ・再圧縮で可能であればライブラリの削除コマンドを使用するように。
 ・ユーザ独自のパラメータを指定できる「/P」オプションを追加。
 ・圧縮形式にヘッダ暗号化7z書庫関連の「7zhe」「7zhesfx」を追加。
 ・lzh自己解凍書庫作成の際に設定ウインドウダイアログを表示するように。
 ・再圧縮後のファイル名で必要以上に拡張子が削られる不具合を修正。
 ・空ディレクトリのみの書庫を再圧縮できない不具合を修正。
 ・再圧縮で自己解凍書庫が作成されない不具合を修正。
 ・「/ms」を省略できる機能を廃止。
 ・UNLHA32.dllでの「mL」を簡易表示から詳細表示に変更。
 ・「mL」でも一部ライブラリに就いてはフィルタを適用するように。
 ・unrar32.dllでの書庫内ファイルの指定に関する不具合を修正。
 ・書庫内ファイルが半角スペースを持つ場合、正しく処理されない不具合を修正。
 ・日付フィルタで月日を指定しなければ正しく処理されない不具合を修正。
 ・一度読み込んだライブラリは全ての処理が終了するまで解放しないように。
 ・XacRett.dllの「/mL」で更新日時、属性、サイズ、名前をreces側で取得して表示するように。
 ・リダイレクト時のパスワード要求メッセージを標準エラー出力で表示するように。
 ・ログが長すぎる場合コンソールに出力されない不具合を修正。
 ・Readme.txtにおける表記ミスを幾つか修正。


 ○Ver.0.00r11 - 2013/01/21
 ・再圧縮で解凍と圧縮の間に任意のコマンドを実行できる「/r」オプションを追加。
 ・リダイレクトが正しく行えるように。
 ・パスワードの入力で、何も入力しなければキャンセル扱いとするように。
 ・パスワードの入力前後でウインドウのフォーカスが奪われる不具合を極力修正。
 ・パスワードの入力で、書庫Dllのダイアログが表示される不具合を修正。
 ・標準入力からファイルを指定した場合、パスワードの入力が出来ない不具合を修正。
 ・エラーメッセージをX座標0で表示するように。
 ・入力書庫(パスワード付)と同じ形式で処理する「/mr@pw」オプションを追加。
 ・再圧縮で解凍時のパスワードを引数から指定しないと、再度入力を求められる不具合を修正。
 ・処理をキャンセルした際、一時ディレクトリやリストファイルを削除するように。
 ・lzh書庫で「/c1」「/c2」オプションが使用できない不具合を修正。
 ・分割書庫作成で書庫のファイルサイズが分割サイズを下回るとファイルが作成されない不具合を修正。
 ・分割後ファイルの出力先が処理元ファイルと同じ階層ではなくカレントディレクトリとなっていた不具合を修正。
 ・複数ファイルの解凍で「/c1」「/c2」を指定すると2番目以降のファイルでディレクトリが作成されない場合がある不具合を修正。
 ・「/oF」指定時に出力されるファイルが正しくない不具合を修正。


 ○Ver.0.00r10 - 2012/11/14
 ・コードを全て書き直しました。
 ・UnAceV2J.DLL、Bga32.dll、UnGCA32.dll、Yz1.dllのサポートを中止。
 ・tar32.dll、UNLHA32.DLLの再圧縮及び圧縮機能のサポートを再開。
 ・UnIso32.dllのサポートを再開。
 ・「/d」オプションの指定したファイルを削除する機能を廃止。代わりに元ファイルを削除する機能を割り当て。
 ・「:I」「:i」「:X」「:x」オプションを追加。処理対象や処理対象除外を指定することが出来ます。
 ・再圧縮/圧縮形式の指定方法を変更。
 ・ディレクトリ階層を無視する「/mR」、「/mC」、「/mE」オプションを追加。
 ・書庫内ファイルの一覧を出力する「/ml」、「/mL」オプションを追加。
 ・「/l」で最低圧縮率、「/lx」で最高圧縮率を指定出来るように。
 ・ファイル解凍の際に、拡張子から書庫形式を推測するように。
 ・「/h」オプションを廃止。デフォルトで非表示に。
 ・プログレスバーをrecesで出力するように。
 ・パスワードの入力をrecesで求めるように。
 ・パスワードを非表示にする「/np」オプションを追加。
 ・reces本体のファイル名を各書庫操作ライブラリ名にリネームすることで、「/md」を省略して呼び出せるように。
 ・reces本体のファイル名末尾に「reces_me」の様に「/m」オプションを付加できるように。
 ・処理対象をリストファイルに書き出して指定するように。
 ・処理対象をリストファイルから指定できるように。
 ・リストファイルの文字コードを指定できる「/C」オプションを追加。
 ・処理対象を標準入力から指定できるように。
 ・設定をファイルから読み込む「/{」オプションを追加。
 ・設定をファイルに書き出す「/}」オプションを追加。
 ・7-zip32.dllによるCheckArchive()の優先度を下げました。
 ・「/me」で指定したライブラリがCheckArchive()でfalseを返す場合、終了せずに各書庫操作ライブラリで試行するように。
 ・処理時間の表示を廃止。
 ・書庫形式の表示を廃止。
 ・「/v」オプションを廃止。「/mv」オプションとして実装。
 ・「/tr」、「/tn」オプションを廃止。同等の機能を「/c」オプションに統合。
 ・ファイルが一つしか含まれない書庫では新規ディレクトリを作成しない「/c1」オプションを追加。
 ・「/a」オプションを廃止。一括圧縮をデフォルトにし、個別圧縮を行う「/e」オプションを追加。
 ・「/n」オプションの数字指定を廃止。
 ・「/od」オプションでディレクトリが指定されていない場合、デスクトップに出力するように。
 ・「/of」オプションでパスを無視するように。(「/od」オプションでお願いします。)
 ・「/of」オプションで自動的に拡張子を付加するように。
 ・拡張子を自動的に付加しない「/oF」オプションを追加。
 ・「/pw」オプションを複数指定出来るように。
 ・「/s」オプションの自己解凍書庫作成機能を廃止。代わりにファイルを分割する機能を割り当て。
 ・「/u」オプションを廃止。
 ・「/w」オプションを廃止。デフォルトで上書きするように。
 ・進捗状況やエラーを日本語で表示するように。
 ・RuRuRu氏によるunrar32.dll x64/ユニコード対応版を同梱するように。
 ・一部Dllのダイアログに対しフックするように。
 ・作者HNを変更。
 ・NYSL Version 0.9982を適用。


 ○Ver.0.00r9 - 2010/10/03
 ・Unarj32j.dll,Unbel32.dll,UnHki32.dll,UnIso32.dll,ISH32.DLL,Jack32.dll,Paq32.dll,
   unzip32.dll,ZIP32J.DLL,gcac.exe,dgcac.exe,MsiX.exe,UHARC.EXE,JPZ.dllのサポートを中止。
 ・Bga32.dll,tar32.dll,cab32.dll,UnHki32.dll,UNLHA32.dll,Yz1.dllの圧縮及び再圧縮機能を廃止。
 ・/de,/dfオプションを廃止し、/dオプションを復活。ワイルドカードが使用出来る様になりました。
 ・/tr,/tnオプションを追加。二重フォルダ防止や末尾の数字削除を行う事が出来ます。
 ・/gオプションを廃止。
 ・配色を暗めに変更。
 ・archiveとModeの指定を省略した状態でオプションを指定すると、処理が開始されない不具合を修正。
 ・同じ書庫形式に再圧縮する場合、指定したファイルが削除されない場合がある不具合を修正。
 ・/aオプションが有効且つ/ofオプションが無効な場合、出力ファイル名を先頭の対象ファイルに合わせるように。
 ・引数なしで実行した場合、getchar()ではなくgetch()するように。


 ○Ver.0.00r8 - 2010/05/08
 ・再圧縮の際、一時解凍ディレクトリをGetTempPath()で取得するように。


 ○Ver.0.00r7 - 2010/04/05
 ・引数無しで実行した場合、getchar()するように。
 ・複数ファイルの取り扱いに暫定的に対応。
 ・/aオプションを追加。
 ・圧縮対象ファイルをフルパス指定した場合でも、サブディレクトリ以下にある同名ファイルも圧縮対象となるバグを修正。
 ・書庫を圧縮する際、/uオプションが無ければ、パスワードが付加できないバグを修正。(Special Thanks!:すがりすがりくん様)


 ○Ver.0.00r6 - 2010/02/11
 ・パスワードを扱う処理が出来なくなっていたバグを修正。
 ・lzop.exeを非対応に。これにより、lzo/tzo書庫ファイルを圧縮/解凍する事は出来ません。
 ・lzhで圧縮する際、隠し属性やシステム属性と言ったファイルが扱われないバグを修正。
 ・/dオプションを廃止し、/de,/dfオプションを追加。因みに、同時指定可能。
 ・文字色を一部変更。


 ○Ver.0.00r5 - 2010/01/20
 ・再圧縮機能を実装。
 ・メモリの確保が不十分な為、全ての場合において正常に処理されない可能性があるバグを修正。
 ・解凍対象にフォルダを指定した場合、不正終了するバグを修正。
 ・/dオプションを追加。いつかsystem()を使わずに書きます...
 ・/gオプションを追加。デフォルトではプログレスが表示されません。
 ・/nオプションを拡張。新たに、Infoを非表示に出来るように、他。
 ・/uオプションを追加。
 ・/vオプションでreces本体のバージョンも確認できるように。
 ・/vオプションでunrar32.dllを表示するように。
 ・/wオプションを追加。
 ・ログが無い場合、"Log:"を表示しないように。
 ・JPZ.DLLを指定する場合、複号ファイルの出力先をカレント限定に。
 ・tar32.dllを使用する場合、常に同一ファイルを上書きしていた処理を、/wオプション無しでは出来ないように。
 ・tar32.dllを使用する場合に、/hオプションを指定すると正常に処理されないバグを修正。
 ・Bga32.dll,UNLHA32.DLLを使用して圧縮する場合に、正常に処理されないバグ、/hオプションを指定しても動作しないバグを修正。
 ・Yz1.dllを使用して圧縮する場合、カレント以下が含まれない書庫が作成されるバグを修正。
 ・gcacを直接指定した際に、正常に処理されないバグを修正。
 ・EXEの実行にCreateProcess()を使用するように。
 ・UnImp32.dll、nz.exe、ARJ32.exeのサポートを中止。
 ・tzoの圧縮に対応。(ただし、解凍は一度で出来ません)
 ・作成者HNを変更。
 ・アイコンを設定。(Special thx! なつだ氏)


 ○Ver.0.00r4 - 2009/12/18
 ・圧縮機能を実装。
 ・ArcTypeを表示する際に、不正終了するバグを修正。
 ・ArcTypeで正常に表示されない書庫があるのを修正。(Bga32.dll,tar32.dll関係)
 ・Rk.exe,base64arc.dll,Oar32.dllのサポートを中止。
 ・/oオプションを廃止し、/od,/ofオプションを追加。
 ・/pオプションを廃止し、/pw,/pfオプションを追加。
 ・対象書庫のパスに空白が含まれる場合、正常に処理されないバグを修正。
 ・/cオプションにて、ディレクトリ名が重複する場合、エラーが発生するバグを修正。
 ・開発環境を変更。


 ○Ver.0.00r3 - 2009/11/27
 ・/cオプションを追加。
 ・/meオプションでEXEファイルが指定可能に。
 ・/meオプションでXacrett.dllを指定するとPaq32.dllと認識されるバグを修正。
 ・Jak書庫を正常に解凍できないバグを修正。
 ・Base64ファイルを正常に解凍できないバグを修正。
 ・解凍オプションが無い場合、コマンドラインに不要なスペースが含まれるバグを修正。
 ・Rk.exe,ARJ32.EXE,Msix.exe,AISH32.DLLに対応。
 ・CheckArchive()で総当り処理中にプログレスを表示するように。


 ○Ver.0.00r2 - 2009/11/23
 ・/hオプションを追加。
 ・パスワードが正しく処理されないバグを修正。
 ・EXEを使用するファイルが正しく処理されないバグを修正。
 ・対象書庫が7zの場合、ArcTypeが正常に表示されないバグを修正。
 ・DLL,EXE名を直接指定して処理できるように。(ただし、アーカイバEXEは指定できない。)
 ・JPZ.dll、loliconv.exeに対応。(ただし、DLL、EXE名を直接指定しないと処理されない。)
 ・バージョン情報を付加。
 ・デザインを少しだけ改良。


 ○Ver.0.00r1 - 2009/11/19
 ・解凍機能,バージョン確認機能のみ実装。


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
This Readme file made by x@rgs
