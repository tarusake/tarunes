EESchema Schematic File Version 4
LIBS:power
LIBS:device
LIBS:Connector_Generic
EELAYER 29 0
EELAYER END
$Descr A2 23386 16535
Sheet 1 1
Title "Tang Primer 20K Dock V3713 - FC serial cartridge reader"
Date "2026-11-22"
Rev "A"
Comp "tarunes"
Comment1 "Pin-resolved assembly schematic; J1 is user supplied/DNP"
Comment2 "Dock-powered 3V3 to protected 5V cartridge supply"
Comment3 "PMOD-SFC pass-through on J3/J4"
Comment4 "NROM boot-time PRG/CHR reader"
$EndDescr
Text Notes 900 900 0    150  ~ 30
FC CARTRIDGE INTERFACE
Text Notes 5000 900 0    150  ~ 30
5V SHIFT-REGISTER BUS
Text Notes 5000 6800 0    150  ~ 30
POWER AND PROTECTION
Text Notes 5000 8750 0    150  ~ 30
DECOUPLING / TERMINATION
$Comp
L Connector_Generic:Conn_01x02 C1
U 1 1 64000001
P 20400 10400
F 0 "C1" H 20600 10500 50  0000 C CNN
F 1 "100nF" H 20750 10300 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 20400 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    20400 10400
	1    0    0    -1
$EndComp
Text Label 20200 10400 2    35   ~ 0
+5V_RAW
Text Label 20200 10500 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C10
U 1 1 64000002
P 20400 11500
F 0 "C10" H 20600 11600 50  0000 C CNN
F 1 "10uF" H 20750 11400 50  0000 C CNN
F 2 ":C_0805_2012Metric" H 20400 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    20400 11500
	1    0    0    -1
$EndComp
Text Label 20200 11500 2    35   ~ 0
+3V3
Text Label 20200 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C11
U 1 1 64000003
P 5200 12600
F 0 "C11" H 5400 12700 50  0000 C CNN
F 1 "10uF" H 5550 12500 50  0000 C CNN
F 2 ":C_0805_2012Metric" H 5200 12600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5200 12600
	1    0    0    -1
$EndComp
Text Label 5000 12600 2    35   ~ 0
+3V3
Text Label 5000 12700 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C12
U 1 1 64000004
P 7100 12600
F 0 "C12" H 7300 12700 50  0000 C CNN
F 1 "10uF" H 7450 12500 50  0000 C CNN
F 2 ":C_0805_2012Metric" H 7100 12600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    7100 12600
	1    0    0    -1
$EndComp
Text Label 6900 12600 2    35   ~ 0
+5V_RAW
Text Label 6900 12700 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C13
U 1 1 64000005
P 9000 12600
F 0 "C13" H 9200 12700 50  0000 C CNN
F 1 "22uF" H 9350 12500 50  0000 C CNN
F 2 ":C_0805_2012Metric" H 9000 12600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    9000 12600
	1    0    0    -1
$EndComp
Text Label 8800 12600 2    35   ~ 0
+5V_RAW
Text Label 8800 12700 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C14
U 1 1 64000006
P 10900 12600
F 0 "C14" H 11100 12700 50  0000 C CNN
F 1 "22uF" H 11250 12500 50  0000 C CNN
F 2 ":C_0805_2012Metric" H 10900 12600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10900 12600
	1    0    0    -1
$EndComp
Text Label 10700 12600 2    35   ~ 0
+5V_CART
Text Label 10700 12700 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C2
U 1 1 64000007
P 5200 11500
F 0 "C2" H 5400 11600 50  0000 C CNN
F 1 "100nF" H 5550 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 5200 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5200 11500
	1    0    0    -1
$EndComp
Text Label 5000 11500 2    35   ~ 0
+5V_RAW
Text Label 5000 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C3
U 1 1 64000008
P 7100 11500
F 0 "C3" H 7300 11600 50  0000 C CNN
F 1 "100nF" H 7450 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 7100 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    7100 11500
	1    0    0    -1
$EndComp
Text Label 6900 11500 2    35   ~ 0
+5V_RAW
Text Label 6900 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C4
U 1 1 64000009
P 9000 11500
F 0 "C4" H 9200 11600 50  0000 C CNN
F 1 "100nF" H 9350 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 9000 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    9000 11500
	1    0    0    -1
