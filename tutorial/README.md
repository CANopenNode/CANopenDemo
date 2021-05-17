CANopenNode Tutorial
====================

CANopen
-------
Before starting tutorial with CANopenNode you should be familiar with the CANopen.
CANopen is the internationally standardized CAN-based higher-layer protocol for embedded control system.
It is specified by CiA301 (or by EN 50325-4) standard. It can be freely downloaded from https://can-cia.org/groups/specifications/.
Some information about CAN and CANopen can be found on https://can-cia.org/can-knowledge/ website. Very efficient way to get familiar with CANopen is by reading a book, for example [Embedded Networking with CAN and CANopen](https://can-newsletter.org/engineering/engineering-miscellaneous/nr_e_cia_can_books_3-2008_emb_can_pfeiffer_120529).

CANopen itself is not a typical master/slave protocol (or commander/responder protocol in the new terminology). It is more like producer/consumer protocol. It is also possible to operate CANopen network without a commander. For example, pre-configured process data objects (PDO) are transmitted from producers. Each PDO may be consumed by multiple nodes. Other useful CANopen functionalities of each CANopen device are also: Heartbeat producer and consumer, Emergency producer, Sync producer or consumer, Time producer or consumer, SDO server (Service data objects - serve variables from Object dictionary), NMT slave (network management - start or stop parts of communication), LSS slave (configuration of Node-Id and Bitrate).

CANopen network usually has one device with commander functionalities for network configuration, such as: NMT master, LSS master, SDO client, Emergency consumer. Commander functionalities in CANopenNode are implemented with Ascii command line interface according to standard CiA309-3.


Environment
-----------
The only requirement for this tutorial is computer with Linux. Examples will run from several windows of Command Line Interface. There are many other CANopen tools available for configuration and working with CANopen network. Some of them have GUI, some can import electronic data sheet of particular device, etc. Here we will use CLI tools from CANopenNode for most basic demonstration of CANopen principles.

### CANopen devices
Two CANopen devices will be used, both based on CANopenNode:
1. `canopend` from CANopenLinux repository will be a commander device with CANopen ASCII command interface. It will run with CANopen NodeId = 1. Application `cocomm` will be used for sending commands to `canopend`. For information on compiling and installing those two applications see CANopenLinux repository.
2. CANopen [demoDevice](demo/README.md) from demo directory. It will run with CANopen NodeId = 4. It can use any target platform listed there. Prepare the device according to the instructions.

### CANopen network
If you don't have CAN interface and both devices are Linux based, then virtual CAN interface can be used, which runs internally. Otherwise connect CANopen devices with wires:

    +-------------------+                             +-------------------+
    | Node-id = 1       |                             | Node-id = 4       |
    |                   |                             |                   |
    | canopend          |                             | demoDevice        |
    +-------------------+                             +-------------------+
    | CAN + transceiver |                             | CAN + transceiver |
    +-----+---+---+-----+                             +-----+---+---+-----+
          |   |   |                                         |   |   |
          |   |   |          can-hi                         |   |   |
      +-------|---+---------------------------------------------|---+-----+
    120Ω  |   |              can-lo                         |   |       120Ω
      +-------+-------------------------------------------------+---------+
          |                  gnd                            |
          +-------------------------------------------------+

#### Start CANopen network
Start CAN interface in Linux according to instructions in the CANopenLinux repository or according to specifications of your CAN interface. By default network runs with 250kbps.

Alternatively enable virtual CAN interface (named `can0`):

    sudo modprobe vcan
    sudo ip link add dev can0 type vcan
    sudo ip link set up can0

### candump
Run a tool, which will display raw CAN messages, for example `candump` from `can-utils` package. Run from own terminal window:

    sudo apt-get install can-utils
    candump -td can0

### Start devices
This is an example, devices may also be started different way.

#### canopend
Use own terminal window:

    cd CANopenLinux
    make
    rm *.persist
    ./canopend can0 -i 1 -c "local-/tmp/CO_command_socket"

#### demoDevice
If demoDevice is not Linux device skip this chapter and run it as necessary. If it is a Linux device, then run in own terminal window:

    cd demo
    make
    rm *.persist
    ./demoLinuxDevice can0

