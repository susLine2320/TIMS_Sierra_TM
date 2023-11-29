# TIMSプラグイン（東京メトロ互換）
このファイルはろくらいん式うさプラ用TIMSプラグイン（TIMS_new_TM.dll）の説明書です。

## ダウンロード方法
右側の[リリースページ](https://github.com/susLine2320/TIMS_sierra_TM/releases)より、最新のバージョンを選択し、zipファイルをダウンロードしてください。

## データについて
Unicorn氏が以前公開されていたTIMS.dllを、エラーが出ないようにコード追加を行ってkikuike氏が自炊した自炊TIMSプラグインを、全文書き直しの上ライセンスを緩和したものです。

このプラグインは、JR・相鉄線内における通常の電車型TIMS画面表示の模擬、およびその他の路線における簡易型のTIMS画面表示の模擬（メトロ総合プラグイン・小田急線用プラグインのTIS画面表示と互換性あり）、及びメータ画面の模擬を行います。メトロ総合プラグインとの併用が前提です。**本プラグインはメトロ総合プラグインの後に読み込むと正常に動作します**。

BVE Trainsim 6 + Windows10での動作を確認していますが、恐らくBve5.4以降でも動くと思われます。

このプラグインはGPLライセンス適用ではないので、各車両ファイルに同梱してお使いください。

## 仕様について
0.04bより、大幅に仕様が変更され、以下の通り分割されました。
- 列車用(一般型車両MON・TIMS・INTEROS列車タイプ3段表示、E331系AIMS4段表示、E259系・E657系・E353系TIMS5段表示、E257系時刻情報6段表示に対応)
- 電車用(一般型車両MON・TIMS・INTEROS行路表タイプ5段・7段表示に対応)

に分割されました。詳細は仕様書・TIMS_new内の初期値一覧をご覧ください。

本プラグインでは両仕様を設定ファイルにて切り替えることが可能です。自炊TIMSプラグインのようなプラグインの違いはありません。

なお、snp2プラグインの端子台とは大幅に異なります。
以前Unicorn氏が公開されていた内房線TIMS化構文(E233系5000番台+snp2プラグインセットに同封、現在DL不可)や、oshimatyuuou氏が公開されていた京葉線TIMS化構文(現在DL不可)等では正常に動作いたしませんので、ご了承ください。

### 仕様詳細
- [x] メーター表示器
- [x] TIMS画面（電車用）
- [x] 誤通過防止装置
- [x] 交直切換・電圧
- [x] iniファイルによる設定切り替え（現状ReadMeで詳細な解説ができていないので、リリース付属のiniファイルを見てください）
- [x] **外部マスコンキーによる表示内容切り替え**

## 設定番号
### panelインデックス
- 端子台を参照すること。一部の機能はiniファイルの設定により有効・無効化される。
- DigitalNumber形式の画像の詳細については付属のファイル「Beacon(for JR Line).pdf」を参照すること。

### soundインデックス
- 端子台を参照すること。一部の機能はiniファイルの設定により有効・無効化される。

### beaconインデックス
- JR・相鉄線内のインデックスは（列車番号を除き）通常の自炊TIMSプラグインと同一。付属のファイル「Beacon(for JR Line).pdf」を参照すること。
- JR・相鉄線内以外のインデックスは下記の通り。なお端子台にも同様の内容がある。

| INDEX |表示|設定値|備考|
| ---: | :---: | :---: | :---: |
|10|駅名表示設定|駅番号を設定|senddataを0にしないこと
|70|走行区間設定|駅番号をつなげた4桁の整数|index10とは併用不可。小田急線内用
89|方向設定|A線/下り線:奇数、B線/上り線:偶数。|31～:下り江ノ島線、51～:下り多摩線・上り新宿行き（9号線の場合）
104|TIMS運行パターン設定|発駅番号3桁*1000+着駅番号3桁|JR線内では使用不可
105|次駅接近|停止位置までの距離+更新実施可否*1000000|更新を行わない場合、603番を併用すること
117|TIMS行路表矢印|矢印画像のシフト番号 (0～n) を設定|駅間は4等分を推奨
127|降車駅駅名|駅番号を設定|
603|駅名割込み設定|駅番号を設定|踏むと瞬時に設定した駅名になる
604|駅名表示設定|駅番号を設定|他PIに影響を及ぼさない独自地上子

### signalインデックス
- 使用しない

### SWインデックス
- 使用しない

### 設定ファイル
ファイル名と同名の設定ファイルが読み込まれる（デフォルトTIMS_new_TM.ini）ここでは以下の設定が可能。

| Section |Key|設定値|既定値|説明|
| :---: | :---: | :---: | ---: | :---: |
|Emulate|ebCutPressure|減圧時間[ms]|`1000`|ブレーキ減圧時間
|Emulate|lbInit|使用`1`、不使用`0`|`1`|初回起動時力行遅延
Disp|useLegacyDisp|各路線モード使用`0`、**TIMS_new_9.dll**互換`1`、**TIMS_new_E.dll**互換`2`|`0`|互換モード
Disp|UnitDispEnable|TIMSユニット表示灯`1`、ユニット表示灯`2`、不使用`0`|`0`|ユニット灯使用可否
Disp|EnableMeterDisp|使用`1`|`1`|メーター表示器使用可否
Disp|EnableVmeter|使用`1`|`1`|電圧関連の表示可否
Disp|EnableTIMSDisp|使用`1`|`1`|TIMS行路表の表示可否
Disp|AMType|片振り`1`、両振り`0`|`0`|電流計仕様
Disp|DispType|東西`5`、南北`7`、千代田`9`、副都心`13`|`9`|使用路線

そのほか、TIMS_sierraにある設定項目は大体使えます。詳しくはwikiを参照ください。

GPL時代と項目名が大幅に変更されていますので注意してください。

### その他
- 詳細は[Excelシート](https://docs.google.com/spreadsheets/d/1rsIOkY-5uuszAekwyyYBjYd1NrkQBQti/edit?usp=sharing&ouid=117001858782449869576&rtpof=true&sd=true)を参照し、シートを参考に設定すること。

## 著作権について
基本的にライセンスはGNU Lesser General Public Licenseによります。

従って当プラグインは、自由に実行し、動作を調べ、ソースコードを改変し、複製物を再頒布したり、ソースコードを改変したものを公開することができます。商用利用も可能なようですが、BVE界の風潮的にやめておくべきだと思います(コミケ等で当プラグインを含んだ車両データを頒布する、という場合とかは認められると思いますが)。**なお、LGPLではGPLと異なり、車両に同梱して使っても、プラグイン以外の部分にはLGPLは適用されませんので、同梱して使っていただいて結構です。**（ちなみにGPLの場合、別途ダウンロードさせる形を取ることが多いが、厳密にはそれでGPLを逃れることはできないらしいです。要はBVEのプラグインに適用するのには不向きなライセンスということです）

ライセンスによってソースコードを同梱します。LGPLに従っていただければ煮るなり焼くなりどうぞ。有用な機能はぜひ付属させたいと考えているので、[プルリク](https://github.com/susLine2320/TIMS_sierra_TM/pulls)を送っていただけるとありがたいです。その他、実装してほしい機能などありましたら、[issues](https://github.com/susLine2320/TIMS_sierra_TM/issues)を利用して連絡してください。

当プラグインを同梱する車両データの公開に対し、事前許可は不要ですが、当方における把握のために、必ず事後連絡をお願いいたします。

これらの著作権はATSplugin.hに対してmackoy氏、Rock_on氏に、それ以外のソースコードに対してろくらいんが著作権を持ちます。

## 免責事項
このデータを使用したことによって発生したいかなる損害について作者はその責を負いません。

## 更新履歴（自炊TIMSより続行）
- 2017/06/18	：0.01b版リリース。路線図に手を付けたいです。
- 2017/07/08	：0.02版リリース。ゴタついたので追記入れときました。
		プラグイン自体に変更はありません。
- 2017/07/21	：0.03a版リリース。行路表矢印・行先・運行パターン表示実装。
		試用版なのでオマケ同梱はありません。
- 2017/07/28	：0.031a版リリース。種別と行路表矢印を入換。
		試用版なのでオマケ同梱はありません。
- 2017/07/31	：0.032a/0.033a版リリース。一部構文見直し。
		試用版なの(ｒｙ
- 2017/08/06	：0.033b版リリース。やっとまともに出せるようになったよ
		社線の動作の確認取れたから箱根行きたい。
- 2017/11/20	：0.04a版リリース。7段表示に対応しました。
		いろいろ変わったのでできれば開発ノートを。
- 2017/12/27	：0.04b版リリース。電車と列車を分離。
		もはや0.04とは言えないので0.05にしようかなぁ。
- 2018/01/06	：1月6日は霊夢の日記念でリリース。
		開発ノートとかどうなってんだろうね。
- 2018/02/09	：0.05a版リリース。電列完全分離。
		今回で一応行路表完成。しかしここまで使用データなし。
- 2018/02/18	：build2をリリース。交直切替で直流側に入れない問題を修正。
		Snp2や純正TIMSもこの問題がある模様。
- 2018/03/11	：build3をリリース。TIMSモニタの初期値を10へ、未読込を0に変更統一。
- 2018/03/18	：build4をリリース。build3のバグを修正、通過設定の仕様を変更。
- 2018/04/01	：0.05正式版リリース。正式に電列分離して双方とも使用可能に。
- 2018/04/08	：0.051版リリース。Panel配置ミスを修正。
- 2018/07/25	：0.10版リリース。駅番号3桁化、採時駅・降車駅のプッシュアップ対応等。
- 2018/10/22	：0.11a版リリース。次駅・採時駅のみ更新モード実装、列番のバグを修正。
- 2018/12/11	：build2リリース。プッシュアップのデフォルト修正。
- 2019/01/06	：0.11版リリース。バグ修正と細かい調整など。
- 2019/02/08	：0.12a版リリース。列番のバグ修正。次駅停車灯の挙動変更。
- 2019/03/16	：0.12版リリース。多分次更新からは開発環境を一新します。
- 2019/07/28	：0.13版リリース。再度バグ修正(次行路列番)。
- 2020/01/31	：0.14版リリース。9号線対応専用版。
- 2020/04/27	：0.15版リリース。再度バグ修正(次採時駅番線)。
- 2020/08/26	：0.16版リリース。裏機能追加(力行表示)。
- 2023/02/15    ：0.16TM版　メトロ総合プラグイン併用でTIMS全機能使用可に改修。社線内のTIMS画面表示をTIS地上子互換に変更。これにより社鉄跨ぎも可能。
- 2023/03/28    ：0.16.1TM版　電流計仕様を改修。
- 2023/04/08	：build2をリリース。全コード書き直しによる脱GPL化。一部バグ修正。
- 2023/04/09	：1.00を公開。kikuike氏のタスクリストの内容は大体やったので正式版にしました。
- 2023/04/23	：1.01を公開。
- 2023/05/30	：1.02を公開。
- 2023/07/16	：1.03を公開。うさプラ版において脱GPL化。
- 2023/07/21	：マイナー更新。
- 2023/11/29	：1.04を公開。9号線モードのグラスコックピット機能を分離。**次バージョンでグラスコックピット機能を分離します。**

## Author
- Twitter [@16000land](https://twitter.com/16000land)
- Web [Github.io](https://susline2320.github.io/)
- Mail [GMail](chiyoda16122@gmail.com)

## このプラグインに関するContact
- Mail [Gmail](chiyoda16122@gmail.com)
- issues [Github](https://github.com/susLine2320/TIMS_Sierra_TM/issues)**おススメ**

©Line-16 2018-2023