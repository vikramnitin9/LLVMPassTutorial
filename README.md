# LLVMPassTutorial
Writing an LLVM pass and a Clang frontend action without having to build LLVM from source.

```sh
sudo apt update
sudo apt install llvm-10 llvm-10-dev llvm-10-tools clang-10 libclang-10-dev
```
If you already have a version of LLVM or Clang installed, and you want to use that instead, make sure that LLVM and Clang are the same version.
```sh
llvm-config --version
clang --version
```
The passes may have to change slightly based on your LLVM/Clang version, so keep that in mind too.

```sh
export LLVM_DIR="<your path here>"
```

```sh
mkdir build && cd build
cmake ..
make
```

Add the build directory to PATH and LD_LIBRARY_PATH
```sh
export PATH="$(pwd):$PATH"
export LD_LIBRARY_PATH="$(pwd):$LD_LIBRARY_PATH"
```

Run the Clang frontend action on a single file:
```sh
cd ../example
clang-action arithmetic.c --
```

To run it on the entire project, you need a `compile_commands.json` compilation database. CMake can generate it automatically for you if you specify
```
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```
However, our example program doesn't use CMake. Instead, we can use the [Bear](https://github.com/rizsotto/Bear) utility.
```sh
bear -- make
clang-action *.c
```

Now to run the LLVM pass. Compile each `.c` file into LLVM bitcode (`.bc`) and link them together:
```sh
clang -emit-llvm -O0 -c *.c
llvm-link *.bc -o combined.bc
```

Run the LLVM pass with `opt` on the combined bitcode file:
```sh
opt --load-pass-plugin libMyLLVMPass.so -passes="my-pass" ./combined.bc -o optimized.bc
```