$EndComp
Text Label 8800 11500 2    35   ~ 0
+5V_RAW
Text Label 8800 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C5
U 1 1 6400000A
P 10900 11500
F 0 "C5" H 11100 11600 50  0000 C CNN
F 1 "100nF" H 11250 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 10900 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10900 11500
	1    0    0    -1
$EndComp
Text Label 10700 11500 2    35   ~ 0
+5V_RAW
Text Label 10700 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C6
U 1 1 6400000B
P 12800 11500
F 0 "C6" H 13000 11600 50  0000 C CNN
F 1 "100nF" H 13150 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 12800 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    12800 11500
	1    0    0    -1
$EndComp
Text Label 12600 11500 2    35   ~ 0
+5V_RAW
Text Label 12600 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C7
U 1 1 6400000C
P 14700 11500
F 0 "C7" H 14900 11600 50  0000 C CNN
F 1 "100nF" H 15050 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 14700 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    14700 11500
	1    0    0    -1
$EndComp
Text Label 14500 11500 2    35   ~ 0
+5V_RAW
Text Label 14500 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C8
U 1 1 6400000D
P 16600 11500
F 0 "C8" H 16800 11600 50  0000 C CNN
F 1 "100nF" H 16950 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 16600 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    16600 11500
	1    0    0    -1
$EndComp
Text Label 16400 11500 2    35   ~ 0
+3V3
Text Label 16400 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 C9
U 1 1 6400000E
P 18500 11500
F 0 "C9" H 18700 11600 50  0000 C CNN
F 1 "100nF" H 18850 11400 50  0000 C CNN
F 2 ":C_0603_1608Metric" H 18500 11500 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    18500 11500
	1    0    0    -1
$EndComp
Text Label 18300 11500 2    35   ~ 0
+5V_RAW
Text Label 18300 11600 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x60 J1
U 1 1 6400000F
P 2600 5000
F 0 "J1" H 2800 5100 50  0000 C CNN
F 1 "FC_60P_SLOT_ALIEXPRESS_1005007638566482_E" H 2950 4900 50  0000 C CNN
F 2 "tarunes:FC_60P_SLOT_E" H 2600 5000 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    2600 5000
	1    0    0    -1
