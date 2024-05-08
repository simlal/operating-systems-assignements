#! /usr/bin/bash

# Use cli arg to specify the VM to copy to
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

scp -P $PORT_NUM chat/chat.c ift320@localhost:/home/ift320/rs232_tut-0.2/chat/chat.c 
scp -P $PORT_NUM driver/cbuffer.c ift320@localhost:/home/ift320/rs232_tut-0.2/driver/cbuffer.c
scp -P $PORT_NUM driver/cbuffer.h ift320@localhost:/home/ift320/rs232_tut-0.2/driver/cbuffer.h 
scp -P $PORT_NUM driver/rs232_tp.c ift320@localhost:/home/ift320/rs232_tut-0.2/driver/rs232_tp.c 
