#!/bin/sh

usage() {
	echo "update_satelite_data.sh: Update satelite data in a EEPROM file"
	echo "Usage:"
	printf "\tupdate_satelite_data.sh <EEPROM> <SATELITE_DATA_BIN>\n"
}

if [ $1x = x ]
then
	echo "no EEPROM file specified"
	usage
	exit
fi

if [ $2x = x ]
then
	echo "no satelite binary data specified"
	usage
	exit
fi

# There are still 100 MR channels available

dd if=$2 of=$1 bs=1 seek=1600 conv=notrunc