$EndComp
Text Label 2400 2100 2    35   ~ 0
GND
Text Label 2400 2200 2    35   ~ 0
CPU_A11
Text Label 2400 2300 2    35   ~ 0
CPU_A10
Text Label 2400 2400 2    35   ~ 0
CPU_A9
Text Label 2400 2500 2    35   ~ 0
CPU_A8
Text Label 2400 2600 2    35   ~ 0
CPU_A7
Text Label 2400 2700 2    35   ~ 0
CPU_A6
Text Label 2400 2800 2    35   ~ 0
CPU_A5
Text Label 2400 2900 2    35   ~ 0
CPU_A4
Text Label 2400 3000 2    35   ~ 0
CPU_A3
Text Label 2400 3100 2    35   ~ 0
CPU_A2
Text Label 2400 3200 2    35   ~ 0
CPU_A1
Text Label 2400 3300 2    35   ~ 0
CPU_A0
Text Label 2400 3400 2    35   ~ 0
CPU_RNW
NoConn ~ 2400 3500
Text Label 2400 3600 2    35   ~ 0
GND
Text Label 2400 3700 2    35   ~ 0
PPU_RD_N
NoConn ~ 2400 3800
Text Label 2400 3900 2    35   ~ 0
PPU_A6
Text Label 2400 4000 2    35   ~ 0
PPU_A5
Text Label 2400 4100 2    35   ~ 0
PPU_A4
Text Label 2400 4200 2    35   ~ 0
PPU_A3
Text Label 2400 4300 2    35   ~ 0
PPU_A2
Text Label 2400 4400 2    35   ~ 0
PPU_A1
Text Label 2400 4500 2    35   ~ 0
PPU_A0
Text Label 2400 4600 2    35   ~ 0
PPU_D0
Text Label 2400 4700 2    35   ~ 0
PPU_D1
Text Label 2400 4800 2    35   ~ 0
PPU_D2
Text Label 2400 4900 2    35   ~ 0
PPU_D3
Text Label 2400 5000 2    35   ~ 0
+5V_CART
Text Label 2400 5100 2    35   ~ 0
+5V_CART
Text Label 2400 5200 2    35   ~ 0
CPU_M2
Text Label 2400 5300 2    35   ~ 0
CPU_A12
Text Label 2400 5400 2    35   ~ 0
CPU_A13
Text Label 2400 5500 2    35   ~ 0
CPU_A14
Text Label 2400 5600 2    35   ~ 0
CPU_D7
Text Label 2400 5700 2    35   ~ 0
CPU_D6
Text Label 2400 5800 2    35   ~ 0
CPU_D5
Text Label 2400 5900 2    35   ~ 0
CPU_D4
Text Label 2400 6000 2    35   ~ 0
CPU_D3
Text Label 2400 6100 2    35   ~ 0
CPU_D2
Text Label 2400 6200 2    35   ~ 0
CPU_D1
Text Label 2400 6300 2    35   ~ 0
CPU_D0
Text Label 2400 6400 2    35   ~ 0
CPU_ROMSEL_N
NoConn ~ 2400 6500
NoConn ~ 2400 6600
Text Label 2400 6700 2    35   ~ 0
PPU_WR_N
NoConn ~ 2400 6800
Text Label 2400 6900 2    35   ~ 0
PPU_A13_N
Text Label 2400 7000 2    35   ~ 0
PPU_A7
Text Label 2400 7100 2    35   ~ 0
PPU_A8
Text Label 2400 7200 2    35   ~ 0
PPU_A9
Text Label 2400 7300 2    35   ~ 0
PPU_A10
Text Label 2400 7400 2    35   ~ 0
PPU_A11
Text Label 2400 7500 2    35   ~ 0
PPU_A12
Text Label 2400 7600 2    35   ~ 0
PPU_A13
Text Label 2400 7700 2    35   ~ 0
PPU_D7
Text Label 2400 7800 2    35   ~ 0
PPU_D6
Text Label 2400 7900 2    35   ~ 0
PPU_D5
Text Label 2400 8000 2    35   ~ 0
PPU_D4
$Comp
L Connector_Generic:Conn_01x12 J2
U 1 1 64000010
P 10400 4900
F 0 "J2" H 10600 5000 50  0000 C CNN
F 1 "DOCK_J14_MIC_PMOD" H 10750 4800 50  0000 C CNN
F 2 ":PinHeader_2x06_P2.54mm_Vertical" H 10400 4900 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10400 4900
	1    0    0    -1
$EndComp
Text Label 10200 4400 2    35   ~ 0
+3V3
Text Label 10200 4500 2    35   ~ 0
+3V3
Text Label 10200 4600 2    35   ~ 0
GND
Text Label 10200 4700 2    35   ~ 0
GND
Text Label 10200 4800 2    35   ~ 0
CART_SCLK
Text Label 10200 4900 2    35   ~ 0
CART_MOSI
Text Label 10200 5000 2    35   ~ 0
CART_RCLK
Text Label 10200 5100 2    35   ~ 0
CART_PL_N
Text Label 10200 5200 2    35   ~ 0
CART_MISO
Text Label 10200 5300 2    35   ~ 0
CART_OE_N
Text Label 10200 5400 2    35   ~ 0
CART_PWR_EN
Text Label 10200 5500 2    35   ~ 0
CART_PRESENT_N
$Comp
L Connector_Generic:Conn_01x12 J3
U 1 1 64000011
P 12800 4900
F 0 "J3" H 13000 5000 50  0000 C CNN
F 1 "DOCK_J5_RGB_PMOD_A" H 13150 4800 50  0000 C CNN
F 2 ":PinHeader_2x06_P2.54mm_Vertical" H 12800 4900 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    12800 4900
	1    0    0    -1
$EndComp
NoConn ~ 12600 4400
NoConn ~ 12600 4500
Text Label 12600 4600 2    35   ~ 0
GND
Text Label 12600 4700 2    35   ~ 0
GND
NoConn ~ 12600 4800
Text Label 12600 4900 2    35   ~ 0
SFC_DATA_IN
NoConn ~ 12600 5000
Text Label 12600 5100 2    35   ~ 0
SFC_CLK_IN
NoConn ~ 12600 5200
NoConn ~ 12600 5300
NoConn ~ 12600 5400
Text Label 12600 5500 2    35   ~ 0
SFC_LATCH_IN
$Comp
L Connector_Generic:Conn_01x12 J4
U 1 1 64000012
P 15200 4900
F 0 "J4" H 15400 5000 50  0000 C CNN
F 1 "PMOD_SFC_PASS_THROUGH" H 15550 4800 50  0000 C CNN
F 2 ":PinSocket_2x06_P2.54mm_Vertical" H 15200 4900 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    15200 4900
	1    0    0    -1
