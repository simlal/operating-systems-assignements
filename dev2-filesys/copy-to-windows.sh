#! /usr/bin/bash
# Read the win machine username password from .env
WIN_USERNAME=$(cat .env | grep -oP "WIN_USERNAME=\K.*")
WIN_ACCOUNT_PASSWORD=$(cat .env | grep -oP "WIN_ACCOUNT_PASSWORD=\K.*")
WIN_MACHINE_IP=$(cat .env | grep -oP "WIN_MACHINE_IP=\K.*")
VM_IP=$(cat .env | grep -oP "VM_IP=\K.*")

# Check sshpass dependencies
if [[ ! $(which sshpass) ]]; then
    exit 1
fi

# Directories on Steam Deck
SCRIPT_DIR=$(pwd)
MAIN_MACHOS_DECK=$SCRIPT_DIR/nachos
FILESYS_DIR_DECK=$MAIN_MACHOS_DECK/code/filesys

# Directories on WIN machine
MAIN_NACHOS_WIN=/Users/simla/programming/ift320_dev2/nachos
FILESYS_DIR_WIN=$MAIN_NACHOS_WIN/code/filesys

# First copy from deck to win machine
VM_PASSWORD='ubuntu'
echo "Copying .h and .cc files from Steam Deck to win machine's user '$WIN_USERNAME'..."

cd $FILESYS_DIR_DECK
for file in $(ls *.h *.cc); do
    echo "Copying $file from SteamDeck to win machine..."
    sshpass -v -p $WIN_ACCOUNT_PASSWORD scp $file $WIN_USERNAME@$WIN_MACHINE_IP:$FILESYS_DIR_WIN/$file
done
echo "Files copied successfully."

# Copy from win machine to the vm
# First ssh into win then scp into each vm
echo "SSHing automatically into win machine  with sshpass."
echo "Call manually 'copy_to_vm.cmd batch script within win machine to copy files to VM."
sshpass -v -p $WIN_ACCOUNT_PASSWORD ssh $WIN_USERNAME@$WIN_MACHINE_IP