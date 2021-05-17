CANopen SDO Example
===================

This chapter describes some more advanced SDO access to some parameters of the [demoDevice](../demo/README.md). Example runs with CANopen devices as specified in [tutorial/README.md](README.md).

We will communicate with demoDevice, so set the default Node-ID to 4:

     cocomm "set node 4"


Demo record
-----------
Read 64-bit integer:

    cocomm "r 0x2120 1 i64"
    [1] -1234567890123456789

Read 64-bit unsigned integer and display result in different data types:

    cocomm "r 0x2120 2 x64" "r 0x2120 2 u64" "r 0x2120 2"
    [1] 0x1234567890ABCDEF
    [2] 1311768467294899695
    [3] EF CD AB 90 78 56 34 12

Read float values:

    cocomm "r 0x2120 3 r32" "r 0x2120 4 r64"
    [1] 12.345
    [2] 456.789

Read average of those values (custom read function calculates the average):

    cocomm "r 0x2120 5 r64"
    [1] 1.93001e+16

Change one of the input values and re-read the average:

    cocomm "w 0x2120 3 r32 -7.720058e+16" "r 0x2120 5 r64"
    [1] OK
    [2] 7.8681e+07


Demo strings
------------
Read three strings:

    cocomm "r 0x2121 1 vs" "r 0x2121 2 vs" "r 0x2121 3 hex"
    [1] "str"
    [2] "Example string with 1000 bytes capacity. It may contain UTF-8 characters, like '€', tabs '	', newlines, etc."
    [3] C8 3D BB 00 00 00 00 00 00 00

Write to string with capacity 3 bytes in different ways, also read the values:

    cocomm "w 0x2121 1 vs 12"      "r 0x2121 1 vs" \
           "w 0x2121 1 vs 1234"    "r 0x2121 1 vs" \
           "w 0x2121 1 vs \"1 2\"" "r 0x2121 1 vs"
    [1] OK
    [2] "12"
    [3] ERROR:0x06070012 #Data type does not match, length of service parameter too high.
    [4] "12"
    [5] OK
    [6] "1 2"

Write to string with capacity 1000 bytes:

    cocomm "w 0x2121 2 vs \"Writing newLines is not possible as visible string, but exotic \"\"ß\"\" characters works.\""
    [1] OK
    cocomm "r 0x2121 2 vs"
    [1] "Writing newLines is not possible as visible string, but exotic ""ß"" characters works."

Reading the string as "octet string" returns value in base64 format:

    cocomm "r 0x2121 2 os"
    [1] V3JpdGluZyBuZXdMaW5lcyBpcyBub3QgcG9zc2libGUgYXMgdmlzaWJsZSBzdHJpbmcsIGJ1dCBleG90aWMgIsOfIiBjaGFyYWN0ZXJzIHdvcmtzLg==

Decode the base64 through a pipe:

    cocomm "r 0x2121 2 os" | base64 -d
    [1]
    Writing newLines is not possible as visible string, but exotic "ß" characters works.

    cocomm "r 0x2121 2 os" | base64 -d | hexdump -C
    [1]...

Encode something to base64 and sent it to cocomm via pipe:

    echo -ne "We can encode anything to base64\n\nand transfer data as octet string or domain." | base64 -w0 | cocomm -i "w 0x2121 2 os"
    [1] OK

This still works, but would be safer via base64:

    cocomm "r 0x2121 2 vs"
    [1] "We can encode anything to base64

    and transfer data as octet string or domain."

Read octet string and display result in different data types:

    cocomm "r 0x2121 3 hex" "r 0x2121 3 os" "r 0x2121 3 vs" "r 0x2121 3 d" "r 0x2121 3"
    [1] C8 3D BB 00 00 00 00 00 00 00
    [2] yD27AAAAAAAAAA==
    [3] "�=�"
    [4] yD27AAAAAAAAAA==
    [5] C8 3D BB 00 00 00 00 00 00 00

Octet strings have fixed length, in contrast to strings, which accept smaller data size than its buffer size:

    cocomm "w 0x2121 3 hex 01 02 03"
    [1] ERROR:0x06070013 #Data type does not match, length of service parameter too short.

