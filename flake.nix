{
  description = "Fixes executable permissions";
  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-22.05;
  outputs = { self, nixpkgs }: {
    packages.x86_64-linux.default =
      with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        name = "execpermfix";
        src = self;
        buildInputs = [ pkgs.cmake ];
      };
  };
}
