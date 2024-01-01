#!/bin/sh

./generate_eeprom.lua example.data.lua satelite_data.bin	&&
k5prog -r -f eeprom.bin						&&
./update_satelite_data.sh eeprom.bin satelite_data.bin		&&
k5prog -w -f eeprom.bin
