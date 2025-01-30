{
  description = "Fixes executable permissions";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages."${system}";
      in
      rec {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "execpermfix";
          version = "1.0.7";
          src = self;
          buildInputs = [ pkgs.cmake ];
        };
      }
    );
}
