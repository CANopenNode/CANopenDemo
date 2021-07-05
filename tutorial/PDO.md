CANopen PDO Example
===================
A **PDO** (Process Data Object) is providing real-time data transfer of object entries of a CANopen device's object dictionary. The transfer of PDO is performed with no protocol overhead. The PDO correspond to objects in the object dictionary and provide the interface to the application objects. Data type and mapping of application objects into a PDO is determined by a corresponding PDO mapping structure within the object dictionary.

This chapter describes PDO configuration and some experimenting with the [demoDevice](../demo/README.md). Example runs with CANopen devices as specified in [tutorial/README.md](README.md).


Basic principles
----------------
TPDO with specific 11-bit CAN identifier is transmitted by one device and recieved by zero or more devices as RPDO. PDO communication parameters(COB-ID, transmission type, etc.) are in the Object Dictionary at index 0x1400+ and 0x1800+. PDO mapping parameters (size and contents of the PDO) are in the Object Dictionary at index 0x1600+ and 0x1A00+.

Each PDO can be configured with any valid 11-bit CAN identifier. Lower numbers have higher priorities on CAN bus. As a general rule, each CAN message is identified with own CAN-ID, which must be unique and produced by single source. The same is with PDO objects: Any TPDO produced on the CANopen network must have unique CAN-ID and there can be zero to many RPDOs (from different devices) configured to match the CAN-ID of the TPDO of interest.

CANopen standard provides pre-defined connection sets for four RPDOs and four TPDOs on each device with specific 7-bit Node-ID. These are default values and are usable in configuration, where CANopen network contains a master device, which directly communicates with many slaves. In de-centralized systems, where devices operate without a master, it makes sense to configure CAN-IDs of the RPDOs to the non-default values. For default CAN identifiers see #CO_Default_CAN_ID_t.

PDOs can be configured with Object Dictionary Editor or can be configured dynamically inside device running in CANopenNetwork, could be in NMT operational state.

Configure PDO by writing to the OD variables in the following procedure:

 - Disable the PDO by setting bit-31 to 1 in PDO communication parameter, COB-ID
 - Node-Id can be configured only when PDO is disabled.
 - Disable mapping by setting PDO mapping parameter, sub index 0 to 0
 - Configure mapping
 - Enable mapping by setting PDO mapping param, sub 0 to number of mapped objects
 - Enable the PDO by setting bit-31 to 0 in PDO communication parameter, COB-ID

### PDO communication parameter
 1. COB-ID used by PDO:
    - bit 31: If set, PDO does not exist / is not valid
    - bit 30: If set, NO RTR is allowed on this PDO (ignored by CANopenNode)
    - bit 11-29: set to 0
    - bit 0-10: 11-bit CAN-ID
 2. Transmission type:
    - Value 0: synchronous (acyclic)
    - Value 1-240: synchronous (cyclic every (1-240)-th sync)
    - Value 241-253: not used
    - Value 254: event-driven (manufacturer-specific)
    - Value 255: event-driven (device profile and application profile specific)
 3. Inhibit time in multiple of 100µs, if the transmission type is set to 254 or 255 (0 = disabled). Only with TPDO.
 4. (unavailable)
 5. Event timer interval in ms, if the transmission type is set to 254 or 255 (0 = disabled). Deadline monitoring with RPDO.
 6. SYNC start value, only with TPDO.
    - Value 0: Counter of the SYNC message shall not be processed.
    - Value 1-240: The SYNC message with the counter value equal to this value shall be regarded as the first received SYNC message.

### PDO mapping parameter
 * subindex 0: Number of mapped application objects in PDO:
    - Value 0: mapping is disabled.
    - Value 1: sub-index 0x01 is valid.
    - Value 2-8: sub-indexes 0x01 to (0x02 to 0x08) are valid.
 * subindex 1-8: Application object 1-8:
    - bit 16-31: index
    - bit 8-15: sub-index
    - bit 0-7: data length in bits


Read current PDO configuration in demoDevice
--------------------------------------------

Set the default Node-ID to 4 for demoDevice:

    cocomm "set node 4"

### RPDOs

