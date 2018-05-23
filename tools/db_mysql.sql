###########################################################################
#  db_mysql.sql Smart House SQL Module                                    #
#                                                                         #
#     Database schema for Smart House module                              #
#                                                                         #
#     To load:                                                            #
#         mysql -uroot -p smart_house < db_mysql.sql                      #
#                                                                         #
#                       Andrii Bilynskyi <andriy.bilynskyy@gmail.com>     #
###########################################################################

#
# Table structure for table 'users'
#

CREATE TABLE users (
    user_name VARCHAR(64) NOT NULL,
    password  VARCHAR(64) NOT NULL,

    PRIMARY KEY (user_name)
);

#
# Default values for table 'users'
#

INSERT INTO users (user_name, password) VALUES (
    'admin',
    '123456'
);

#
# Table structure for table 'data_log'
#

CREATE TABLE data_log (
    record_id int(11)      unsigned NOT NULL auto_increment, 
    date_time TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    unit      VARCHAR(64)  NOT NULL DEFAULT 'N/A',
    message   VARCHAR(253) NOT NULL DEFAULT '',

    PRIMARY KEY (record_id)
);

#
# Table structure for table 'on_line_sensors'
#

CREATE TABLE on_line_sensors (
    address   CHARACTER(17) NOT NULL,
    type      VARCHAR(24)   NOT NULL,
    voltage   CHARACTER(4)  NOT NULL,
    value     VARCHAR(24)   NOT NULL,
    last_seen TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    PRIMARY KEY (address)
);

#
# Table structure for table 'subscribed_wet_sensors'
#

CREATE TABLE subscribed_wet_sensors (
    address     CHARACTER(17) NOT NULL,
    description VARCHAR(64)   NOT NULL DEFAULT '',

    PRIMARY KEY (address)
);

#
# Table structure for table 'subscribed_temperature_sensors'
#

CREATE TABLE subscribed_temperature_sensors (
    address     CHARACTER(17) NOT NULL,
    description VARCHAR(64)   NOT NULL DEFAULT '', 
    
    PRIMARY KEY (address)
);

#
# Table structure for table 'valves'
#

CREATE TABLE valves (
    name     CHARACTER(32)  NOT NULL,
    state    binary     NOT NULL DEFAULT 0,
    op_time  TIMESTAMP  NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, 

    PRIMARY KEY (name)
);

#
# Default values for table 'valves'
#

INSERT INTO valves (name) VALUES (
    'main water tap'
);


