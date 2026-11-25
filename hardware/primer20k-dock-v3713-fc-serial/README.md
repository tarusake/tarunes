# Tang Primer 20K Dock V3713 - FC NROM serial cartridge reader

Tang Primer 20K Dock V3713を残したまま、実ファミコンカートリッジの
PRG ROM/CHR ROMを起動時に読み出すためのDock直挿しアダプタ基板。

## 対応範囲

- 日本版FC 60ピンカートリッジ
- Mapper 0（NROM-256、PRG ROM 32 KiB＋CHR ROM 8 KiB）
- Dock PMOD0への1コネクタ直挿し（ケーブル不要）
- PMOD1/2/3位置を一続きに切り欠き、別PMODをDockへ直接挿せる
- Dockの3.3 Vから基板上で5.0 Vを生成
- カートリッジ電源をFPGAからON/OFF
- 電源OFF時はレベル変換ICから全74HC595出力をHi-Z化し、逆給電を防止

本基板はNROMカートリッジのPRG/CHR ROMを起動時に読み出す。ダンプ後に
カートリッジ電源を切り、tarunes本体は取得済みイメージから動作する。
カートリッジバスのリアルタイムパススルーやMMC書込み回路は実装しない。

## Dock接続

DockのPMOD0へ直接挿す。HDMI、Ethernet、
USBデバッガ、オーディオ出力には干渉しない。

`ciniml/TangFPGAExtensions` の `TangPrimer_PmodExtender.kicad_pcb`を基準にする。
Dock側4コネクタは同一Yで隣接間隔19.10 mm。J2をPMOD0へ挿し、
PMOD1/2/3位置をまとめて露出する幅47.5 mm、深さ15 mmの切り欠きを設ける。
J2は秋月 `PH-2x40SG(6.1/6.1)`（販売コード112180）から2x6を1個切り出し、
基板下向きに手実装する。6.1 mmの嵌合長を確保し、JLCPCBの実装対象からは除外する。
JLCPCBで波はんだ実装まで依頼する場合の代替は
`PZ254-2-06-Z-8.5`（C2894967、嵌合長6 mm）だが、本リリースBOMでは採用しない。

PMOD3には既存のPMOD-SFCを切り欠きからDockへ直接挿せる。tarunesの既存配置は
`sfc_data=N8`、`sfc_clk=N7`、`sfc_latch=B11`で、いずれもJ5に出ている。
J5はRGB LCD端子と信号を共有するため、PMOD-SFC使用中はRGB LCDを接続しない。
HDMI出力とは競合しない。

PMOD-SFCパススルー回路は実装しない。PMOD-SFCへの電源と信号はすべて
Dock PMOD3から直接供給される。

PMOD1/2/3には本基板から電源も信号も接続しない。

| FPGA ball | Adapter signal | Direction |
|---|---|---|
| P9 | CART_SCLK | Dock -> adapter |
| T9 | CART_MOSI | Dock -> adapter |
| T8 | CART_RCLK | Dock -> adapter |
| P8 | CART_PL_N | Dock -> adapter |
| R8 | CART_MISO | adapter -> Dock |
| T6 | CART_PWR_EN | Dock -> adapter |
| P6, T7 | NC | - |

J14 pin 6/T9はDock上のWS2812にも接続されているため、ROM読み出し中にLEDが
点滅する。電気的競合はない。GowinプロジェクトではT9に
`set_option -use_sspi_as_gpio 1`が必要。

## シリアルプロトコル

秋月TC74HC595AFを5個（40 bit出力）、TC74HC165APを2個（16 bit入力）として
使用し、すべて5 Vで動作する。Dockの3.3 V制御信号はTC74VHCT540AFで
5 Vへ変換する。540は反転型なので、FPGAはMOSI/SCLK/RCLK/PLを反転して
出力する。MISOは1 kΩ/2 kΩの抵抗分圧で3.3 Vへ変換する。

出力チェーンは最初に送ったbitがU5側へ移動する。`shift-map.csv`を正本とする。

1. `CART_PWR_EN=0`で開始する。U9が595出力をHi-Zに保つ。
2. `CART_PWR_EN=1`後5 ms待つ。
3. 40 bitをMSB firstで送信し、`CART_RCLK`をLow→High→Lowにする。
4. 500 ns待ち、`CART_PL_N`をLowにして入力をラッチする。
5. `CART_PL_N=High`後、16 bitを読み出す。CPU D0-D7に続いてPPU D0-D7。
6. 全ROM読出し後`CART_PWR_EN=0`。U9が595出力をHi-Zに戻す。

PMODからU9までの4本は反転前の信号で、U9出力が実際の595/165制御信号に
なる。上記手順はカートリッジ側で観測する論理極性を示す。

初版はNROM-256固定で、PRG 32 KiBとCHR 8 KiBを読み出す。MMC1/MMC3/MMC5、
CHR RAM、NROM-128の自動判別は対象外。

シリアルクロック上限は10 MHzとする。カートリッジアクセス待ちは500 ns以上。

## 電源条件

- Dock PMOD +3V3入力: 約400 mAまでを想定し、0.8 mm配線
- FC +5V出力: 200 mA連続を設計上限
- 秋月AE-XCL102D503CR-G（販売コード113065）: 5 V、最大210 mA
- TCK107AFロードスイッチ（販売コード116072）: FPGA正論理制御
- 電源投入中のカートリッジ抜き差し禁止
- Dockへ5 Vを逆給電しない

## 製造ファイル

PCBAは使用せず、JLCPCBへは`manufacturing/gerbers.zip`だけを投入する。
部品は`manual-parts.csv`に従って秋月から購入し、すべて手実装する。

- `gerbers.zip`
- `manual-parts.csv`
- `erc.rpt`, `drc.rpt`

`bom.csv`と`cpl.csv`はPCBA非使用を明示するためヘッダーのみで、アップロード
しない。

FCコネクタJ1はユーザー支給・DNP。調達品はAliExpress item
`1005007638566482`の選択肢E「For Famicom FC 60pin」（固定耳付き）。
信号端子部はFC規格とHirose `CR22A-60D-2.54DS(70)`の公式図面を
交差チェックし、以下でフットプリントを確定する。

- 信号ピン: 2x30、2.54 mmピッチ
- 列間: 5.08 mm
- 端子列の長さ: 29 x 2.54 = 73.66 mm
- 基板穴: 1.0 mm drill、1.8 mm pad（Hirose推奨の0.8 +0.2/-0 mmを包含）

AliExpress品の固定耳は寸法図がない。商品画像を2.54 mmピッチで
画像校正した参考値は、本体幅約87 mm、全長約124 mm、固定穴中心間
約115 mm。ただし写真透視の推定なのでPCB穴には使用ず、耳は基板の外へ
オーバーハングさせる。60本のはんだ端子で固定し、耳の固定穴は本設計では
使用しない。これにより、耳の実測待ちにせずGerberを確定できる。

## 再生成

KiCad 10とpcbnew Python moduleが必要。

```bash
./build.sh
```

## 発注設定

- 2層、FR-4、1.6 mm、1 oz（両面GNDポリゴン）
- HASL lead-freeまたはENIG
- 最小配線/間隔 0.20/0.15 mm（通常配線間隔0.20 mm、U9パッド間のみ0.16 mm）
- 銅箔－基板外形クリアランス 0.30 mm
- 最大外形 約85 x 55.12 mm（PMOD1/2/3に47.5 x 10.5 mmの連続切り欠き）
- Dock直挿し保持のため、単独ケース用M3取付穴は省略
- J1は実装しない

