setlocal
cls
@call config.cmd
@if not exist libq64.a (
    echo create libq64.a with mklib64.cmd
    exit /b 1
)
gcc -Wall -shared sd1mux.c selectable-socketpair/socketpair.c -m64 -I%KX_KDB_PATH%/c/c -L. -lws2_32 -lq64 -o sd1mux_wi64.dll -static -static-libgcc
