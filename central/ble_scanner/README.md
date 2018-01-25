## Bluetooth low energy sensors scanner
This part of software monitors available bluetooth LE sensors.
And updates the information about them and theirs state.
#### Compile executable
source files location: *central/ble_scanner*
necessarily libraries:
- libmysqlclient-dev
- libbluetooth-dev

	make
#### Insatallation
	sudo cp ble_scanner /etc/init.d/
	sudo update-rc.d ble_scanner defaults
#### Usage
	sudo /etc/init.d/ble_scanner status
	sudo /etc/init.d/ble_scanner start
	sudo /etc/init.d/ble_scanner stop
	sudo /etc/init.d/ble_scanner restart
