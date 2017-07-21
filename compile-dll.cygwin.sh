#
#    https://stackoverflow.com/questions/30265036/dll-compiled-from-c-source-code-not-able-to-use-in-excel-vba-file-not-found-er
#
i686-w64-mingw32-gcc -c swap_keys.c
i686-w64-mingw32-gcc -shared -o swap_keys.dll swap_keys.c -Wl,--add-stdcall-alias -static-libgcc
