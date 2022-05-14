# Sulfuric
[![CMake Build badge](https://github.com/ThePyrotechnic/sulfuric/workflows/CMake/badge.svg)](https://github.com/thepyrotechnic/sulfuric/actions)

## Notes

Thank you [@sudara](https://melatonin.dev) for your [CMake template](https://github.com/sudara/pamplejuce)

## Building

### Linux

**NixOS-specific**
 First enter the `nix-shell` or wrap the commands below with `nix-shell --run "<command>"`

1. `cmake -B Builds`
2. `cmake --build Builds --config Release`

Outputs are in the `Builds/sulfuric_artefacts` directory