Our parameter is 10 bytes long:

    cocomm "w 0x2121 3 hex 01 2 30 456789 0A b0 0 ff"
    [1] OK


Demo domain
-----------
Domain enables transfer an arbitrary large block of data in one SDO communication cycle. Application must provide own OD read/write access functions to use domain data type. In our example basic device just transfers incremental sequence of bytes. See [demoDevice](../demo/README.md) for details.

SDO block transfer is used for highest efficiency over CAN. If internal virtual CAN network is used, communication is very fast. On real CAN networks data transfer rates are significantly lower. However, SDO communication does not disturb higher priority real time data transfer, which is likely present on CAN bus. SDO block transfer is also resistant to disturbances to some degree. If necessary, it re-transmits corrupted data automatically.

First enable block transfer:

    cocomm "set sdo_block 1"
    [1] OK

Read data (by default 1024 bytes) from "Demo domain" parameter:

    cocomm "r 0x2122 0 d" | base64 -d | pv > file_read
    [1]
    ...success
    1000 B 0:00:00 [ 986KiB/s] [   <=>                                         ]

    ls -l file_read
    hexdump -C file_read

`pv` is nice Linux command, which displays progress. Pure data are stored into to file_read. Other messages from cocomm are printed colored to stderr. If necessary, transfer can be interrupted by `Ctrl+C`. SDO communication will be closed correctly by abort message.

Now create ten times larger file for write and upload it to the demoDevice:

    for n in {1..10}; do cat file_read >> file_write; done

    pv file_write | base64 -w0 | cocomm -i "w 0x2122 0 d"

Domain example in demoDevice works so, that size of data read from device equals to the size of the data written the last time, with initial data size 1024 bytes. Additionally, values of data bytes must be correct sequence of numbers 0, 1, ..., 255, 0, 1, etc. Otherwise error `0x06090030 #Invalid value for parameter` will be responded.

Try also with segmented transfer:

    cocomm "set sdo_block 0"
    pv file_write | base64 -w0 | cocomm -i "w 0x2122 0 d"

Examine candump. Experiment further with larger data sizes or with different devices on real CAN interfaces with different CAN bit-rates, etc.

