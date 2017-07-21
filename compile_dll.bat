gcc -mwindows -DAS_DLL -c swap_keys.c -o swap_keys.o

rem ld  -shared --output-def swap_keys_generated.def  swap_keys.o -lUser32 -lKernel32 -lmsvcrt -o swap_keys.dll
ld  -shared              swap_keys.def            swap_keys.o -LC:\Strawberry\c\i686-w64-mingw32\lib -lUser32 -lKernel32 -lmsvcrt -o swap_keys.dll
