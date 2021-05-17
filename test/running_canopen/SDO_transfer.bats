#!/usr/bin/env ../libs/bats-core/bin/bats
load '../libs/bats-support/load.bash'
load '../libs/bats-assert/load.bash'

setup_file() {
    . ./config_bats.sh

    export sedStr='s/^\([0-9]*\).*/\1/'

    # prepare data for SDO transfer
    run rm testFile_*
    for n in {1..40}; do cat SDO_transfer_domainData >> testFile_10KiB; done
}

teardown_file() {
    cat testFile_report >&3
    rm testFile_*
}

@test 'Segmented write' {
    assert $cocomm -n0 "set sdo_block 0"
    time_begin=$(date +%s.%N)
    assert base64 -w0 testFile_10KiB | $cocomm -n0 -i "[1] 0x$D2 w 0x2122 0 d"
    time_end=$(date +%s.%N)
    time_diff=$(echo "($time_end - $time_begin) * 1000" | bc | sed $sedStr)
    echo -n "# $(stat --printf="%s" testFile_10KiB) bytes, segmented written/read: $time_diff/" > testFile_report
}

@test 'Segmented read' {
    assert $cocomm -n0 "set sdo_block 0"
    time_begin=$(date +%s.%N)
    assert $cocomm -n0 -odata "[1] 0x$D2 r 0x2122 0 d" > testFile_10KiB_read_b64
    assert base64 -d testFile_10KiB_read_b64 > testFile_10KiB_read
    time_end=$(date +%s.%N)
    time_diff=$(echo "($time_end - $time_begin) * 1000" | bc | sed $sedStr)
    echo "$time_diff milliseconds." >> testFile_report
}

@test 'Segmented transfer, files equal' {
    assert cmp testFile_10KiB testFile_10KiB_read
}

@test 'Block write' {
    assert $cocomm -n0 "set sdo_block 1"
    time_begin=$(date +%s.%N)
    assert base64 -w0 testFile_10KiB | $cocomm -n0 -i "[1] 0x$D2 w 0x2122 0 d"
    time_end=$(date +%s.%N)
    time_diff=$(echo "($time_end - $time_begin) * 1000" | bc | sed $sedStr)
    echo -n "# $(stat --printf="%s" testFile_10KiB) bytes, block written/read: $time_diff/" >> testFile_report
}

@test 'Block read - 2x' {
    assert $cocomm -n0 "set sdo_block 1"
    time_begin=$(date +%s.%N)
    assert $cocomm -n0 -odata "[1] 0x$D2 r 0x2122 0 d" > testFile_10KiB_read1_b64
    assert $cocomm -n0 -odata "[1] 0x$D2 r 0x2122 0 d" > testFile_10KiB_read2_b64
    assert base64 -d testFile_10KiB_read1_b64 > testFile_10KiB_read1
    assert base64 -d testFile_10KiB_read2_b64 > testFile_10KiB_read2
    time_end=$(date +%s.%N)
    time_diff=$(echo "($time_end - $time_begin) * 1000 / 2" | bc | sed $sedStr)
    echo "$time_diff milliseconds." >> testFile_report
}

@test 'Block transfer, files equal' {
    assert cmp testFile_10KiB testFile_10KiB_read1
    assert cmp testFile_10KiB testFile_10KiB_read2
}