As set by application, startup Node-ID for demoDevice is 4. It can be changed by LSS.

### cocomm
Make sure, utility [cocomm](cocomm/README.md) is working properly. Use another terminal window and display help (help string is received from canopend):

    cocomm "help"


First steps
-----------
candump shows the following raw CAN messages:

    can0  701   [1]  00                       # Boot-up message from canopend
    can0  081   [8]  00 50 01 2F 14 00 00 00  # Emergency from canopend
    can0  704   [1]  00                       # Boot-up message from demoDevice
    can0  084   [8]  00 50 01 2F 74 00 00 00  # Emergency from demoDevice

Second column is 11-bit standard CAN identifier. See #CO_Default_CAN_ID_t for information, how it is used in CANopen. Bootup message has the same 11-bit CAN ID as Heartbeat: 0x700 + CANopen Node-ID.

There are no more messages from CANopen devices, because nothing is configured.


### Emergency messages
Both devices sends Emergency message after the boot-up. Contents of emergency message is:
 - bytes 0..1: #CO_EM_errorCode_t, in our case 0x5000 (Device Hardware) (mind that CANopen is little endian).
 - byte 2: #CO_errorRegister_t, in our case 0x01 (generic error).
 - byte 3: Index of error condition from #CO_EM_errorStatusBits_t, in our case 0x2F (CO_EM_NON_VOLATILE_MEMORY - Error with access to non volatile device memory).
 - bytes 4..7: Additional informative argument, in our case 0x00000014 or 0x00000074.

Emergency messages are triggered internally by #CO_errorReport() function. You may seek inside source code for `CO_EM_NON_VOLATILE_MEMORY` to find source of the emergency message.

`CO_EM_NON_VOLATILE_MEMORY` is generic, critical error, which by default sets the CANopen Error Register. If error register is different than zero, then node may be prohibited to enter NMT operational state and PDOs can not be exchanged with it.


### Basic SDO communication and Heartbeat message
A SDO (Service Data Object) is providing direct access to object entries of a CANopen device's object dictionary. As these object entries may contain data of arbitrary size and data type. SDOs may be used to transfer multiple data sets (each containing an arbitrary large block of data) from a client to a server and vice versa. The client shall control via a multiplexer (index and sub-index of the object dictionary) which data set shall be transferred. The content of the data set is defined within the object dictionary.

See [demo/demoDevice.md](demo/demoDevice.md) for CANopen device documentation of demoDevice with description of all parameters accessible via SDO.

First configure parameter 0x1017 - Producer heartbeat time. Read current value on demoDevice:

    cocomm "4 read 0x1017 0 u16"

Command requested SDO read from device with CANopen Node-ID = 4. Parameter 0x1017 is accessed at sub-index = 0. Data should be displayed as 16bit unsigned integer. Command returns value `0`, which means, heartbeat producer is disabled.

candump shows this communication cycle:

    can0  604   [8]  40 17 10 00 00 00 00 00
    can0  584   [8]  4B 17 10 00 00 00 00 00

First message is client request. In our case client is canopend device, to which cocomm establishes a connection. Second message is server response from demoDevice. It is not necessary to know the details of SDO messages, for more information see #CO_SDO_state_t.

For more information on cocomm commands type:

    cocomm "help"
    cocomm "help datatype"

Now set parameter 0x1017 to 1000 milliseconds on both devices:

    cocomm "1 write 0x1017 0 u16 1000"
    cocomm "4 write 0x1017 0 u16 1000"

candump shows:

    can0  701   [1]  7F
    can0  604   [8]  2B 17 10 00 E8 03 00 00
    can0  584   [8]  60 17 10 00 00 00 00 00
    can0  704   [1]  7F
    can0  701   [1]  7F

Now there are heartbeat messages from each device in one second intervals. 7F means device is in NMT pre-operational state, see #CO_NMT_internalState_t.
There is also one SDO communication cycle with demoDevice. SDO communication cycle with canopend is not visible, because it executes internally.


### Network management - NMT
CANopen NMT messages have highest priority and are sent from NMT master, canopend in our case. They can be sent to specific node or all nodes. They can reset device, communication or set internal state of the remote device to operational, pre-operational(PDO disabled) or stopped(only heartbeat producer and NMT consumer enabled).

    cocomm "4 reset communication"
    cocomm "4 start"
    cocomm "0 reset node"