RPDO communication parameters, COB-ID used by RPDO and RPDO mapping parameters, number of mapped application objects:

    cocomm "r 0x1400 1 x32" "r 0x1401 1 x32" "r 0x1402 1 x32" "r 0x1403 1 x32"
    cocomm "r 0x1600 0 u8" "r 0x1601 0 u8" "r 0x1602 0 u8" "r 0x1603 0 u8"

    [1] 0x00000204
    [2] 0x00000304
    [3] 0x80000404
    [4] 0x80000504

    [1] 2
    [2] 4
    [3] 0
    [4] 0

RPDO 1 is enabled, accepts CAN messages with CAN-ID = 0x204 and has mapped two objects. RPDO 2 is enabled, accepts CAN messages with CAN-ID = 0x304 and has mapped four objects. RPDO 3 and RPDO 4 are disabled.

Read communication and mapping parameter for RPDO 1:

    pc=0x1400; pm=0x1600
    cocomm "[1] r $pc 1 x32" "[2] r $pc 2 u8" "[5] r $pc 5 u16"
    cocomm "[0] r $pm 0 u8"; for i in {1..8}; do cocomm "[$i] r $pm $i x32"; done

    [1] 0x00000204
    [2] 255
    [5] 0

    [0] 2
    [1] 0x62000108
    [2] 0x62000208
    [3] 0x62000308
    [4] 0x62000408
    [5] 0x62000508
    [6] 0x62000608
    [7] 0x62000708
    [8] 0x62000808

Mapping parameter shows, that OD variable at index 0x6200, subindex 0x01 and OD variable at index 0x6200, subindex 0x02 are mapped to that RPDO. Both have length of 8 bits, that means RPDO must have two data bytes. See [demo/demoDevice.md](../demo/demoDevice.md) for documentation about OD variables. In our case 0x6200 are digital outputs on the device. Transmission type of RPDO 1 is 255, which means, every received PDO is processed. Event timer is 0, so timeout monitoring is not used.

To conclude: if basicDevice is in NMT operational state and it receives a CAN message with 11-bit CAN identifier equal to 0x204 and data length of two bytes, then data from that message will be written into OD variables 0x6200,1 and 0x6200,2. Furthermore, if CANopen device has digital outputs in its hardware, those OD variables will most likely be written to them. Received CAN message is usually named PDO in CANopen network and is transmitted from known device as TPDO.

Read communication and mapping parameter for RPDO 2:

    pc=0x1401; pm=0x1601
    cocomm "[1] r $pc 1 x32" "[2] r $pc 2 u8" "[5] r $pc 5 u16"
    cocomm "[0] r $pm 0 u8"; for i in {1..8}; do cocomm "[$i] r $pm $i x32"; done

    [1] 0x00000304
    [2] 255
    [5] 0

    [0] 4
    [1] 0x64110110
    [2] 0x64110210
    [3] 0x64110310
    [4] 0x64110410
    [5] 0x00000000
    [6] 0x00000000
    [7] 0x00000000
    [8] 0x00000000

RPDO 2 accepts 0x304 CAN-ID, 8 bytes long and maps to four 16-bit analog outputs.

### TPDOs

TPDO communication parameters, COB-ID used by TPDO and TPDO mapping parameters, number of mapped application objects:

    cocomm "r 0x1800 1 x32" "r 0x1801 1 x32" "r 0x1802 1 x32" "r 0x1803 1 x32"
    cocomm "r 0x1A00 0 u8" "r 0x1A01 0 u8" "r 0x1A02 0 u8" "r 0x1A03 0 u8"

    [1] 0x40000184
    [2] 0x40000284
    [3] 0xC0000384
    [4] 0xC0000484

    [1] 2
    [2] 4
    [3] 0
    [4] 0

