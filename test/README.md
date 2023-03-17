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


CANopen Conformance Test Tool
-----------------------------

### Testing environment
[CANopen Conformance Test Tool](https://www.can-cia.org/services/test-center/conformance-test-tool/), version 3.0.3.10, running on Windows PC with CAN interface


### Device under test
Device under test is `demoDevice` with with some additional settings, device dependent:
 - PIC32:
   - commented out from code: `CO_errorReport(CO->em, CO_EM_NON_VOLATILE_MEMORY` (file CO_main_PIC32.c)
   - remove `CO_NMT_STARTUP_TO_OPERATIONAL` (file CO_main_PIC32.c)
   - add `CO_CONFIG_EM_PROD_CONFIGURABLE` to `CO_CONFIG_EM` (re-define macro `CO_CONFIG_EM` in CO_driver_custom.h file)


### Test results
Complete test result are available in [test_report_CCT.md](test_report_CCT.md). Note that sequence of CAN messages may not be always correct. Note also, test "OD 10" takes very long time. Test succeed, but data is skipped from report.


### Failures and explanation
 - Test case : 1.0 (EDS checker 01): Check EDS file
   - Some warnings: Access type in section xxxx has no clear mapping direction.
 - Test case : 3.7 (PDO 07): Configure TPDO for sync (acyclic) mode, if supported (Did not find PDO traffic, but should)
   - This is acyclic mode, one PDO is sent, but second isn't, because there is no change-of-state. Failure is still reported.
 - Test case : 3.22 (PDO 22): Try to write a new correct mapping with the mapping procedure.
   - Failure is reported, no reason indicated, erroneous behavior wasn't recognized.
 - Test case : 4.2 (OD 02), Test case : 4.8 (OD 08): Compare the read value against the default value in the EDS if it exists.
   - There are some differences between the values in some objects.
 - Test case : 5.6 (Emergency 06): read object 1003:1 and compare it with received msg (Additional information 0x42 doesn't match)
   - Additional information are manufacturer specific anyway
 - Test case : 9.4 (State 04): Test if DUT supports NMT state stopped correctly.
   - Test sometimes fails: CCT tool detects SYNC message immediately after NMT command stop. NMT status may not be updated fast enough. However, on another CANdump terminal there is no SYNC after NMT stop command.
