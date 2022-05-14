with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake pkgconfig ];
  buildInputs = [
    alsa-lib
    catch2
    curl
    freetype
    xorg.libX11
    xorg.libXcursor
    xorg.libXext
    xorg.libXinerama
    xorg.libXrandr
  ];
}