Read communication and mapping parameter for TPDO 1:

    pc=0x1800; pm=0x1A00
    cocomm "[1] r $pc 1 x32" "[2] r $pc 2 u8" "[3] r $pc 3 u16" "[5] r $pc 5 u16" "[6] r $pc 6 u8"
    cocomm "[0] r $pm 0 u8"; for i in {1..8}; do cocomm "[$i] r $pm $i x32"; done

    [1] 0x40000184
    [2] 255
    [3] 0
    [5] 0
    [6] 0

    [0] 2
    [1] 0x60000108
    [2] 0x60000208
    [3] 0x60000308
    [4] 0x60000408
    [5] 0x60000508
    [6] 0x60000608
    [7] 0x60000708
    [8] 0x60000808

TPDO 1 has mapped 2*8 digital inputs. It has CAN-ID 0x184 and 2 data bytes, when sent. Transmission type is 255, so it sends on event timer or on application specified event like change-of-state of digital inputs. TPDO 1 is always transmitted on communication reset. Set event timer to send some more:

    cocomm "w 0x1800 5 u16 500"
    # observe candump
    cocomm "w 0x1800 5 u16 0"

Read communication and mapping parameter for TPDO 2:

    pc=0x1801; pm=0x1A01
    cocomm "[1] r $pc 1 x32" "[2] r $pc 2 u8" "[3] r $pc 3 u16" "[5] r $pc 5 u16" "[6] r $pc 6 u8"
    cocomm "[0] r $pm 0 u8"; for i in {1..8}; do cocomm "[$i] r $pm $i x32"; done

    [1] 0x40000284
    [2] 255
    [3] 0
    [5] 0
    [6] 0

    [0] 4
    [1] 0x64010110
    [2] 0x64010210
    [3] 0x64010310
    [4] 0x64010410
    [5] 0x00000000
    [6] 0x00000000
    [7] 0x00000000
    [8] 0x00000000

TPDO 2 has mapped 4 16-bit analog inputs. It has CAN-ID 0x284 and 8 data bytes, when sent.


Configure TPDO 3 in demoDevice
------------------------------
One 32-bit OD variable will be mapped to TPDO 3: 0x2110,1. See [demoDevice](../demo/README.md), section "OD variable mapped to TPDO".

Disable TPDO, if not already disabled, configure mapping and enable TPDO with default CAN-ID:

    cocomm "w 0x1802 1 u32 0x80000000"
    cocomm "w 0x1A02 0 u8 0"
    cocomm "w 0x1A02 1 u32 0x21100120"
    cocomm "w 0x1A02 0 u8 1"
    cocomm "w 0x1802 1 u32 0x00000384"

candump shows one TPDO:

    can0  384   [4]  00 00 00 00

Use event timer, watch candump:

    cocomm "w 0x1802 5 u16 500"

Write something to 0x2110,1 via SDO:

    cocomm "w 0x2110 1 u32 0xAB00CDEF"

    #candump: can0  384   [4]  EF CD 00 AB

Disable event timer and test change-of-state mechanism, used by application in demoDevice. SDO write will change the value of the variable mapped to TPDO and application will trigger TPDO when change-of-state is detected. Observe candump between the commands (ignore SDO messages):

    cocomm "w 0x1802 5 u16 0"

    cocomm "w 0x2110 1 u32 0x12345678"

    cocomm "w 0x2110 1 u32 0x12345678" # same data, no TPDO

    cocomm "w 0x2110 1 u32 0xAABBCCDD"


Configure RPDO in one or several remote devices to accept TPDO 3 from demoDevice
--------------------------------------------------------------------------------
Use any free RPDO and configure it to accept PDO with CAN-ID = 0x384 and data length of 4 bytes. Provide OD variables for data from demoDevice and map RPDO to them.


Other mapping features
----------------------
CANopenNode enables mapping length smaller than variable length. For example, only two bytes of variable of type uint32_t can be mapped, where its little end will be used.

To adjust the length of TPDO or RPDO, dummy entries can be used for mapping, for example:

| Data Type    | dummy mapping |
| ------------ | ------------- |
| int8_t       | 0x00020008    |
| int16_t      | 0x00030016    |
| int32_t      | 0x00040032    |
| int64_t      | 0x00150064    |

CANopenNode granularity is 8 bits, so smallest mappable length is one byte.


Other communication options
---------------------------
More deterministic transmission of PDOs may be configured with usage of CANopen SYNC messages. See CANopen documentation.
