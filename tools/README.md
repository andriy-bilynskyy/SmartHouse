## New database creator
This script is used to prepare empty project database. If database exists it drops old database and creates new empty one. This script also prepares credential file to access database from software units.
#### Usage
	./db_prepare.sh SQL_root_password DB_smart_house_user DB_smart_house_password

#### Generated files
- central/html/smarthouse/credentials.php
- central/components/dbhandler/credentials.h