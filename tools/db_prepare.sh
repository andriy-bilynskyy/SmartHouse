#!/bin/bash

if (( $# == 3 )); then
    sqlrootpass=$1
    user=$2
    password=$3
    mysql -u root -p$sqlrootpass << EOF
    DROP DATABASE IF EXISTS smart_house;
    CREATE DATABASE smart_house;
    GRANT ALL ON smart_house.* TO $user@localhost IDENTIFIED BY "$password";
    exit
EOF
    mysql -u$user -p$password smart_house < ./db_mysql.sql
    echo "<?php"                                      > ../central/html/smarthouse/credentials.php
    echo "    define('DB_USERNAME', '$user');"       >> ../central/html/smarthouse/credentials.php
    echo "    define('DB_PASSWORD', '$password');"   >> ../central/html/smarthouse/credentials.php
    echo "?>"                                        >> ../central/html/smarthouse/credentials.php

    echo "#ifndef DB_CREDENTIAL_H"                          > ../central/components/dbhandler/credentials.h
    echo "#define DB_CREDENTIAL_H"                          >> ../central/components/dbhandler/credentials.h
    echo "    #define DB_CREDENTIAL_USERNAME \"$user\""     >> ../central/components/dbhandler/credentials.h
    echo "    #define DB_CREDENTIAL_PASSWORD \"$password\"" >> ../central/components/dbhandler/credentials.h
    echo "#endif"                                           >> ../central/components/dbhandler/credentials.h

    echo Created database smart_house. Access by: \"$user\":\"$password\"
else
    echo Wrong usage: Use $0 SQL_root_password DB_smart_house_user DB_smart_house_password
fi

exit 0