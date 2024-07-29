#! /usr/bin/bash

# Copy the files from WIN MACHINE TO VM
SCRIPT_DIR=$(pwd)
USERPROG_DIR_WIN=$SCRIPT_DIR/programming/ift320_dev4/userprog
THREADS_DIR_WIN=$SCRIPT_DIR/programming/ift320_dev4/threads

# VM credentials and dir
VM_IP='192.168.198.128'
USERPROG_DIR_VM='/home/ubuntu/NachosLinuxUdes.IFT320/nachos-tp4-lals2906/code/userprog'
THREADS_DIR_VM='/home/ubuntu/NachosLinuxUdes.IFT320/nachos-tp4-lals2906/code/threads'


# Pre-auth scp from win to vm
echo "Copying .h and .cc files from win machine to VM..."
cd $USERPROG_DIR_WIN || exit
for f in $(ls *.h *.cc); do
    echo "Copying $f from win machine to VM..."
    scp $f ubuntu@$VM_IP:$USERPROG_DIR_VM/$f
done

cd $THREADS_DIR_WIN || exit
for f in $(ls *.h *.cc); do
    echo "Copying $f from win machine to VM..."
    scp $f ubuntu@$VM_IP:$THREADS_DIR_VM/$f
done

if [ $? -ne 0 ]; then
    echo "Error copying files to VM."
    exit 1
else
    echo "Files copied successfully."
fi
