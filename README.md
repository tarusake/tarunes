# tarunes

Veryl で記述された NES 互換の CPU / PPU / バス / メモリを含むハードウェア設計プロジェクトです。
6502 を参考にした CPU と NES 風の PPU を Verilator でシミュレーションし、SDL2 ウィンドウに 256x240 の映像を表示できます。

現在の実装は検証用 ROM を動かすための最小構成です。CPU 命令や PPU レジスタは段階的に実装中です。

## プロジェクト構造

```text
tarunes/
├── Veryl.toml          # Veryl プロジェクト設定
├── Makefile            # ビルド / ROM 変換 / シミュレーション用 Makefile
├── nes2hex.py          # iNES ROM から PRG / CHR HEX への変換スクリプト
├── src/
│   ├── top.veryl       # トップモジュール
│   ├── cpu.veryl       # CPU コア
│   ├── ppu.veryl       # Picture Processing Unit (PPU)
│   ├── bus_cpu.veryl   # CPU バスデコーダ
│   ├── bus_ppu.veryl   # PPU バスデコーダ
│   ├── bus_if.veryl    # バスインターフェース
│   ├── memory.veryl    # メモリモジュール
│   ├── tb_top.sv       # SystemVerilog テストベンチ
│   └── tb_top.cpp      # C++ テストベンチドライバ
├── target/             # Veryl が生成する RTL (SystemVerilog)
└── obj_dir/            # Verilator が生成するオブジェクトディレクトリ
```

## 必要条件

- [Verilator](https://www.veripool.org/verilator/) (5.0 以上)
- [Veryl](https://github.com/veryl-lang/veryl) (0.20.1)
- SDL2 開発パッケージ (`sdl2-config` が使えること)
- SDL2_image 開発パッケージ (`SDL2_image.pc` が `pkg-config` で見えること)
- GNU Make
- C++17 に対応した GCC または Clang

Ubuntu / Debian 系の例:

```bash
sudo apt install verilator libsdl2-dev libsdl2-image-dev make g++
```

### Veryl のインストール

Veryl は公式の toolchain installer である `verylup` でインストールできます。
このプロジェクトでは Veryl `0.20.1` を使用しています。

Cargo を使う場合:

```bash
cargo install verylup
verylup setup
verylup install 0.20.1
verylup default 0.20.1
veryl --version
```

`verylup setup` は初回のみ必要です。
`veryl --version` で `veryl 0.20.1` と表示されれば準備完了です。

最新の toolchain に更新する場合:

```bash
verylup update
```

`verylup` はリリースページからバイナリをダウンロードして `PATH` の通った場所に配置する方法でもインストールできます。
詳細は [verylup: Veryl toolchain installer](https://veryl-lang.org/blog/verylup-veryl-toolchain-installer/) を参照してください。

## ビルド方法

リポジトリ直下に `<ROM名>.nes` を置いてから `make` を実行します。
デフォルトでは `helloworld.nes` を入力にして、`helloworld_prg.hex` と `helloworld_chr.hex` を生成します。

```bash
make
```

別の ROM を使う場合は、拡張子を除いた名前を `ROM` に指定します。

```bash
make ROM=sample
```

この場合、`sample.nes` から `sample_prg.hex` と `sample_chr.hex` が生成され、Verilator の `PROM_PATH` / `CROM_PATH` パラメータに渡されます。

## シミュレーション実行

ビルドとシミュレーションをまとめて実行できます。

```bash
make run
```

実行すると SDL2 ウィンドウに 256x240 の画面が 2 倍スケールで表示され、10 フレーム分シミュレーションします。
同時に CPU ログが表示され、波形ファイル `wave.vcd` と最終フレーム画像 `last_frame.png` が生成されます。`wave.vcd` は GTKWave などのツールで可視化できます。

ログと `wave.vcd` 生成を止めて高速に実行する場合:

```bash
make run ARGS=fast
```

別の ROM を実行する場合:

```bash
make run ROM=sample
```

生成物を削除する場合:

```bash
make clean
```

## ROM ファイル生成

NES の ROM ファイル (`.nes`) を、シミュレーションで読み込む HEX 形式に変換できます。

```bash
./nes2hex.py <input.nes>
```

例:

```bash
./nes2hex.py helloworld.nes
```

出力:

- `helloworld_prg.hex`: CPU 側の PRG ROM
- `helloworld_chr.hex`: PPU 側の CHR ROM

CHR RAM タイプの ROM は CHR ROM を含まないため、`*_chr.hex` は生成されません。

## PPU (Picture Processing Unit)

`src/ppu.veryl` は以下の機能を提供します。

- バックグラウンドレンダリング
- 8x8 スプライトレンダリング (`$2003` / `$2004` による OAM アクセス)
- `$4014` OAM DMA による WRAM から OAM への転送
- VRAM / CHR ROM アクセス制御
- Palette RAM と NES パレットによる RGB 出力
- 画面出力タイミング生成

CPU からは `$2006` / `$2007` 相当のレジスタ経由で VRAM / Palette RAM にアクセスします。スプライトは `$2003` / `$2004` 相当のレジスタで内部 OAM に書き込み、PPU のレンダリング経路で背景と合成されます。PPU のレンダリング経路は CHR ROM と VRAM を参照し、トップモジュールから `pixel_r` / `pixel_g` / `pixel_b` を出力します。

現在のスプライト実装は 8x8 モード、透明色、スプライトパレット、水平/垂直反転、背景前後優先度、sprite zero hit に対応しています。`$4014` OAM DMA は WRAM (`$0000-$1FFF`) 転送元に対応していますが、実機の 513/514 サイクル差は未実装です。8x16 スプライト、sprite overflow は未実装です。

## CPU

`src/cpu.veryl` は、検証用 ROM を動かすために必要な 6502 系命令を部分的に実装しています。

- `SEI`
- `TXS`
- `INX`
- `DEY`
- `LDA #imm`
- `LDX #imm`
- `LDY #imm`
- `STA abs`
- `LDA abs,X`
- `BNE`
- `JMP abs`

未実装命令はログに Unknown Opcode として表示されます。

## メモリマップ

CPU 側:

- `$0000-$1FFF`: WRAM
- `$2000-$2007`: PPU レジスタ (`$2003`: OAMADDR, `$2004`: OAMDATA)
- `$4014`: OAM DMA
- `$8000-$FFFF`: PRG ROM

PPU 側:

- `$0000-$1FFF`: CHR ROM
- `$2000-$3EFF`: VRAM
- `$3F00-$3F1F`: Palette RAM

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています。
