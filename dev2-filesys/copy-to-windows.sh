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

# Directories on Steam Deck
SCRIPT_DIR=$(pwd)
MAIN_MACHOS_DECK=$SCRIPT_DIR/nachos
FILESYS_DIR_DECK=$MAIN_MACHOS_DECK/code/filesys
THREADS_DIR_DECK=$MAIN_MACHOS_DECK/code/threads

# Directories on WIN machine
MAIN_NACHOS_WIN=$WIN_USER_PATH/programming/ift320_dev2/
FILESYS_DIR_WIN=$MAIN_NACHOS_WIN/filesys
THREADS_DIR_WIN=$MAIN_NACHOS_WIN/threads

# First copy from deck to win machine
# Use the $1 if provided to only copy the file otherwise all files
if [[ $1 ]]; then
    # For threads-related (file startswith 'thread.')
    if [[ $1 == thread.* || $1 == main.cc ]]; then
        SOURCE_DIR_DECK=$THREADS_DIR_DECK
        TARGET_DIR_WIN=$THREADS_DIR_WIN
    else
        SOURCE_DIR_DECK=$FILESYS_DIR_DECK
        TARGET_DIR_WIN=$FILESYS_DIR_WIN
    fi
    echo "Copying $1 from SteamDeck to win machine..."
    sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $SOURCE_DIR_DECK/$1 $WIN_USERNAME@$WIN_MACHINE_IP:$TARGET_DIR_WIN/$1
    echo "File copied successfully."
    exit 0
else
    echo "Copying .h and .cc files from Steam Deck to win machine's user '$WIN_USERNAME'..."
    cd $FILESYS_DIR_DECK
    for file in $(ls *.h *.cc); do
        echo "Copying $file from SteamDeck to win machine..."
        sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $file $WIN_USERNAME@$WIN_MACHINE_IP:$FILESYS_DIR_WIN/$file
    done
    cd $THREADS_DIR_DECK
    echo "Copying 'main.cc', 'thread.cc' and 'thread.h' from SteamDeck to win machine..."
    sshpass -v -p $WIN_ACCOUNT_PASSWORD scp main.cc $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/main.cc
    sshpass -v -p $WIN_ACCOUNT_PASSWORD scp thread.cc $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/thread.cc
    sshpass -v -p $WIN_ACCOUNT_PASSWORD scp thread.h $WIN_USERNAME@$WIN_MACHINE_IP:$THREADS_DIR_WIN/thread.h
    echo "Files copied successfully."
fi
# Copy from win machine to the vm
# First ssh into win then scp into each vm
echo "\"C:\\Program Files\\Git\\bin\\bash.exe\" -c \"./copy-to-vm.sh\""
sshpass -p $WIN_ACCOUNT_PASSWORD ssh $WIN_USERNAME@$WIN_MACHINE_IP
#"C:\Program Files\Git\bin\bash.exe" -c "./copy-to-vm.sh"