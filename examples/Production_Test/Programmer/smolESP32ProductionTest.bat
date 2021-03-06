@echo Programming the SparkX smôl ESP32. If this looks incorrect, abort and retry.
@pause
:loop
@echo Flashing firmware...
for /f "tokens=2 delims=(" %%a in ('wmic path win32_pnpentity get caption /format:list ^| find "COM" ^| find "CP210x"') do (
    for /f "tokens=1 delims=)" %%b in ("%%a") do (
        @esptool.exe --chip esp32 --port %%b --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 smolESP32ProductionTest.bootloader.bin 0x10000 smolESP32ProductionTest.bin 0x8000 smolESP32ProductionTest.partitions.bin
    )
)
@echo Done programming! Ready for the next board.
@pause
goto loop