Observe CAN messages in candump. Second command does not work, because there is critical emergency which sets error register. Third command resets our devices, so go to their terminal windows and restart them.


### Non-volatile storage
Emergency message, error register and NMT pre-operational state have source in uninitialized non-volatile storage. Objects 0x1010 and 0x1011 are used to store and restore the data, usually from CANopen Object Dictionary.

Restore all non-volatile storage on both devices and reset them:

    cocomm "1 w 0x1011 1 vs load"
    cocomm "4 w 0x1011 1 vs load"
    cocomm "0 reset node"
    # re-run devices in their terminals

candump is now without emergency messages and there are two additional PDO messages, because devices are now in NMT operational state. Heratbeat messages are gone:

    can0  701   [1]  00
    can0  704   [1]  00
    can0  184   [2]  00 00
    can0  284   [8]  00 00 00 00 00 00 00 00

Configure heartbeats, observe candump, then reset and re-run devices. Heartbeat messages are gone again:

    cocomm "1 write 0x1017 0 u16 1000"
    cocomm "4 write 0x1017 0 u16 1000"

    cocomm "0 reset node"
    # re-run devices in their terminals

Configure heartbeats, store all parameters, then reset and re-run devices. Heartbeat configuration has been stored, no more future configuration is necessary, devices are autonomous:

    cocomm "1 write 0x1017 0 u16 1000"
    cocomm "4 write 0x1017 0 u16 1000"
    cocomm "1 w 0x1010 1 vs save"
    cocomm "4 w 0x1010 1 vs save"

    cocomm "0 reset node"
    # re-run devices in their terminals

Parameters from CANopen Object Dictionary are sorted into several storage groups. This is configured by Object dictionary editor. See [demo/demoDevice.md](demo/demoDevice.md) for storage group for each OD object.

Storage groups are configured for storage with array of #CO_storage_entry_t objects. (For initialization in Linux devices see CANopenLinux/CO_main_basic.c and demo/linux/CO_driver_custom.h files.)

#### Default storage groups for demoDevice
 - `RAM` is never stored
 - `PERSITS_COMM` is used for store-able communication parameters. Group is assigned to sub-index 2 of the 0x1010 and 0x1011 objects.
 - `PERSIST_APP` is demoDevice specific and is used for its store-able parameters. Group is assigned to sub-index 5 of the 0x1010 and 0x1011 objects.
 - `PERSIST_APP_AUTO` is demoDevice specific and is used for its parameters, which are stored automatically. Group is assigned to sub-index 6 of the 0x1010 and 0x1011 objects.
 - Internal data block used by mainline, which is stored automatically. Block is assigned to sub-index 4 of the 0x1010 and 0x1011 objects.
 - Sub-index 1 of the 0x1010 and 0x1011 stores/restores all parameters.


Next steps
----------
 - [tutorial/LSS.md](LSS.md) - Assigning Node-ID or CAN bitrate to devices, which support LSS configuration.
 - [tutorial/SDO.md](SDO.md) - Further SDO access to the [demoDevice](../demo/README.md) parameters.
 - [tutorial/PDO.md](PDO.md) - Demonstration of Process Data Objects
 - [test/README.md](../test/README.md) - Automatic tests on demoDevice in running CANopen network.
 - Learn other CANopen functionalities like Heartbeat consumer, Time stamp, Sync, etc. Read CiA301 standard, CANopen book or [demo/demoDevice.md](demo/demoDevice.md).
 - Experiment with [demoDevice](../demo/README.md) on available platforms or create your own. Create own Object Dictionary.
 - For own CANopen device with own microcontroller, see `CANopenNode/doc/deviceSupport.md`.
 - Search many very useful and high quality specifications for different [device profiles](http://www.can-cia.org/standardization/specifications/), some of them are public and free to download, for example [CiA401](https://www.can-cia.org/can-knowledge/canopen/cia401/).
 - There are many other CANopen tools, both commercial and free, for example [CANopen for Python](https://github.com/christiansandberg/canopen).
 - Keep control, safety and responsibility on your devices.
 - Read the [CAN Newsletter](https://can-newsletter.org/).
