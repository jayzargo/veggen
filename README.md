Pred buildom treba inicializovat a updatnut submodules:
    git submodule update --init --recursive

Zatial staci ist do korena a spustit:
    cmake -S . -B build -G "Visual Studio 17 2022" -A x64

Ak neches ist do Visual Studia mozes zbuildit rovno z CMD:
    pre Debug:      cmake --build build --config Debug
    pre Release:    cmake --build build --config Release
