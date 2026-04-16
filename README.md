# Veggen

## Usage

**Git**

Pred buildom treba inicializovat a updatnut submodules:

```
git submodule update --init --recursive
```

**CMake**

Zatial staci ist do korena a spustit:

```
cmake --presets vs2022
cmake --build --presets <vs2022-debug|vs2022-release>
```

Alebo bez cmake presets:

```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config <Release|Debug>
```