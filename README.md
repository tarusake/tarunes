# tarunes

Veryl で記述された CPU とバス・メモリを含むハードウェア設計プロジェクトです。6502 を参考にした CPU アーキテクチャを模倣しており、Verilator を用いたシミュレーションが可能です。

## プロジェクト構造

```
tarunes/
├── Veryl.toml          # プロジェクト設定
├── Makefile            # ビルド・シミュレーション用 Makefile
├── src/
│   ├── top.veryl       # トップモジュール
│   ├── cpu.veryl       # CPU コア
│   ├── bus.veryl       # バスアービタ
│   ├── bus_if.veryl    # バスインターフェース定義
│   ├── memory.veryl    # メモリモジュール
│   ├── my_pkg.veryl    # パッケージ定義
│   └── tb_top.sv       # SystemVerilog テストベンチ
├── target/             # Veryl が生成する RTL (SystemVerilog)
└── obj_dir/            # Verilator が生成するオブジェクトディレクトリ
```

## 必要条件

- [Verilator](https://www.veripool.org/verilator/) (4.0 以上推奨)
- [Veryl](https://github.com/veryl-lang/veryl) (プロジェクトのビルドに使用)
- GNU Make
- GCC または Clang

## ビルド方法

1. Veryl で RTL (SystemVerilog) を生成します（既に `target/` ディレクトリに生成済みの場合があります）:

```bash
veryl build
```

2. Verilator でシミュレータをビルドします:

```bash
make build
```

または単に:

```bash
make
```

## シミュレーション実行

ビルド後、テストベンチを実行して波形を取得できます:

```bash
make run
```

波形ファイル `wave.vcd` が生成されます。GTKWave などのツールで可視化できます。

## テストベンチ

`src/tb_top.sv` は以下の動作をシミュレートします:

- クロック生成 (100MHz)
- リセットシーケンス
- プログラムメモリ (PROM) への簡単な命令書き込み
  - SEI (78h)
  - LDX #$FF (A2 FF)
  - TXS (9A)
  - LDA #$00 (A9 00)
- リセット解除後の CPU 実行

シミュレーション終了時にプログラムカウンタ (PC) の値が表示されます。

## 依存関係

- プロジェクトは Veryl 言語に依存しています。`Veryl.lock` にバージョン情報が記録されています。
- シミュレーションには Verilator が必要です。

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています。詳細は `LICENSE` ファイルを参照してください（ファイルが存在しない場合は、プロジェクトオーナーに確認してください）。

## 開発者向け情報

- Veryl ソースファイル (`*.veryl`) を編集した後は `veryl build` を実行して RTL を再生成してください。
- `make clean` で生成されたオブジェクトファイルと波形を削除できます。
- 新しいテストを追加する場合は `src/tb_top.sv` を編集するか、新しいテストベンチを作成してください。
