default:help
help:
	@echo "available targets:"
	@echo "clean            - clean generated files"
	@echo "waterleak        - build water leak sensor FW"
	@echo "temperature      - build temperature sensor FW"
	@echo "flash            - flash app FW"
	@echo "flash_softdevice - flash softdevice FW"
	@echo "erase            - flash erase"
clean:
	@make --no-print-directory -C src clean
waterleak:
	@make --no-print-directory -C src all SENSOR_SRC=sensor_waterleak.c
temperature:
	@make --no-print-directory -C src all SENSOR_SRC=sensor_temperature.c
flash:
	@make --no-print-directory -C src flash
flash_softdevice:
	@make --no-print-directory -C src flash_softdevice
erase:
	@make --no-print-directory -C src erase