EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Transistor_FET:2N7000 Q?
U 1 1 6152D106
P 2350 3000
AR Path="/6152D106" Ref="Q?"  Part="1" 
AR Path="/61506093/6152D106" Ref="Q2"  Part="1" 
AR Path="/6158B349/6152D106" Ref="Q5"  Part="1" 
AR Path="/6158B83D/6152D106" Ref="Q11"  Part="1" 
AR Path="/6158B957/6152D106" Ref="Q8"  Part="1" 
AR Path="/6158BA21/6152D106" Ref="Q?"  Part="1" 
AR Path="/6158BA91/6152D106" Ref="Q?"  Part="1" 
AR Path="/6158BC32/6152D106" Ref="Q?"  Part="1" 
AR Path="/6158BCC0/6152D106" Ref="Q?"  Part="1" 
F 0 "Q2" H 2554 3046 50  0000 L CNN
F 1 "2N7000" H 2554 2955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 2550 2925 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/NDS7002A-D.PDF" H 2350 3000 50  0001 L CNN
	1    2350 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 6152D10C
P 2150 3500
AR Path="/6152D10C" Ref="R?"  Part="1" 
AR Path="/61506093/6152D10C" Ref="R7"  Part="1" 
AR Path="/6158B349/6152D10C" Ref="R12"  Part="1" 
AR Path="/6158B83D/6152D10C" Ref="R22"  Part="1" 
AR Path="/6158B957/6152D10C" Ref="R17"  Part="1" 
AR Path="/6158BA21/6152D10C" Ref="R?"  Part="1" 
AR Path="/6158BA91/6152D10C" Ref="R?"  Part="1" 
AR Path="/6158BC32/6152D10C" Ref="R?"  Part="1" 
AR Path="/6158BCC0/6152D10C" Ref="R?"  Part="1" 
F 0 "R7" H 2220 3546 50  0000 L CNN
F 1 "1M" H 2220 3455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2080 3500 50  0001 C CNN
F 3 "~" H 2150 3500 50  0001 C CNN
	1    2150 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6152D112
P 2450 3800
AR Path="/6152D112" Ref="#PWR?"  Part="1" 
AR Path="/61506093/6152D112" Ref="#PWR028"  Part="1" 
AR Path="/6158B349/6152D112" Ref="#PWR032"  Part="1" 
AR Path="/6158B83D/6152D112" Ref="#PWR040"  Part="1" 
AR Path="/6158B957/6152D112" Ref="#PWR036"  Part="1" 
AR Path="/6158BA21/6152D112" Ref="#PWR?"  Part="1" 
AR Path="/6158BA91/6152D112" Ref="#PWR?"  Part="1" 
AR Path="/6158BC32/6152D112" Ref="#PWR?"  Part="1" 
AR Path="/6158BCC0/6152D112" Ref="#PWR?"  Part="1" 
F 0 "#PWR040" H 2450 3550 50  0001 C CNN
F 1 "GND" H 2455 3627 50  0000 C CNN
F 2 "" H 2450 3800 50  0001 C CNN
F 3 "" H 2450 3800 50  0001 C CNN
	1    2450 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 3800 2450 3650
Wire Wire Line
	2150 3650 2450 3650
$Comp
L Device:R R?
U 1 1 6152D11A
P 2450 3500
AR Path="/6152D11A" Ref="R?"  Part="1" 
AR Path="/61506093/6152D11A" Ref="R8"  Part="1" 
AR Path="/6158B349/6152D11A" Ref="R13"  Part="1" 
AR Path="/6158B83D/6152D11A" Ref="R23"  Part="1" 
AR Path="/6158B957/6152D11A" Ref="R18"  Part="1" 
AR Path="/6158BA21/6152D11A" Ref="R?"  Part="1" 
AR Path="/6158BA91/6152D11A" Ref="R?"  Part="1" 
AR Path="/6158BC32/6152D11A" Ref="R?"  Part="1" 
AR Path="/6158BCC0/6152D11A" Ref="R?"  Part="1" 
F 0 "R8" H 2520 3546 50  0000 L CNN
F 1 "1M" H 2520 3455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2380 3500 50  0001 C CNN
F 3 "~" H 2450 3500 50  0001 C CNN
	1    2450 3500
	1    0    0    -1  
$EndComp
Connection ~ 2450 3650
Wire Wire Line
	2450 3350 2450 3200
Wire Wire Line
	2150 3350 2150 3000
