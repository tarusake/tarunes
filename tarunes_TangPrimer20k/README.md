# Tang Primer 20K

Tang Primer 20K Dock向けのGowin Tclビルドです。暗号化されたDVI TX IPは使わず、
Veryl製TMDSエンコーダーとGowinの `CLKDIV`、`OSER10`、`TLVDS_OBUF` で
720x480映像を出力します。

## ビルド

リポジトリ直下で実行します。

```bash
make tangprimer20k ROM=mario
```

bitstreamは次に生成されます。

```text
tarunes_TangPrimer20k/impl/pnr/tarunes_tangprimer20k.fs
```

デバイスは `GW2A-LV18PG256C8/I7`、入力クロックは27 MHzです。PLLで135 MHzを
生成し、`CLKDIV`で位相の揃った27 MHzピクセルクロックを作ります。

DockのS0（FPGA T10）はactive-lowリセットです。T10はSSPI兼用ピンのため、Tcl
ビルドでは `use_sspi_as_gpio` を有効にしています。

ピン情報は[Sipeed公式TangPrimer-20K-example](https://github.com/sipeed/TangPrimer-20K-example)
と、以前のGowin配置配線レポートを照合しています。

APU音声はDockのI2S端子へ出力します。アンプは同期リセット解除後に有効になります。
