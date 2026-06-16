# tarunes

Veryl で記述された NES 互換の CPU / PPU / バス / メモリを含むハードウェア設計プロジェクトです。
6502 系 CPU、NES 風の PPU / APU、HDMI 480p スケーラ、I2S 音声出力、SFC コントローラ入力を含みます。

現在の公開版は Verilator + SDL2 によるシミュレーションを主対象にしています。iNES ROM を PRG / CHR HEX に変換して読み込み、SDL2 ウィンドウに 256x240 の内部映像または 720x480 の HDMI 出力を表示できます。

商用 ROM ファイルはこのリポジトリに含めていません。

## プロジェクト構造

```text
tarunes/
├── Veryl.toml          # Veryl プロジェクト設定
├── Makefile            # ビルド / ROM 変換 / シミュレーション用 Makefile
├── nes2hex.py          # iNES ROM から PRG / CHR HEX への変換スクリプト
├── csv_to_opcodenotrace.py
│                       # Gowin CSV トレースから PC/opcode 行を抽出する補助スクリプト
├── collapse_cpu_trace_loops.py
│                       # CPU トレース中の長い待ちループを畳む補助スクリプト
├── src/
│   ├── top.veryl       # トップモジュール
│   ├── cpu.veryl       # CPU コア
│   ├── ppu.veryl       # Picture Processing Unit (PPU)
│   ├── apu.veryl       # Audio Processing Unit (APU)
│   ├── i2s.veryl       # I2S 送信
│   ├── hdmi_480p_scaler.veryl
│   │                   # 256x240 映像から 720x480 HDMI タイミングへの変換
│   ├── sfc_controller.veryl
│   │                   # SFC コントローラポーリング
│   ├── controller.veryl # $4016/$4017 コントローラレジスタ
│   ├── oam_dma.veryl   # $4014 OAM DMA
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

シミュレーション環境は Ubuntu 24.04 LTS 以上を推奨します。

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

APU を無効化してビルドする場合:

```bash
make ENABLE_APU=0
```

## シミュレーション実行

ビルドとシミュレーションをまとめて実行できます。

```bash
make run
```

実行すると SDL2 ウィンドウに 256x240 の画面が 2 倍スケールで表示されます。
通常実行では CPU ログと波形ファイル `wave.vcd` を生成します。`wave.vcd` は GTKWave などのツールで可視化できます。

ログと `wave.vcd` 生成を止めて高速に実行する場合:

```bash
make run ARGS=--fast
```

別の ROM を実行する場合:

```bash
make run ROM=sample
```

フレーム数を指定して終了する場合:

```bash
make run ARGS="--fast --frames 10"
```

HDMI 480p 出力側をキャプチャする場合:

```bash
make run ARGS="--fast --capture-hdmi --frames 10"
```

実行時オプション:

- `--fast`: CPU トレースと VCD 出力を無効化
- `--capture-hdmi`: SDL 表示と最終フレーム保存を 720x480 HDMI 出力に切り替え
- `--frames N`: N フレームで終了
- `--dump-audio PATH`: WAV 出力先を指定
- `--dump-vram PATH`: 終了時に VRAM をダンプ
- `--log-ppu-writes PATH`: `$2006` / `$2007` 書き込みログを出力
- `--dump-state`: 終了時に PPU / WRAM の簡易状態を出力

終了時には最終フレーム画像 `last_frame.png` と WAV 音声 `apu.wav` が生成されます。

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
- スクロールレジスタ (`v` / `t` / `w` / fine X) の基本動作
- 8x8 スプライトレンダリング (`$2003` / `$2004` による OAM アクセス)
- `$4014` OAM DMA による WRAM から OAM への転送
- VRAM / CHR ROM アクセス制御
- Palette RAM と NES パレットによる RGB 出力
- 画面出力タイミング生成
- NMI / vblank / sprite zero hit

CPU からは `$2006` / `$2007` 相当のレジスタ経由で VRAM / Palette RAM にアクセスします。スプライトは `$2003` / `$2004` 相当のレジスタで内部 OAM に書き込み、PPU のレンダリング経路で背景と合成されます。PPU のレンダリング経路は CHR ROM と VRAM を参照し、トップモジュールから内部 palette index と HDMI RGB 出力を生成します。

現在のスプライト実装は 8x8 モード、透明色、スプライトパレット、水平/垂直反転、背景前後優先度、sprite zero hit に対応しています。`$4014` OAM DMA は WRAM (`$0000-$1FFF`) 転送元に対応していますが、実機の 513/514 サイクル差は未実装です。8x16 スプライト、sprite overflow は未実装です。

## CPU

`src/cpu.veryl` は 6502 系命令の主要な公式 opcode を実装しています。

- ロード / ストア: `LDA`, `LDX`, `LDY`, `STA`, `STX`, `STY`
- 算術 / 論理: `ADC`, `SBC`, `CMP`, `CPX`, `CPY`, `AND`, `ORA`, `EOR`, `BIT`
- シフト / ローテート / インクリメント: `ASL`, `LSR`, `ROL`, `ROR`, `INC`, `DEC`, `INX`, `INY`, `DEX`, `DEY`
- 分岐 / ジャンプ / サブルーチン: `BCC`, `BCS`, `BNE`, `BEQ`, `BPL`, `BMI`, `BVC`, `BVS`, `JMP`, `JSR`, `RTS`, `RTI`, `BRK`
- スタック / 転送 / フラグ: `PHA`, `PLA`, `PHP`, `PLP`, `TAX`, `TXA`, `TAY`, `TYA`, `TSX`, `TXS`, `SEI`, `SEC`, `CLI`, `CLD`, `CLC`, `SED`, `CLV`, `NOP`

未実装 opcode は `INVALID` として扱います。非公式 opcode は現状対象外です。

## APU / 音声

`src/apu.veryl` は pulse 2ch、triangle、noise の基本的なレジスタとカウンタを実装しています。DMC は未実装です。

- `$4000-$4007`: pulse 1 / pulse 2
- `$4008-$400B`: triangle
- `$400C-$400F`: noise
- `$4015`: channel enable / status
- `$4017`: frame counter

APU の 8bit サンプルはトップモジュール内の小さな FIFO に入り、`src/i2s.veryl` から I2S 信号として出力されます。Verilator テストベンチでは WAV として保存できます。

## 映像出力

`src/hdmi_480p_scaler.veryl` は 256x240 の PPU 出力を 720x480 の HDMI 480p タイミングへ変換します。Verilator テストベンチでは `--capture-hdmi` で HDMI 側の RGB / sync / data enable からフレームをキャプチャできます。

## メモリマップ

CPU 側:

- `$0000-$1FFF`: WRAM
- `$2000-$2007`: PPU レジスタ (`$2003`: OAMADDR, `$2004`: OAMDATA)
- `$4000-$4017`: APU / OAM DMA / コントローラ
- `$4014`: OAM DMA (`$4000-$4017` の一部)
- `$4016-$4017`: コントローラ
- `$8000-$FFFF`: PRG ROM

PPU 側:

- `$0000-$1FFF`: CHR ROM
- `$2000-$3EFF`: VRAM
- `$3F00-$3F1F`: Palette RAM

## 参考資料

NES 互換動作の実装にあたって、主に以下の公開仕様情報を参考にしています。

- [NESdev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
- [Emulator tests](https://www.nesdev.org/wiki/Emulator_tests)
- [CPU unofficial opcodes](https://www.nesdev.org/wiki/CPU_unofficial_opcodes)
- [PPU scrolling](https://www.nesdev.org/wiki/PPU_scrolling)
- [PPU palettes](https://www.nesdev.org/wiki/PPU_palettes)
- [APU](https://www.nesdev.org/wiki/APU)
- [APU Mixer](https://www.nesdev.org/wiki/APU_Mixer)
- [APU Length Counter](https://www.nesdev.org/wiki/APU_Length_Counter)
- [NES研究室 - サンプル](https://tekepen.com/nes/sample.html)

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています。

## English README

English documentation is available in [README.md](README.md).