$Comp
L power:+3.3V #PWR?
U 1 1 6152D123
P 2450 2600
AR Path="/6152D123" Ref="#PWR?"  Part="1" 
AR Path="/61506093/6152D123" Ref="#PWR027"  Part="1" 
AR Path="/6158B349/6152D123" Ref="#PWR031"  Part="1" 
AR Path="/6158B83D/6152D123" Ref="#PWR039"  Part="1" 
AR Path="/6158B957/6152D123" Ref="#PWR035"  Part="1" 
AR Path="/6158BA21/6152D123" Ref="#PWR?"  Part="1" 
AR Path="/6158BA91/6152D123" Ref="#PWR?"  Part="1" 
AR Path="/6158BC32/6152D123" Ref="#PWR?"  Part="1" 
AR Path="/6158BCC0/6152D123" Ref="#PWR?"  Part="1" 
F 0 "#PWR039" H 2450 2450 50  0001 C CNN
F 1 "+3.3V" H 2465 2773 50  0000 C CNN
F 2 "" H 2450 2600 50  0001 C CNN
F 3 "" H 2450 2600 50  0001 C CNN
	1    2450 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 2800 2450 2600
Text HLabel 2150 3150 0    50   Input ~ 0
ZoneInput
Text HLabel 2450 3250 2    50   Input ~ 0
ZoneInputTTL
Text HLabel 6650 3150 2    50   Input ~ 0
ZoneOutput
Text HLabel 5550 3850 0    50   Input ~ 0
ZoneOutputTTL
$Comp
L Transistor_FET:2N7000 Q?
U 1 1 6155430A
P 5750 3650
AR Path="/6155430A" Ref="Q?"  Part="1" 
AR Path="/61506093/6155430A" Ref="Q3"  Part="1" 
AR Path="/6158B349/6155430A" Ref="Q6"  Part="1" 
AR Path="/6158B83D/6155430A" Ref="Q12"  Part="1" 
AR Path="/6158B957/6155430A" Ref="Q9"  Part="1" 
AR Path="/6158BA21/6155430A" Ref="Q?"  Part="1" 
AR Path="/6158BA91/6155430A" Ref="Q?"  Part="1" 
AR Path="/6158BC32/6155430A" Ref="Q?"  Part="1" 
AR Path="/6158BCC0/6155430A" Ref="Q?"  Part="1" 
F 0 "Q3" H 5954 3696 50  0000 L CNN
F 1 "2N7000" H 5954 3605 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5950 3575 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/NDS7002A-D.PDF" H 5750 3650 50  0001 L CNN
	1    5750 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 61554310
P 5550 4150
AR Path="/61554310" Ref="R?"  Part="1" 
AR Path="/61506093/61554310" Ref="R9"  Part="1" 
AR Path="/6158B349/61554310" Ref="R14"  Part="1" 
AR Path="/6158B83D/61554310" Ref="R24"  Part="1" 
AR Path="/6158B957/61554310" Ref="R19"  Part="1" 
AR Path="/6158BA21/61554310" Ref="R?"  Part="1" 
AR Path="/6158BA91/61554310" Ref="R?"  Part="1" 
AR Path="/6158BC32/61554310" Ref="R?"  Part="1" 
AR Path="/6158BCC0/61554310" Ref="R?"  Part="1" 
F 0 "R9" H 5620 4196 50  0000 L CNN
F 1 "1M" H 5620 4105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5480 4150 50  0001 C CNN
F 3 "~" H 5550 4150 50  0001 C CNN
	1    5550 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61554316
P 5850 4450
AR Path="/61554316" Ref="#PWR?"  Part="1" 
AR Path="/61506093/61554316" Ref="#PWR029"  Part="1" 
AR Path="/6158B349/61554316" Ref="#PWR033"  Part="1" 
AR Path="/6158B83D/61554316" Ref="#PWR041"  Part="1" 
AR Path="/6158B957/61554316" Ref="#PWR037"  Part="1" 
AR Path="/6158BA21/61554316" Ref="#PWR?"  Part="1" 
AR Path="/6158BA91/61554316" Ref="#PWR?"  Part="1" 
AR Path="/6158BC32/61554316" Ref="#PWR?"  Part="1" 
AR Path="/6158BCC0/61554316" Ref="#PWR?"  Part="1" 
F 0 "#PWR041" H 5850 4200 50  0001 C CNN
F 1 "GND" H 5855 4277 50  0000 C CNN
F 2 "" H 5850 4450 50  0001 C CNN
F 3 "" H 5850 4450 50  0001 C CNN
	1    5850 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 4450 5850 4300
