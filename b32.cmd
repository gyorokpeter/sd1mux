setlocal
cls
@call config.cmd
@if not exist libq.a (
    echo create libq.a with mklib.cmd
    exit /b 1
)
gcc -Wall -Werror -shared sd1mux.c selectable-socketpair/socketpair.c -I%KX_KDB_PATH%/c/c -L. -lq -lws2_32 -o sd1mux_wi32.dll -static -static-libgcc
