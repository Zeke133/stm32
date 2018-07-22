SETLOCAL

REM number of COM-port which device connected
set COM_PORT_NUMBER=5

REM file with memory map used by utility, fil located in "/Map" directory
REM STM32F103C8T6 on practice have 128Kb flash but it's not guarantied
REM "STM32F1_Med-density_128K"
set MEMORY_MAP=STM32F1_Med-density_64K

REM flash must be erased before writing
REM here we specify number of sectors to erase and their indexes
REM 1 sector = 1024 bytes, so here we erase first 20Kb of Flash
set ERASE_SECTORS=20 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19

REM specify which file you want to upload
set FILE_TO_UPLOAD_PATH=D:\Projects\stm32\bin\app.hex

REM RUN of utility
.\STMFlashLoader.exe -c --pn %COM_PORT_NUMBER% -i %MEMORY_MAP% -e --sec %ERASE_SECTORS% -d --fn %FILE_TO_UPLOAD_PATH%

ENDLOCAL
