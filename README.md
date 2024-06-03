# Operating Systems Course Assignements
Assignments for an operating systems class IFT-320 at UdeS

## Assignements
### Assignment 1 - RS232 Driver
Overview of the assignment:
- Write a cbuffer implementation for the RS232 driver.
- Implement the RS232 driver over a chat application.

### Assignment 2 - File System
Overview of the assignment:
TODO

## !DEPRECATED SETUP!
### Assignment 1 - RS232 Driver
**Environment Setup:**
1. Copy the Ubuntu VMs in the `dev1-rs232-driver` folder to your local machine.
2. Add a localhost entry with adequate key exchange and host key algorithms to the `~/.ssh/config` file.
```
Host localhost
    KexAlgorithms +diffie-hellman-group1-sha1,diffie-hellman-group14-sha1,diffie-hellman-group-exchange-sha1
    HostKeyAlgorithms +ssh-dss,ssh-rsa
```

3. Go inside the nix-shell and to install QEMU dependencies.
```bash
nix-shell -p qemu
```

4. Run the first VM with special flags to enable ssh.
```bash
qemu-system-x86_64 -hda PATH-TO-VM-1 -m 1024 -net nic -net user,hostfwd=tcp::2222-:22
```

5. Run the second VM with special flags to port 2223 to enable ssh.
```bash
qemu-system-x86_64 -hda PATH-TO-VM-2 -m 1024 -net nic -net user,hostfwd=tcp::2223-:22
```

6. SSH into both VMs.
```bash
ssh -p 2222 ift320@localhost
ssh -p 2223 ift320@localhost
```

7. To transfer files between the VMs, use the `scp` command.
```bash
scp -P 2222 file ift320@localhost:/path/to/destination
```
You can also use the transfer script provided in the `dev1-rs232-driver` folder.

[Modified driver files](./dev1-rs232-driver/)