$EndComp
Text Label 15000 4400 2    35   ~ 0
+3V3
Text Label 15000 4500 2    35   ~ 0
+3V3
Text Label 15000 4600 2    35   ~ 0
GND
Text Label 15000 4700 2    35   ~ 0
GND
NoConn ~ 15000 4800
Text Label 15000 4900 2    35   ~ 0
SFC_DATA
NoConn ~ 15000 5000
Text Label 15000 5100 2    35   ~ 0
SFC_CLK
NoConn ~ 15000 5200
NoConn ~ 15000 5300
NoConn ~ 15000 5400
Text Label 15000 5500 2    35   ~ 0
SFC_LATCH
$Comp
L Connector_Generic:Conn_01x02 L1
U 1 1 64000013
P 10400 7400
F 0 "L1" H 10600 7500 50  0000 C CNN
F 1 "2.2uH" H 10750 7300 50  0000 C CNN
F 2 ":L_Sunlord_SWPA3015S" H 10400 7400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10400 7400
	1    0    0    -1
$EndComp
Text Label 10200 7400 2    35   ~ 0
+3V3
Text Label 10200 7500 2    35   ~ 0
BOOST_SW
$Comp
L Connector_Generic:Conn_01x02 R1
U 1 1 64000014
P 5200 9300
F 0 "R1" H 5400 9400 50  0000 C CNN
F 1 "1k" H 5550 9200 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 5200 9300 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5200 9300
	1    0    0    -1
$EndComp
Text Label 5000 9300 2    35   ~ 0
PISO_Q_CPU
Text Label 5000 9400 2    35   ~ 0
CART_MISO
$Comp
L Connector_Generic:Conn_01x02 R11
U 1 1 64000015
P 7100 10400
F 0 "R11" H 7300 10500 50  0000 C CNN
F 1 "33k" H 7450 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 7100 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    7100 10400
	1    0    0    -1
$EndComp
Text Label 6900 10400 2    35   ~ 0
+5V_RAW
Text Label 6900 10500 2    35   ~ 0
BOOST_FB
$Comp
L Connector_Generic:Conn_01x02 R12
U 1 1 64000016
P 9000 10400
F 0 "R12" H 9200 10500 50  0000 C CNN
F 1 "3.65k" H 9350 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 9000 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    9000 10400
	1    0    0    -1
$EndComp
Text Label 8800 10400 2    35   ~ 0
BOOST_FB
Text Label 8800 10500 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 R13
U 1 1 64000017
P 10900 10400
F 0 "R13" H 11100 10500 50  0000 C CNN
F 1 "33k" H 11250 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 10900 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10900 10400
	1    0    0    -1
$EndComp
Text Label 10700 10400 2    35   ~ 0
ILIM
Text Label 10700 10500 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 R14
U 1 1 64000018
P 12800 10400
F 0 "R14" H 13000 10500 50  0000 C CNN
F 1 "33R" H 13150 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 12800 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    12800 10400
	1    0    0    -1
$EndComp
Text Label 12600 10400 2    35   ~ 0
SFC_CLK_IN
Text Label 12600 10500 2    35   ~ 0
SFC_CLK
$Comp
L Connector_Generic:Conn_01x02 R15
U 1 1 64000019
P 14700 10400
F 0 "R15" H 14900 10500 50  0000 C CNN
F 1 "33R" H 15050 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 14700 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    14700 10400
	1    0    0    -1
$EndComp
Text Label 14500 10400 2    35   ~ 0
SFC_LATCH_IN
Text Label 14500 10500 2    35   ~ 0
SFC_LATCH
$Comp
L Connector_Generic:Conn_01x02 R16
U 1 1 6400001A
P 16600 10400
F 0 "R16" H 16800 10500 50  0000 C CNN
F 1 "10k" H 16950 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 16600 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    16600 10400
	1    0    0    -1
