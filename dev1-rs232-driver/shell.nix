{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = [
        pkgs.sshpass
    ];

    shellHook = ''
        echo -e "Setting up environment...\n"
        echo -e "sshpass available at: $(which sshpass)\n"

        echo "\nNow inside Nix shell with environment ready!"
    '';
}
