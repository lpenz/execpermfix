{ pkgs ? import <nixpkgs> {} }:
pkgs.stdenv.mkDerivation {
  name = "execpermfix";
  src = ./.;
  buildInputs = [ pkgs.cmake ];
}
