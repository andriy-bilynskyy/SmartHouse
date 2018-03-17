## Water valve control service
This part of software analyzes state of subscribed water leak sensors.
Based on this information it controls the water valve.
#### Compile executable
source files location: *central/water_valve_ctrl*
necessarily libraries:
- libmysqlclient-dev
- [Wiring Pi](http://wiringpi.com/examples/quick2wire-and-wiringpi/install-and-testing/)

	make
#### Insatallation
	sudo cp valve_ctrl_serv /etc/init.d/
	sudo update-rc.d valve_ctrl_serv defaults
#### Usage
	sudo /etc/init.d/valve_ctrl status
	sudo /etc/init.d/valve_ctrl start
	sudo /etc/init.d/valve_ctrl stop
	sudo /etc/init.d/valve_ctrl restart
