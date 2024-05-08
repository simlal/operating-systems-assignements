#! /usr/bin/bash

# Use cli arg to specify the VM to copy from
VM_NUM=$1
POSSIBLE_VM_NUMS=(1 2)

# Check if the VM number is valid
if [[ ! " ${POSSIBLE_VM_NUMS[@]} " =~ " ${VM_NUM} " ]]; then
    echo "Invalid VM number. Please specify 1 or 2."
    exit 1
fi

# Get the script path
SCRIPT_DIR=$(cd $(dirname $0) && pwd)

# Get the vm path
if [[ $VM_NUM == 1 ]]; then
    VM_PATH=$SCRIPT_DIR/VM/VM-IFT320-$VM_NUM/Ubuntu-ift320-again-cl$VM_NUM.vmdk
    PORT_NUM=2222

elif [[ $VM_NUM == 2 ]]; then
    VM_PATH=$SCRIPT_DIR/VM/VM-IFT320-$VM_NUM/Ubuntu-ift320-again-cl$VM_NUM.vmdk
    PORT_NUM=2223
fi

# Launch the VM with QEMU's ssh support
qemu-system-x86_64 -hda $VM_PATH -m 1024 -net nic -net user,hostfwd=tcp::$PORT_NUM-:22