$EndComp
Text Label 16400 10400 2    35   ~ 0
+3V3
Text Label 16400 10500 2    35   ~ 0
SFC_DATA
$Comp
L Connector_Generic:Conn_01x02 R17
U 1 1 6400001B
P 18500 10400
F 0 "R17" H 18700 10500 50  0000 C CNN
F 1 "0R" H 18850 10300 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 18500 10400 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    18500 10400
	1    0    0    -1
$EndComp
Text Label 18300 10400 2    35   ~ 0
SFC_DATA_IN
Text Label 18300 10500 2    35   ~ 0
SFC_DATA
$Comp
L Connector_Generic:Conn_01x02 R2
U 1 1 6400001C
P 7100 9300
F 0 "R2" H 7300 9400 50  0000 C CNN
F 1 "2k" H 7450 9200 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 7100 9300 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    7100 9300
	1    0    0    -1
$EndComp
Text Label 6900 9300 2    35   ~ 0
CART_MISO
Text Label 6900 9400 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 R3
U 1 1 6400001D
P 9000 9300
F 0 "R3" H 9200 9400 50  0000 C CNN
F 1 "10k" H 9350 9200 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 9000 9300 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    9000 9300
	1    0    0    -1
$EndComp
Text Label 8800 9300 2    35   ~ 0
+3V3
Text Label 8800 9400 2    35   ~ 0
CART_PRESENT_N
$Comp
L Connector_Generic:Conn_01x02 R4
U 1 1 6400001E
P 10900 9300
F 0 "R4" H 11100 9400 50  0000 C CNN
F 1 "0R_DNP" H 11250 9200 50  0000 C CNN
F 2 ":R_0603_1608Metric" H 10900 9300 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10900 9300
	1    0    0    -1
$EndComp
Text Label 10700 9300 2    35   ~ 0
CART_PRESENT_N
Text Label 10700 9400 2    35   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x16 U1
U 1 1 6400001F
P 5600 2200
F 0 "U1" H 5800 2300 50  0000 C CNN
F 1 "MC74HCT595ADR2G" H 5950 2100 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 5600 2200 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5600 2200
	1    0    0    -1
$EndComp
Text Label 5400 1500 2    35   ~ 0
CPU_A1
Text Label 5400 1600 2    35   ~ 0
CPU_A2
Text Label 5400 1700 2    35   ~ 0
CPU_A3
Text Label 5400 1800 2    35   ~ 0
CPU_A4
Text Label 5400 1900 2    35   ~ 0
CPU_A5
Text Label 5400 2000 2    35   ~ 0
CPU_A6
Text Label 5400 2100 2    35   ~ 0
CPU_A7
Text Label 5400 2200 2    35   ~ 0
GND
Text Label 5400 2300 2    35   ~ 0
SHIFT_OUT_1_2
Text Label 5400 2400 2    35   ~ 0
+5V_RAW
Text Label 5400 2500 2    35   ~ 0
CART_SCLK
Text Label 5400 2600 2    35   ~ 0
CART_RCLK
Text Label 5400 2700 2    35   ~ 0
CART_OE_N
Text Label 5400 2800 2    35   ~ 0
CART_MOSI
Text Label 5400 2900 2    35   ~ 0
CPU_A0
Text Label 5400 3000 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U2
U 1 1 64000020
P 8000 2200
F 0 "U2" H 8200 2300 50  0000 C CNN
F 1 "MC74HCT595ADR2G" H 8350 2100 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 8000 2200 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    8000 2200
	1    0    0    -1
$EndComp
Text Label 7800 1500 2    35   ~ 0
CPU_A9
Text Label 7800 1600 2    35   ~ 0
CPU_A10
Text Label 7800 1700 2    35   ~ 0
CPU_A11
Text Label 7800 1800 2    35   ~ 0
CPU_A12
Text Label 7800 1900 2    35   ~ 0
CPU_A13
Text Label 7800 2000 2    35   ~ 0
CPU_A14
Text Label 7800 2100 2    35   ~ 0
CPU_RNW
Text Label 7800 2200 2    35   ~ 0
GND
Text Label 7800 2300 2    35   ~ 0
SHIFT_OUT_2_3
Text Label 7800 2400 2    35   ~ 0
+5V_RAW
Text Label 7800 2500 2    35   ~ 0
CART_SCLK
Text Label 7800 2600 2    35   ~ 0
CART_RCLK
Text Label 7800 2700 2    35   ~ 0
CART_OE_N
Text Label 7800 2800 2    35   ~ 0
SHIFT_OUT_1_2
Text Label 7800 2900 2    35   ~ 0
CPU_A8
Text Label 7800 3000 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U3
U 1 1 64000021
P 10400 2200
F 0 "U3" H 10600 2300 50  0000 C CNN
F 1 "MC74HCT595ADR2G" H 10750 2100 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 10400 2200 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    10400 2200
	1    0    0    -1
