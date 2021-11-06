@echo off
@echo Flashing firmware...
for /f "tokens=2 delims=(" %%a in ('wmic path win32_pnpentity get caption /format:list ^| find "COM" ^| find "CP210x"') do (
    for /f "tokens=1 delims=)" %%b in ("%%a") do (
        @echo on
        @esptool.exe --chip esp32 --port %%b --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 RGB_LED.ino.bin 0x8000 RGB_LED.ino.partitions.bin
        @echo off
    )
)
@echo Done programming!
@pause
