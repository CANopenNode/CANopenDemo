Test
====

Tests in Running CANopen network
--------------------------------

Tests on Running CANopen network are implemented with two CANopenNode devices, running on CAN interface. Example runs with CANopen devices as specified in [tutorial/README.md](../tutorial/README.md). First is `canopend`, CANopenLinux device with gateway interface, second is `demoDevice`. `cocomm` program from CANopenLinux is used for communication with CANopenLinux gateway device.

Tests are run in Linux command line with [Bash Automated Testing System - BATS](https://github.com/bats-core/bats-core), which is included in CANopenDemo as three git submodules.

Test results are available in [test_report.md](test_report.md).


### Prerequisites
 - Get CANopenDemo and all submodules as described in README.md
 - prepare `cocomm`, `canopend` and `demoDevice` as described in [tutorial/README.md](../tutorial/README.md). `canopend` with NodeId=1 must run in Linux, `demoDevice` with NodeId=4 must run on any platform, `can0` is the default CAN interface in Linux.


### Options
See file `config_bats.sh` for configuration options. To change the default values, type the following before running the tests:
 - CAN interface: `export test_can_device=<customCanDevice>` (default `export test_can_device=can0`)
 - Node-Id of the Device 1 (`canopend`): `export D1=<nodeID>; export B1=<80+nodeID>;` (default `export D1=01; export B1=81` - hex value)
 - Node-Id of the Device 2 (`demoDevice`): `export D2=<nodeID>; export B2=<80+nodeID>;` (default `export D2=04; export B2=84` - hex value)


### Run the tests
Run individual tests from the terminal:

    running_canopen/basic_NMT_SDO_heartbeat.bats
    running_canopen/heartbeat_consumer.bats
    running_canopen/SDO_transfer.bats

Run all tests:

    ./test_all.sh


### Testing environment
Open the `*.bats` script file in text editor and examine the contents. Script is quite straightforward, see [BATS usage](https://bats-core.readthedocs.io/en/latest/writing-tests.html) and [BATS assert](https://github.com/bats-core/bats-assert) for information on testing commands. Command line program [cocomm](cocomm/README.md) is used for running each test. Testing program uses one additional functionality of `cocomm` - it prints dump of actual CAN messages, which occur with each `cocomm` command. So BATS can verify the complete result.