Wire Wire Line
	5550 4300 5850 4300
Wire Wire Line
	5550 4000 5550 3650
Wire Wire Line
	5850 3850 5850 4300
Connection ~ 5850 4300
$Comp
L Device:R R?
U 1 1 61554F46
P 5850 3100
AR Path="/61554F46" Ref="R?"  Part="1" 
AR Path="/61506093/61554F46" Ref="R11"  Part="1" 
AR Path="/6158B349/61554F46" Ref="R16"  Part="1" 
AR Path="/6158B83D/61554F46" Ref="R26"  Part="1" 
AR Path="/6158B957/61554F46" Ref="R21"  Part="1" 
AR Path="/6158BA21/61554F46" Ref="R?"  Part="1" 
AR Path="/6158BA91/61554F46" Ref="R?"  Part="1" 
AR Path="/6158BC32/61554F46" Ref="R?"  Part="1" 
AR Path="/6158BCC0/61554F46" Ref="R?"  Part="1" 
F 0 "R11" H 5920 3146 50  0000 L CNN
F 1 "10k" H 5920 3055 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5780 3100 50  0001 C CNN
F 3 "~" H 5850 3100 50  0001 C CNN
	1    5850 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 61555474
P 5850 2600
AR Path="/61555474" Ref="R?"  Part="1" 
AR Path="/61506093/61555474" Ref="R10"  Part="1" 
AR Path="/6158B349/61555474" Ref="R15"  Part="1" 
AR Path="/6158B83D/61555474" Ref="R25"  Part="1" 
AR Path="/6158B957/61555474" Ref="R20"  Part="1" 
AR Path="/6158BA21/61555474" Ref="R?"  Part="1" 
AR Path="/6158BA91/61555474" Ref="R?"  Part="1" 
AR Path="/6158BC32/61555474" Ref="R?"  Part="1" 
AR Path="/6158BCC0/61555474" Ref="R?"  Part="1" 
F 0 "R10" H 5920 2646 50  0000 L CNN
F 1 "100k" H 5920 2555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5780 2600 50  0001 C CNN
F 3 "~" H 5850 2600 50  0001 C CNN
	1    5850 2600
	1    0    0    -1  
$EndComp
$Comp
L SamacSys_Parts:IRF9Z34NPBF Q4
U 1 1 615557CE
P 6350 2850
AR Path="/61506093/615557CE" Ref="Q4"  Part="1" 
AR Path="/6158B349/615557CE" Ref="Q7"  Part="1" 
AR Path="/6158B957/615557CE" Ref="Q10"  Part="1" 
AR Path="/6158BA21/615557CE" Ref="Q?"  Part="1" 
AR Path="/6158BA91/615557CE" Ref="Q?"  Part="1" 
AR Path="/6158BC32/615557CE" Ref="Q?"  Part="1" 
AR Path="/6158BCC0/615557CE" Ref="Q?"  Part="1" 
AR Path="/6158B83D/615557CE" Ref="Q13"  Part="1" 
F 0 "Q4" H 6780 2996 50  0000 L CNN
F 1 "IRF9Z34NPBF" H 6780 2905 50  0000 L CNN
F 2 "SamacSys_Parts:TO254P469X1042X1967-3P" H 6800 2800 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf9z34npbf.pdf?fileId=5546d462533600a40153561220af1ddd" H 6800 2700 50  0001 L CNN
F 4 "MOSFET MOSFT PCh -55V -17A 100mOhm 23.3nC" H 6800 2600 50  0001 L CNN "Description"
F 5 "4.69" H 6800 2500 50  0001 L CNN "Height"
F 6 "942-IRF9Z34NPBF" H 6800 2400 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/Infineon-IR/IRF9Z34NPBF?qs=9%252BKlkBgLFf16a%2FvlD%252BMCtQ%3D%3D" H 6800 2300 50  0001 L CNN "Mouser Price/Stock"
F 8 "Infineon" H 6800 2200 50  0001 L CNN "Manufacturer_Name"
F 9 "IRF9Z34NPBF" H 6800 2100 50  0001 L CNN "Manufacturer_Part_Number"
	1    6350 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 3250 5850 3450
Wire Wire Line
	5850 2950 5850 2850
Connection ~ 5850 2850
Wire Wire Line
	5850 2850 5850 2750
Wire Wire Line
	6650 1700 6250 1700
