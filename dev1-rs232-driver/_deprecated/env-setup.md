# Quick setup to run the VMs and connect from host via ssh

## Pre-requisites
- nix packages
- QEMU
- ssh config file on host
- VMDK images of IFT-3201 and IFT320-2 pre-built

## Steps
1. Install nix packages
2. Install QEMU in a shell
```bash
nix-shell -p qemu
```
3. Change the host ~/.ssh/config file to include the following
```bash
Host localhost
    KexAlgorithms +diffie-hellman-group1-sha1,diffie-hellman-group14-sha1,diffie-hellman-group-exchange-sha1
    HostKeyAlgorithms +ssh-dss,ssh-rsa
```

4. Run the VMs with special flags to enable ssh
```bash
qemu-system-x86_64 -hda PATH-TO-VM-1 -m 1024 -net nic -net user,hostfwd=tcp::2222-:22

# Input the username and password
```

## Connect from the host via ssh
```bash
ssh -p 2222 ift320@localhost
```

Now we can interact with the vm from the host's terminal

## Exchange files via scp
```bash
# Example: Copy the Makefile from the VM to the hosts
scp -P 2222 ift320@localhost:/home/ift320/rs232_tut-0.2/Makefile Makefile
```