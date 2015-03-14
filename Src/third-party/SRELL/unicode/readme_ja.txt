■同梱物について

1. ucfdataout.cpp

  srell_ucfdata.hppの最新版を作成するプログラムのソースファイルです。SRELLは
case-insensitiveな（大文字小文字の違いを無視した）照合を行うために、この
srell_ucfdata.hppを必要とします。

  ucfdataoutは、Unicode Consortiumより提供されているCaseFolding.txtというテキ
ストデータからsrell_ucfdata.hppを自動生成します。

  +---------------------------------------------------------------------------
  | Note 1: CaseFolding.txt
  |
  |   Case-insensitiveな照合を行う際には、大文字小文字の違いを吸収するために
  | "case-folding" と呼ばれる処理が行われます。Unicode規格に基づいた
  | case-foldingを行うために、Unicode Consortiumから提供されているのが
  | CaseFolding.txtです。
  |
  |   このデータファイルはUnicode規格がアップデートされるとそれに合わせてアップ
  | デートされる可能性があります（新たに追加された文字の中に、大文字小文字の別
  | を有するものがなければそのまま）。
  |
  +---------------------------------------------------------------------------

  1-1. 使用方法

    1) ucfdataout.cppをコンパイルします。
    2) 最新版のCaseFolding.txtを次のURLより取得します。
       http://www.unicode.org/Public/UNIDATA/CaseFolding.txt ,
    3) CaseFolding.txtと、1)で作成したバイナリとを同じファイルに置いてバイナリ
       を実行します。
    4) srell_ucfdata.hppが生成されますので、それをSRELLの置かれているディレク
       トリへと移動させます。

