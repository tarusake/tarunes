# tarunes

Veryl で記述された NES互換のCPUとPPU、バス・メモリを含むハードウェア設計プロジェクトです。
6502を参考にしたCPUアーキテクチャと、NESのPPUアーキテクチャを模倣しており、Verilatorを用いたシミュレーションが可能です。

## プロジェクト構造

```
tarunes/
├── Veryl.toml          # プロジェクト設定
├── Makefile            # ビルド・シミュレーション用 Makefile
├── nes2hex.py          # NES ROMからHEXファイルへの変換スクリプト
├── src/
│   ├── top.veryl       # トップモジュール
│   ├── cpu.veryl       # CPU コア (6502互換)
│   ├── ppu.veryl       # Picture Processing Unit (PPU)
│   ├── bus_cpu.veryl   # CPUバスアービタ
│   ├── bus_ppu.veryl   # PPUバスアービタ
│   ├── bus_if.veryl    # バスインターフェース定義
│   ├── memory.veryl    # メモリモジュール
│   ├── tb_top.sv       # SystemVerilog テストベンチ
│   └── tb_top.cpp      # C++ テストベンチドライバ
├── target/             # Veryl が生成する RTL (SystemVerilog)
└── obj_dir/            # Verilator が生成するオブジェクトディレクトリ
```

## 必要条件

- [Verilator](https://www.veripool.org/verilator/) (5.0 以上)
- [Veryl](https://github.com/veryl-lang/veryl) (プロジェクトのビルドに使用)
- GNU Make
- GCC または Clang

## ビルド方法

makeコマンドで以下を実行できます。
- Veryl で RTL (SystemVerilog) を生成
- Verilator でシミュレータをビルド

```bash
make
```

## シミュレーション実行

ビルド後、テストベンチを実行して波形を取得できます:

```bash
make run
```

波形ファイル `wave.vcd` が生成されます。GTKWave などのツールで可視化できます。

## PPU (Picture Processing Unit)

このプロジェクトには、NESのグラフィックス処理を担当するPPUの実装が含まれています。`src/ppu.veryl` は以下の機能を提供します：

- バックグラウンドレンダリング
- VRAMアクセス制御
- 画面出力のタイミング生成

PPUはCPUと独立したクロックで動作し、バスを介してCPUと通信します。詳細な仕様はNESのPPUアーキテクチャを参考にしています。

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています。

## ROMファイル生成

NESのROMファイル（.nes形式）をシミュレーションで使用できるHEX形式に変換するスクリプトを提供しています。

### 使用方法

```bash
./nes2hex.py <input.nes> [output.hex]
```

- `input.nes`: 入力となるNES ROMファイル
- `output.hex`: 出力ファイル名（省略時は`input.hex`）

### 例

```bash
# helloworld.nes を helloworld.hex に変換
./nes2hex.py ./helloworld.nes
