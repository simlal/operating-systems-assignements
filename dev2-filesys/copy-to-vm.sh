#! /usr/bin/bash

# Copy the files from WIN MACHINE TO VM
SCRIPT_DIR=$(pwd)
FILESYS_DIR_WIN=$SCRIPT_DIR/programming/ift320_dev2/filesys

# VM credentials and dir
VM_IP='192.168.59.132'
FILESYS_DIR_VM='/home/ubuntu/NachosLinuxUdes.IFT320/nachos-tp2-lals2906/code/filesys'


# Pre-auth scp from win to vm
echo "Copying .h and .cc files from win machine to VM..."
cd $FILESYS_DIR_WIN
for f in $(ls *.h *.cc); do
    echo "Copying $f from win machine to VM..."
    scp $f ubuntu@$VM_IP:$FILESYS_DIR_VM/$f
done
if [ $? -ne 0 ]; then
    echo "Error copying files to VM."
    exit 1
else
    echo "Files copied successfully."
fi
