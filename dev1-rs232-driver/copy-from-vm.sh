#! /usr/bin/bash

# Use cli arg to specify the VM to copy from
VM_NUM=$1
POSSIBLE_VM_NUMS=(1 2)

# Check if the VM number is valid
if [[ ! " ${POSSIBLE_VM_NUMS[@]} " =~ " ${VM_NUM} " ]]; then
    echo "Invalid VM number. Please specify 1 or 2."
    exit 1
fi

# Copy the file from the VM to the host
if [[ $VM_NUM == 1 ]]; then
    PORT_NUM=2222
elif [[ $VM_NUM == 2 ]]; then
    PORT_NUM=2223
fi
SCRIPT_DIR=$(cd $(dirname $0) && pwd)
CHAT_DIR=$SCRIPT_DIR/rs232_tut-0.2/chat
DRIVER_DIR=$SCRIPT_DIR/rs232_tut-0.2/driver

scp -P $PORT_NUM ift320@localhost:/home/ift320/rs232_tut-0.2/chat/chat.c $CHAT_DIR/chat.c
scp -P $PORT_NUM ift320@localhost:/home/ift320/rs232_tut-0.2/driver/cbuffer.c $DRIVER_DIR/cbuffer.c
scp -P $PORT_NUM ift320@localhost:/home/ift320/rs232_tut-0.2/driver/cbuffer.h $DRIVER_DIR/cbuffer.h
scp -P $PORT_NUM ift320@localhost:/home/ift320/rs232_tut-0.2/driver/rs232_tp.c $DRIVER_DIR/rs232_tp.c
