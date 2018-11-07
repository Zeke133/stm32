..\..\..\OpenOCD\bin\openocd.exe^
    -s ../share/openocd/scripts^
    -c "adapter_khz 3000"^
    -f interface/stlink.cfg^
    -c "transport select hla_swd"^
    -f target/stm32f1x.cfg^
    -c "gdb_port 50250"^
    -c init^
    -c "reset init"^
    -c "echo **READY**"