{
  description = "An over-engineered Hello World in C";

  # Nixpkgs / NixOS version to use.
  inputs.nixpkgs.url = "nixpkgs";

  outputs = { self, nixpkgs }: let

    # to work with older version of flakes
    lastModifiedDate = self.lastModifiedDate or self.lastModified or "19700101";

    # Generate a user-friendly version number.
    version = builtins.substring 0 8 lastModifiedDate;

    # System types to support.
    supportedSystems = [
      "x86_64-linux"
      "x86_64-darwin"
      "aarch64-linux"
      "aarch64-darwin"
    ];

    # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
    forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

    # Nixpkgs instantiated for supported system types.
    nixpkgsFor = forAllSystems (system: import nixpkgs {
      inherit system; overlays = [ self.overlay ];
    });

  in {

    # A Nixpkgs overlay.
    overlay = final: prev: {
      vulkan-project = with final; stdenv.mkDerivation rec {
        name = "vulkan-project";
        src = ./.;
        nativeBuildInputs = [
          autoreconfHook
          vulkan-validation-layers
        ];
        buildInputs = [
          glfw
          glm
          vulkan-headers
          vulkan-loader
        ];
      };
    };

    # Provide some binary packages for selected system types.
    packages = forAllSystems (system: {
      inherit (nixpkgsFor.${system}) vulkan-project;
    });

    # The default package for 'nix build'. This makes sense if the
    # flake provides only one package or there is a clear "main"
    # package.
    defaultPackage = forAllSystems (system: self.packages.${system}.vulkan-project);

    # A NixOS module, if applicable (e.g. if the package provides a system service).
    nixosModules.vulkan-project = { pkgs, ... }: {
      nixpkgs.overlays = [ self.overlay ];
      environment.systemPackages = [ pkgs.vulkan-project ];
    };
  };
}
