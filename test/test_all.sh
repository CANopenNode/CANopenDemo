#!/bin/bash

START=$(date +%s.%N)

cd running_canopen

echo -e "\nRunning 'basic_NMT_SDO_heartbeat.bats':"
./basic_NMT_SDO_heartbeat.bats
echo -e "\nRunning 'heartbeat_consumer.bats':"
./heartbeat_consumer.bats
echo -e "\nRunning 'SDO_transfer.bats':"
./SDO_transfer.bats

END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo -e "Test duration: $DIFF sec\n"

echo -e "\nPress enter to close..."
read
