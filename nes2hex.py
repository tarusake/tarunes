#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# NES ROM (.nes) を読み込み、PRG ROM / CHR ROM を HEXファイルとして出力するスクリプト

import sys
import os

def save_hex(data, filename):
    """バイナリデータを1バイトずつHEXで1行出力"""
    with open(filename, "w") as f:
        for b in data:
            f.write(f"{b:02X}\n")
    print(f"{filename} に {len(data)} バイト書き込み完了")

def main():
    if len(sys.argv) < 2:
        print("Usage: python extract_hex.py <romfile.nes>")
        sys.exit(1)

    rom_path = sys.argv[1]
    base_name = os.path.splitext(os.path.basename(rom_path))[0]

    with open(rom_path, "rb") as f:
        header = f.read(16)
        if header[:4] != b"NES\x1A":
            print("不正なNESファイルです")
            sys.exit(1)

        prg_size = header[4] * 16 * 1024  # 16KB単位
        chr_size = header[5] * 8 * 1024   # 8KB単位

        print(f"PRG ROM size: {prg_size} bytes ({header[4]} x 16KB)")
        print(f"CHR ROM size: {chr_size} bytes ({header[5]} x 8KB)\n")

        prg_data = f.read(prg_size)
        chr_data = f.read(chr_size)

    # 出力ファイル名
    prg_out = f"{base_name}_prg.hex"
    chr_out = f"{base_name}_chr.hex"

    # 出力
    save_hex(prg_data, prg_out)
    if chr_size > 0:
        save_hex(chr_data, chr_out)
    else:
        print("このROMにはCHR ROMが含まれていません（CHR RAMタイプ）")

if __name__ == "__main__":
    main()
