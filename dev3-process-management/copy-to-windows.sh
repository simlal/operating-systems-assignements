#! /usr/bin/bash
# Read the win machine username password from .env
WIN_USERNAME=$(grep -oP "^WIN_USERNAME='\K[^']*" .env)
WIN_ACCOUNT_PASSWORD=$(grep -oP "^WIN_ACCOUNT_PASSWORD='\K[^']*" .env)
WIN_MACHINE_IP=$(grep -oP "^WIN_MACHINE_IP='\K[^']*" .env)
WIN_USER_PATH=$(grep -oP "^WIN_USER_PATH='\K[^']*" .env)

# Check sshpass dependencies
if [[ ! $(which sshpass) ]]; then
    echo "sshpass required. Use nix-shell!"
    exit 1
fi

# Directories on deb
SCRIPT_DIR=$(pwd)
MAIN_MACHOS_DEB=$SCRIPT_DIR/nachos.tp3
USERPROG_DIR_DEB=$MAIN_MACHOS_DEB/code/userprog
THREADS_DIR_DEB=$MAIN_MACHOS_DEB/code/threads

# Directories on WIN machine
MAIN_NACHOS_WIN=$WIN_USER_PATH/programming/ift320_dev3/
USERPROG_DIR_WIN=$MAIN_NACHOS_WIN/userprog
THREADS_DIR_WIN=$MAIN_NACHOS_WIN/threads

# First copy from deb to win machine
# Use the $1 if provided to only copy the file otherwise all files
if [[ $1 ]]; then
    # For threads-related (file startswith 'thread.')
    if [[ $1 == exception.* ]]; then
        SOURCE_DIR_DEB=$USERPROG_DIR_DEB
        TARGET_DIR_WIN=$USERPROG_DIR_WIN
    elif [[ $1 == addrspace.* ]]; then
        SOURCE_DIR_DEB=$USERPROG_DIR_DEB
        TARGET_DIR_WIN=$USERPROG_DIR_WIN
    elif [[ $1 == thread.* ]]; then
        SOURCE_DIR_DEB=$THREADS_DIR_DEB
        TARGET_DIR_WIN=$THREADS_DIR_WIN
    else
        echo "Invalid file name. Exiting..."
        exit 1
    fi
    echo "Copying $1 from Debian to win machine..."
    sshpass -v -p "$WIN_ACCOUNT_PASSWORD" scp "$SOURCE_DIR_DEB"/"$1" "$WIN_USERNAME"@$WIN_MACHINE_IP:$TARGET_DIR_WIN/$1
    if [[ $? -ne 0 ]]; then
        echo "Error copying file. Exiting..."
        exit 1
    fi
    echo "File copied successfully."
    exit 0
else
    echo "Copying 'exception.cc', 'addrspace.cc' and 'addrspace.h' from Debian to win machine..."
    cd "$USERPROG_DIR_DEB" || exit
    for file in exception.cc addrspace.cc addrspace.h; do
        sshpass -v -p "$WIN_ACCOUNT_PASSWORD" scp $file "$WIN_USERNAME"@"$WIN_MACHINE_IP":"$USERPROG_DIR_WIN"/$file
    done
    echo "Files copied successfully."

    echo "Copying thread.cc, thread.h from Debian to win machine..."
    cd "$THREADS_DIR_DEB" || exit
    for file in thread.cc thread.h; do
        sshpass -v -p "$WIN_ACCOUNT_PASSWORD" scp $file "$WIN_USERNAME"@"$WIN_MACHINE_IP":"$THREADS_DIR_WIN"/$file
    done
    # cd $THREADS_DIR_DEB
    # echo "Copying 'main.cc', 'thread.cc' and 'thread.h' from Debian to win machine..."
    # sshpass -v -p $WIN_ACCOUNT_PASSWORD scp main.cc $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/main.cc
    # sshpass -v -p $WIN_ACCOUNT_PASSWORD scp thread.cc $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/thread.cc
    # sshpass -v -p $WIN_ACCOUNT_PASSWORD scp thread.h $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/thread.h
    echo "Files copied successfully."
fi
# Copy from win machine to the vm
# First ssh into win then scp into each vm
echo "\"C:\\Program Files\\Git\\bin\\bash.exe\" -c \"./copy-to-vm.sh\""
# sshpass -p $WIN_ACCOUNT_PASSWORD ssh $WIN_USERNAME@$WIN_MACHINE_IP
#"C:\Program Files\Git\bin\bash.exe" -c "./copy-to-vm.sh"