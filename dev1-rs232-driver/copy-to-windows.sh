#! /usr/bin/bash
# Use cli arg to specify the VM to copy to
VM_NUM=$1
POSSIBLE_VM_NUMS=(1 2)


# Read the win machine username password from .env
WIN_USERNAME=$(cat .env | grep -oP "WIN_USERNAME=\K.*")
WIN_ACCOUNT_PASSWORD=$(cat .env | grep -oP "WIN_ACCOUNT_PASSWORD=\K.*")
WIN_MACHINE_IP=$(cat .env | grep -oP "WIN_MACHINE_IP=\K.*")

# Check if the VM number is valid
if [[ ! " ${POSSIBLE_VM_NUMS[@]} " =~ " ${VM_NUM} " ]]; then
    echo "Invalid VM number. Please specify 1 or 2."
    exit 1
fi

# Check sshpass dependencies
if [[ ! $(which sshpass) ]]; then
    exit 1
fi

# Copy the file from the VM to the host
if [[ $VM_NUM == 1 ]]; then
    VM1_IP='192.168.59.130'
elif [[ $VM_NUM == 2 ]]; then
    VM2_ip='192.168.59.131'
fi

# Directories on Steam Deck
SCRIPT_DIR=$(pwd)
CHAT_DIR_DECK=$SCRIPT_DIR/rs232_tut-0.2/chat
DRIVER_DIR_DECK=$SCRIPT_DIR/rs232_tut-0.2/driver

# Directories on WIN machine
MAIN_DIR_WIN=/Users/simla/programming/ift320_dev1/rs232_tut-0.2
CHAT_DIR_WIN=$MAIN_DIR_WIN/chat
DRIVER_DIR_WIN=$MAIN_DIR_WIN/driver
# INSIDE_WIN_CHAT_DIR

VM_PASSWORD='rs232'

# Check if chat.c exists to test
if [[ ! -f $CHAT_DIR_DECK/chat.c ]]; then
    echo "chat.c does not exist in $CHAT_DIR_DECK"
    exit 1
fi

# First copy from deck to win machine
echo "Copying assignement files from Steam Deck to win machine's user '$WIN_USERNAME'..."
sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $CHAT_DIR_DECK/chat.c $WIN_USERNAME@$WIN_MACHINE_IP:$CHAT_DIR_WIN/chat.c
sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $DRIVER_DIR_DECK/cbuffer.c $WIN_USERNAME@$WIN_MACHINE_IP:$DRIVER_DIR_WIN/cbuffer.c
sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $DRIVER_DIR_DECK/cbuffer.h $WIN_USERNAME@$WIN_MACHINE_IP:$DRIVER_DIR_WIN/cbuffer.h
sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $DRIVER_DIR_DECK/rs232_tp.c $WIN_USERNAME@$WIN_MACHINE_IP:$DRIVER_DIR_WIN/rs232_tp.c
echo "Files copied successfully."


# Copy from win machine to each vm

# First ssh into win then scp into each vm
echo "SSHing automatically into win machine  with sshpass."
echo "Call manually 'copy_to_vm.cmd VM_NUM=$VM_NUM' batch script within win machine to copy files to VM."
sshpass -v -p $WIN_ACCOUNT_PASSWORD ssh $WIN_USERNAME@$WIN_MACHINE_IP "