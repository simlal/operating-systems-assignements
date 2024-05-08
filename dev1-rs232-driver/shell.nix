{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = [
        pkgs.qemu
    ];

    shellHook = ''
        echo -e "Setting up environment...\n"
        echo "Verifiying QEMU installation..."
        qemu-system-x86_64 --version


        echo "Saving the VMs path to VM1_PATH and VM2_PATH variables..."
        cd VM/
        VM1_PATH=$(pwd)/VM-IFT320-1/Ubuntu-ift320-again-cl1.vmdk
        VM2_PATH=$(pwd)/VM-IFT320-2/Ubuntu-ift320-again-cl2.vmdk

        echo -e "\nTo start the VMs, run the following commands:"
        echo "qemu-system-x86_64 -hda \$VM1_PATH -m 1024 -net nic -net user,hostfwd=tcp::2222-:22"
        echo "qemu-system-x86_64 -hda \$VM2_PATH -m 1024 -net nic -net user,hostfwd=tcp::2223-:22"

        echo -e "\nTo connect to the VMs, run the following commands:"
        echo" ssh -p 2222 ift320@localhost"
        echo "ssh -p 2223 ift320@localhost"

        echo -e "\nTo transfer files use 'scp' command:"
        echo "scp -P 2222 file.txt ift320@localhost:/home/ift320"
        
        cd ../
        echo "\nNow inside Nix shell with environment ready!"
    '';
}