Wire Wire Line
	5850 1700 5850 2450
$Comp
L power:+VDC #PWR?
U 1 1 6155EF75
P 6250 1700
AR Path="/6158F2C4/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6158B349/6155EF75" Ref="#PWR034"  Part="1" 
AR Path="/61506093/6155EF75" Ref="#PWR030"  Part="1" 
AR Path="/6158B957/6155EF75" Ref="#PWR038"  Part="1" 
AR Path="/6158BA21/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6158BA91/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6158BC32/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6158BCC0/6155EF75" Ref="#PWR?"  Part="1" 
AR Path="/6158B83D/6155EF75" Ref="#PWR042"  Part="1" 
F 0 "#PWR042" H 6250 1600 50  0001 C CNN
F 1 "+VDC" H 6250 1975 50  0000 C CNN
F 2 "" H 6250 1700 50  0001 C CNN
F 3 "" H 6250 1700 50  0001 C CNN
	1    6250 1700
	1    0    0    -1  
$EndComp
Connection ~ 6250 1700
Wire Wire Line
	6250 1700 5850 1700
Wire Wire Line
	6500 3650 6500 4300
Wire Wire Line
	6500 4300 5850 4300
$Comp
L Device:D_Schottky D3
U 1 1 615DBDB1
P 6500 3500
AR Path="/61506093/615DBDB1" Ref="D3"  Part="1" 
AR Path="/6158B349/615DBDB1" Ref="D5"  Part="1" 
AR Path="/6158B957/615DBDB1" Ref="D7"  Part="1" 
AR Path="/6158BA21/615DBDB1" Ref="D?"  Part="1" 
AR Path="/6158BA91/615DBDB1" Ref="D?"  Part="1" 
AR Path="/6158BC32/615DBDB1" Ref="D?"  Part="1" 
AR Path="/6158BCC0/615DBDB1" Ref="D?"  Part="1" 
AR Path="/6158B83D/615DBDB1" Ref="D9"  Part="1" 
F 0 "D3" V 6454 3580 50  0000 L CNN
F 1 "STPS3150RL" V 6545 3580 50  0000 L CNN
F 2 "Diode_THT:D_DO-201_P15.24mm_Horizontal" H 6500 3500 50  0001 C CNN
F 3 "~" H 6500 3500 50  0001 C CNN
	1    6500 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	6500 3350 6500 3150
Wire Wire Line
	6500 3150 6650 3150
Wire Wire Line
	6650 3150 6650 3050
Wire Wire Line
	6650 2450 6650 1700
$Comp
L SamacSys_Parts:BZX79C10 Z7
U 1 1 6192AFD8
P 6250 1950
AR Path="/6158B957/6192AFD8" Ref="Z7"  Part="1" 
AR Path="/61506093/6192AFD8" Ref="Z5"  Part="1" 
AR Path="/6158B349/6192AFD8" Ref="Z6"  Part="1" 
AR Path="/6158B83D/6192AFD8" Ref="Z8"  Part="1" 
F 0 "Z5" V 6504 1820 50  0000 R CNN
F 1 "BZX79C10" V 6595 1820 50  0000 R CNN
F 2 "DIOAD1068W53L380D172" H 6650 2100 50  0001 L CNN
F 3 "https://www.fairchildsemi.com/datasheets/BZ/BZX79C10.pdf" H 6650 2000 50  0001 L CNN
F 4 "BZX79C10, Zener Diode, 10V +/-5 500 mW 0.2A, 2-Pin DO-35" H 6650 1900 50  0001 L CNN "Description"
F 5 "" H 6650 1800 50  0001 L CNN "Height"
F 6 "512-BZX79C10" H 6650 1700 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/ON-Semiconductor-Fairchild/BZX79C10?qs=FITO%2F%2FQgYDkGEXVpK3DyhQ%3D%3D" H 6650 1600 50  0001 L CNN "Mouser Price/Stock"
F 8 "ON Semiconductor" H 6650 1500 50  0001 L CNN "Manufacturer_Name"
F 9 "BZX79C10" H 6650 1400 50  0001 L CNN "Manufacturer_Part_Number"
	1    6250 1950
	0    -1   1    0   
$EndComp
Wire Wire Line
	5850 2850 6250 2850
Wire Wire Line
	6250 1950 6250 1700
Wire Wire Line
	6250 2550 6250 2850
Connection ~ 6250 2850
Wire Wire Line
	6250 2850 6350 2850
$EndSCHEMATC
