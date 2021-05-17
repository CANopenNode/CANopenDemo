CANopen LSS Example
===================

LSS (Layer settings service) is an extension to CANopen described in CiA DSP 305. The interface is described in CiA DS 309 3.0.0 (ASCII mapping). LSS allows the user to change node ID and bitrate, as well as setting the node ID on an unconfigured node.

LSS uses the the OD Identity register (0x1018) as an unique value to select a node. Therefore the LSS address always consists of four 32 bit values. This also means that LSS relies on this register to actually be unique. (_vendorID_, _productCode_, _revisionNumber_ and _serialNumber_ must be configured and unique on each device.)

This example runs with CANopen devices as specified in [tutorial/README.md](README.md). demoDevice will be configured, its identity object is {0x0, 0x1, 0x2, 0x3}.


Typical usage of LSS
--------------------
Change the node-ID for a known slave, store the new node ID to eeprom, apply new node ID. The node currently has the node-ID = 4:

    cocomm "help lss"

    cocomm "lss_switch_sel 0x00000000 0x00000001 0x00000002 0x00000003"
    cocomm "lss_get_node"
    cocomm "lss_set_node 10"
    cocomm "lss_store"
    cocomm "lss_switch_glob 0"
    cocomm "4 reset comm"

Note that the node ID change is not done until reset communication/node.

Set demoDevice to unconfigured Node_ID and restart it:

    cocomm "lss_switch_sel 0x00000000 0x00000001 0x00000002 0x00000003"
    cocomm "lss_set_node 0xFF"
    cocomm "lss_store"
    cocomm "lss_switch_glob 0"
    cocomm "10 reset node"
    # re-run Linux demoDevice

Now demoDevice is in unconfigured Node-ID state and CANopen services except LSS does not work. CANopen LED flicker. Configure it back to Node-ID = 4. Device enters NMT operational immediately:

    cocomm "lss_switch_sel 0x00000000 0x00000001 0x00000002 0x00000003"
    cocomm "lss_set_node 4"
    cocomm "lss_store"
    cocomm "lss_switch_glob 0"


LSS fastscan
------------
LSS fastscan searches for an unconfigured node. For this example to work, set demoDevice to unconfigured Node_ID as described above.

Search unconfigured node via LSS fastscan:

    cocomm "_lss_fastscan"

After a while, match is found:

    [1] 0x00000001 0x00000002 0x00000003 0x00000006

Set new node-ID, store it to the eeprom, apply the new node-ID:

    cocomm "lss_set_node 4"
    cocomm "lss_store"
    cocomm "lss_switch_glob 0"

To increase scanning speed, you can use:

    cocomm "_lss_fastscan 25"

Where 25 is the scan step delay in ms. Be aware that the scan will become unreliable when the delay is set too low.


Auto enumerate all nodes
------------------------
Auto enumerate all unconfigured nodes via LSS fastscan. Enumeration automatically begins at node ID 2 and node ID is automatically stored to the eeprom. Like with _lss_fastscan, an optional parameter can be used to change the default delay time. For this example to work, set demoDevice to unconfigured Node_ID as described above.

    cocomm "lss_allnodes"

After a while, one or more matches are found:

    # Node-ID 2 assigned to: 0x00000000 0x00000001 0x00000002 0x00000003
    # Found 1 nodes, search finished.
    [1] OK

To get further control over the fastscan process, the lss_allnodes command supports an extended parameter set.

    cocomm "lss_allnodes 25 7 0 2 0x428 1 0 1 0 0 0"

The parameters are as following:
 - 25: scan step delay time in ms
 - 7: enumeration start
 - 0: store node IDs to eeprom; 0 = no, 1 = yes
 - 2: vendor ID scan selector; 0 = fastscan, 2 = match value in next parameter
 - 0x428: vendor ID to match
 - 1: product code scan selector; 0 = fastscan, 1 = ignore, 2 = match value in next parameter
 - 0: product code to match (ignored in this example)
 - 1: software version scan selector; 0 = fastscan, 1 = ignore, 2 = match value in next parameter
 - 0: software version to match (ignored in this example)
 - 0: serial number scan selector; 0 = fastscan, 1 = ignore, 2 = match value in next parameter
 - 0: serial number to match (not used in this example)

Note that only unconfigured nodes (those without a valid node ID) will take part in fastscan!