$EndComp
Text Label 10200 1500 2    35   ~ 0
CPU_ROMSEL_N
Text Label 10200 1600 2    35   ~ 0
PPU_A0
Text Label 10200 1700 2    35   ~ 0
PPU_A1
Text Label 10200 1800 2    35   ~ 0
PPU_A2
Text Label 10200 1900 2    35   ~ 0
PPU_A3
Text Label 10200 2000 2    35   ~ 0
PPU_A4
Text Label 10200 2100 2    35   ~ 0
PPU_A5
Text Label 10200 2200 2    35   ~ 0
GND
Text Label 10200 2300 2    35   ~ 0
SHIFT_OUT_3_4
Text Label 10200 2400 2    35   ~ 0
+5V_RAW
Text Label 10200 2500 2    35   ~ 0
CART_SCLK
Text Label 10200 2600 2    35   ~ 0
CART_RCLK
Text Label 10200 2700 2    35   ~ 0
CART_OE_N
Text Label 10200 2800 2    35   ~ 0
SHIFT_OUT_2_3
Text Label 10200 2900 2    35   ~ 0
CPU_M2
Text Label 10200 3000 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U4
U 1 1 64000022
P 12800 2200
F 0 "U4" H 13000 2300 50  0000 C CNN
F 1 "MC74HCT595ADR2G" H 13150 2100 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 12800 2200 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    12800 2200
	1    0    0    -1
$EndComp
Text Label 12600 1500 2    35   ~ 0
PPU_A7
Text Label 12600 1600 2    35   ~ 0
PPU_A8
Text Label 12600 1700 2    35   ~ 0
PPU_A9
Text Label 12600 1800 2    35   ~ 0
PPU_A10
Text Label 12600 1900 2    35   ~ 0
PPU_A11
Text Label 12600 2000 2    35   ~ 0
PPU_A12
Text Label 12600 2100 2    35   ~ 0
PPU_A13
Text Label 12600 2200 2    35   ~ 0
GND
Text Label 12600 2300 2    35   ~ 0
SHIFT_OUT_4_5
Text Label 12600 2400 2    35   ~ 0
+5V_RAW
Text Label 12600 2500 2    35   ~ 0
CART_SCLK
Text Label 12600 2600 2    35   ~ 0
CART_RCLK
Text Label 12600 2700 2    35   ~ 0
CART_OE_N
Text Label 12600 2800 2    35   ~ 0
SHIFT_OUT_3_4
Text Label 12600 2900 2    35   ~ 0
PPU_A6
Text Label 12600 3000 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U5
U 1 1 64000023
P 15200 2200
F 0 "U5" H 15400 2300 50  0000 C CNN
F 1 "MC74HCT595ADR2G" H 15550 2100 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 15200 2200 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    15200 2200
	1    0    0    -1
$EndComp
Text Label 15000 1500 2    35   ~ 0
PPU_WR_N
Text Label 15000 1600 2    35   ~ 0
PPU_A13_N
Text Label 15000 1700 2    35   ~ 0
SPARE35
Text Label 15000 1800 2    35   ~ 0
SPARE36
Text Label 15000 1900 2    35   ~ 0
SPARE37
Text Label 15000 2000 2    35   ~ 0
SPARE38
Text Label 15000 2100 2    35   ~ 0
SPARE39
Text Label 15000 2200 2    35   ~ 0
GND
Text Label 15000 2300 2    35   ~ 0
SHIFT_OUT_END
Text Label 15000 2400 2    35   ~ 0
+5V_RAW
Text Label 15000 2500 2    35   ~ 0
CART_SCLK
Text Label 15000 2600 2    35   ~ 0
CART_RCLK
Text Label 15000 2700 2    35   ~ 0
CART_OE_N
Text Label 15000 2800 2    35   ~ 0
SHIFT_OUT_4_5
Text Label 15000 2900 2    35   ~ 0
PPU_RD_N
Text Label 15000 3000 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U6
U 1 1 64000024
P 5600 5000
F 0 "U6" H 5800 5100 50  0000 C CNN
F 1 "74HCT165D,653" H 5950 4900 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 5600 5000 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5600 5000
	1    0    0    -1
