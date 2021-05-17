#!/bin/bash


# CAN device for candump
if [[ $test_can_device = "" ]] ; then
    export test_can_device=can0
fi

# Location of cocomm program
if [[ $cocomm = "" ]] ; then
    export cocomm=cocomm
fi

# Heartbeat producer time, 0xA0 = 160ms
if [[ $HB = "" ]] ; then
    export HB=A0
fi

# Heartbeat consumer timeout, 0xF0 = 240ms
if [[ $HB_timeout = "" ]] ; then
    export HB_timeout=F0
fi

# NodeId of device 1 (with command interface)
if [[ $D1 = "" ]] ; then
    export D1=01
fi
# NodeId of device 1 + 0x80
if [[ $B1 = "" ]] ; then
    export B1=81
fi
# NodeId of device 2 (tested device)
if [[ $D2 = "" ]] ; then
    export D2=04
fi
# NodeId of device 2 + 0x80
if [[ $B2 = "" ]] ; then
    export B2=84
fi

# Other
export OK=$'OK\r'
export cocomm_flat=1
export cocomm_candump=$test_can_device
# export cocomm_socket=/tmp/CO_command_socket #default
# export cocomm_host=
# export cocomm_port=