Here is experimental candump output with two independent pairs of `canopend-demoDevice` exchanging random binary data over one virtual CAN interface (two SDO block transfers interlaced):

    (000.000003)  vcan0  604   [8]  6F C3 A9 6E AF 5C 81 32   'o..n.\.2'
    (000.000010)  vcan0  604   [8]  70 B7 32 8A B2 B0 62 27   'p.2...b''
    (000.000006)  vcan0  604   [8]  71 4F 5F 0B F0 40 0B 53   'qO_..@.S'
    (000.000003)  vcan0  604   [8]  72 CE B9 49 D2 80 BD 55   'r..I...U'
    (000.000010)  vcan0  603   [8]  A2 7F 7F 00 00 00 00 00   '........' < response
    (000.000002)  vcan0  604   [8]  73 94 A2 9E E2 23 8C 5A   's....#.Z'
    (000.000005)  vcan0  604   [8]  74 13 90 A2 59 C3 9B 1D   't...Y...'
    (000.000003)  vcan0  604   [8]  75 5A F4 85 3A 3E B9 10   'uZ..:>..'
    (000.000011)  vcan0  604   [8]  76 30 81 CC 7D 2E 7E 0F   'v0..}.~.'
    (000.000000)  vcan0  583   [8]  01 FC 80 1C D4 8F 67 A2   '......g.'
    (000.000006)  vcan0  604   [8]  77 9B FA 36 21 0F 09 2E   'w..6!...'
    (000.000003)  vcan0  604   [8]  78 C2 6F 97 DC 1D 97 9E   'x.o.....'
    (000.000001)  vcan0  583   [8]  02 E0 B9 97 47 8D 37 E6   '....G.7.'
    (000.000008)  vcan0  583   [8]  03 07 B6 88 68 0B 6A 8C   '....h.j.'
    (000.000002)  vcan0  604   [8]  79 4E 6B 8D 2B 02 F5 9E   'yNk.+...'
    (000.000005)  vcan0  583   [8]  04 AD AC A3 B5 59 4C 30   '.....YL0'
    (000.000001)  vcan0  604   [8]  7A 23 97 5B D6 2A 02 F4   'z#.[.*..'
    (000.000007)  vcan0  583   [8]  05 67 6F 4C 1E AF 18 F3   '.goL....'
    (000.000007)  vcan0  604   [8]  7C 6E 6A 12 FE 5E 0B 5B   '|nj..^.['
    (000.000001)  vcan0  583   [8]  06 E0 43 D4 2E D2 98 BA   '..C.....'
    (000.000005)  vcan0  604   [8]  7D 4B BF 69 25 43 98 FF   '}K.i%C..'
    (000.000010)  vcan0  583   [8]  08 52 16 57 0D 11 72 AE   '.R.W..r.'
    (000.000001)  vcan0  604   [8]  7F 63 0C B1 2B 19 72 89   '.c..+.r.'
    (000.000007)  vcan0  583   [8]  09 67 3D EF B9 C1 32 D3   '.g=...2.'
    (000.000012)  vcan0  583   [8]  0A 3D 22 7E 79 5B D5 D4   '.="~y[..'
    (000.000010)  vcan0  584   [8]  A2 7F 7F 00 00 00 00 00   '........' < response
    (000.000001)  vcan0  583   [8]  0B EC 91 5C FF 04 0A D7   '...\....'
    (000.000011)  vcan0  583   [8]  0C 25 86 30 DC 44 6B 59   '.%.0.DkY'
    (000.000006)  vcan0  583   [8]  0D 56 8F B0 23 81 41 FF   '.V..#.A.'
    (000.000000)  vcan0  604   [8]  01 EF 91 CC EE E8 0D 9C   '........'
    (000.000006)  vcan0  604   [8]  02 EA EB 00 A8 34 71 3F   '.....4q?'
    (000.000002)  vcan0  583   [8]  0E 03 79 87 32 C6 96 15   '..y.2...'
    (000.000006)  vcan0  604   [8]  03 C2 A9 61 8E 25 3D 0A   '...a.%=.'
    ...
    (000.000003)  vcan0  604   [8]  79 16 F1 A6 12 9D 3D B2   'y.....=.'
    (000.000005)  vcan0  604   [8]  7A E2 75 5A 16 18 66 37   'z.uZ..f7'
    (000.000003)  vcan0  583   [8]  6B 72 30 0A 0F C2 2C F9   'kr0...,.'
    (000.000000)  vcan0  604   [8]  7B 82 AE B2 D8 F0 71 32   '{.....q2'
    (000.000010)  vcan0  604   [8]  7C 2A 04 DD BE 30 A4 6E   '|*...0.n'
    (000.000001)  vcan0  583   [8]  6C EA 98 F4 F2 DA C6 93   'l.......'
    (000.000005)  vcan0  604   [8]  7D 68 15 72 4D D0 B5 48   '}h.rM..H'
    (000.000005)  vcan0  583   [8]  6D 7B 24 4D 6E 6A C0 71   'm{$Mnj.q'
    (000.000008)  vcan0  604   [8]  7F E3 B8 4A CA BE 20 F1   '...J.. .'
    (000.000003)  vcan0  583   [8]  6E 11 7B 1F DA 9D 2B B3   'n.{...+.'
    (000.000010)  vcan0  583   [8]  6F 0A 1F 8B C4 56 F9 37   'o....V.7'
    (000.000012)  vcan0  583   [8]  70 67 91 36 4F 59 04 C9   'pg.6OY..'
    (000.000012)  vcan0  583   [8]  71 D4 B8 AC D1 06 7F 63   'q......c'
    (000.000001)  vcan0  584   [8]  A2 7F 7F 00 00 00 00 00   '........' < response
    (000.000008)  vcan0  583   [8]  72 F3 5A E9 06 B2 A7 42   'r.Z....B'
    (000.000009)  vcan0  583   [8]  73 3D 99 69 35 9D B9 43   's=.i5..C'
    (000.000000)  vcan0  604   [8]  01 3B CB D9 00 E5 6A B7   '.;....j.'
    (000.000006)  vcan0  604   [8]  02 A6 0F 34 3D 07 C1 6A   '...4=..j'
    (000.000002)  vcan0  583   [8]  74 72 CE 65 1B 82 0F FC   'tr.e....'
    (000.000001)  vcan0  604   [8]  03 0D 35 70 C2 34 C4 7D   '..5p.4.}'
    (000.000007)  vcan0  583   [8]  75 9C 7A 1D 19 FB A0 92   'u.z.....'
    (000.000007)  vcan0  583   [8]  76 3D 9E B8 2F FF 9A 1B   'v=../...'
    (000.000008)  vcan0  583   [8]  77 ED 4E 6D D5 BA E2 D6   'w.Nm....'
    (000.000008)  vcan0  583   [8]  78 BA E2 83 1E 03 03 9B   'x.......'
    (000.000005)  vcan0  604   [8]  04 DD 6B BB FA 9C 67 BF   '..k...g.'
    (000.000002)  vcan0  583   [8]  79 B7 2E A5 37 28 1E F6   'y...7(..'
    (000.000004)  vcan0  604   [8]  05 C6 A3 C7 EE 6D 24 C0   '.....m$.'
    (000.000004)  vcan0  604   [8]  06 5F 4E 41 25 BB D9 44   '._NA%..D'
    (000.000000)  vcan0  583   [8]  7A DB 34 D2 46 33 33 AE   'z.4.F33.'
    (000.000008)  vcan0  583   [8]  7B 30 76 56 31 1A A6 5C   '{0vV1..\'
    (000.000007)  vcan0  583   [8]  7C 20 42 44 A5 60 AC 77   '| BD.`.w'
    (000.000004)  vcan0  604   [8]  07 42 7F 87 4B A5 ED 0E   '.B..K...'
    (000.000004)  vcan0  583   [8]  7D 5D 36 93 F5 27 08 CF   '}]6..'..'
    (000.000002)  vcan0  604   [8]  08 C1 17 36 99 9E 5F 17   '...6.._.'
    (000.000003)  vcan0  604   [8]  09 C0 3A 4F AF 7F 92 71   '..:O...q'
    (000.000006)  vcan0  583   [8]  7E 77 05 8E 00 43 29 D3   '~w...C).'
    (000.000005)  vcan0  604   [8]  0A CF 24 64 25 4D 81 30   '..$d%M.0'
    (000.000004)  vcan0  583   [8]  7F 98 C3 03 A3 2E AE E7   '........'
    (000.000002)  vcan0  604   [8]  0B 60 F0 5B 29 3F 69 4B   '.`.[)?iK'
    (000.000007)  vcan0  604   [8]  0C E1 02 DA CF 08 34 0E   '......4.'
    (000.000007)  vcan0  604   [8]  0D 7F 13 89 DE AB AB 16   '........'
    (000.000006)  vcan0  604   [8]  0E 98 AF 47 B3 30 56 DF   '...G.0V.'
    (000.000003)  vcan0  604   [8]  0F 83 FB 95 9F EB E9 12   '........'
    (000.000010)  vcan0  604   [8]  10 F8 09 E9 39 10 CE 0F   '....9...'
    (000.000007)  vcan0  604   [8]  11 4C 72 0B DB 34 92 14   '.Lr..4..'
    (000.000003)  vcan0  604   [8]  12 9D 83 1C 07 66 DC 7E   '.....f.~'
    (000.000000)  vcan0  603   [8]  A2 7F 7F 00 00 00 00 00   '........' < response
    (000.000008)  vcan0  604   [8]  13 DB E4 73 B0 6F 23 CB   '...s.o#.'
    (000.000006)  vcan0  604   [8]  14 CE 1C 70 EC C3 57 99   '...p..W.'
    (000.000006)  vcan0  604   [8]  15 7C 33 D5 C7 FC 80 9B   '.|3.....'
    (000.000000)  vcan0  583   [8]  01 2D 89 31 D6 1F 36 88   '.-.1..6.'
    (000.000010)  vcan0  583   [8]  02 B6 56 16 06 64 21 69   '..V..d!i'