$EndComp
Text Label 5400 4300 2    35   ~ 0
CART_PL_N
Text Label 5400 4400 2    35   ~ 0
CART_SCLK
Text Label 5400 4500 2    35   ~ 0
CPU_D3
Text Label 5400 4600 2    35   ~ 0
CPU_D2
Text Label 5400 4700 2    35   ~ 0
CPU_D1
Text Label 5400 4800 2    35   ~ 0
CPU_D0
Text Label 5400 4900 2    35   ~ 0
PISO_Q_CPU
Text Label 5400 5000 2    35   ~ 0
GND
Text Label 5400 5100 2    35   ~ 0
PISO_QN_6
Text Label 5400 5200 2    35   ~ 0
PISO_Q_PPU
Text Label 5400 5300 2    35   ~ 0
CPU_D7
Text Label 5400 5400 2    35   ~ 0
CPU_D6
Text Label 5400 5500 2    35   ~ 0
CPU_D5
Text Label 5400 5600 2    35   ~ 0
CPU_D4
Text Label 5400 5700 2    35   ~ 0
GND
Text Label 5400 5800 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x16 U7
U 1 1 64000025
P 8000 5000
F 0 "U7" H 8200 5100 50  0000 C CNN
F 1 "74HCT165D,653" H 8350 4900 50  0000 C CNN
F 2 ":SOIC-16_3.9x9.9mm_P1.27mm" H 8000 5000 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    8000 5000
	1    0    0    -1
$EndComp
Text Label 7800 4300 2    35   ~ 0
CART_PL_N
Text Label 7800 4400 2    35   ~ 0
CART_SCLK
Text Label 7800 4500 2    35   ~ 0
PPU_D3
Text Label 7800 4600 2    35   ~ 0
PPU_D2
Text Label 7800 4700 2    35   ~ 0
PPU_D1
Text Label 7800 4800 2    35   ~ 0
PPU_D0
Text Label 7800 4900 2    35   ~ 0
PISO_Q_PPU
Text Label 7800 5000 2    35   ~ 0
GND
Text Label 7800 5100 2    35   ~ 0
PISO_QN_7
Text Label 7800 5200 2    35   ~ 0
GND
Text Label 7800 5300 2    35   ~ 0
PPU_D7
Text Label 7800 5400 2    35   ~ 0
PPU_D6
Text Label 7800 5500 2    35   ~ 0
PPU_D5
Text Label 7800 5600 2    35   ~ 0
PPU_D4
Text Label 7800 5700 2    35   ~ 0
GND
Text Label 7800 5800 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x06 U8
U 1 1 64000026
P 5600 7600
F 0 "U8" H 5800 7700 50  0000 C CNN
F 1 "TPS61023DRLR" H 5950 7500 50  0000 C CNN
F 2 ":SOT-563" H 5600 7600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    5600 7600
	1    0    0    -1
$EndComp
Text Label 5400 7400 2    35   ~ 0
BOOST_FB
Text Label 5400 7500 2    35   ~ 0
+3V3
Text Label 5400 7600 2    35   ~ 0
+3V3
Text Label 5400 7700 2    35   ~ 0
GND
Text Label 5400 7800 2    35   ~ 0
BOOST_SW
Text Label 5400 7900 2    35   ~ 0
+5V_RAW
$Comp
L Connector_Generic:Conn_01x05 U9
U 1 1 64000027
P 8000 7600
F 0 "U9" H 8200 7700 50  0000 C CNN
F 1 "SY6280AAC" H 8350 7500 50  0000 C CNN
F 2 ":SOT-23-5" H 8000 7600 50  0001 C CNN
F 3 "" H 0 0 50  0001 C CNN
	1    8000 7600
	1    0    0    -1
$EndComp
Text Label 7800 7450 2    35   ~ 0
+5V_RAW
Text Label 7800 7550 2    35   ~ 0
GND
Text Label 7800 7650 2    35   ~ 0
CART_PWR_EN
Text Label 7800 7750 2    35   ~ 0
ILIM
Text Label 7800 7850 2    35   ~ 0
+5V_CART
$EndSCHEMATC
