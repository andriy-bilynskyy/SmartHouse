<?php
    header("refresh: 3;");
    include("../session.php");
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        switch($_REQUEST['action'])
        {
        default:
            $dev_addr = mysqli_real_escape_string($db,$_REQUEST['action']);
            if(!mysqli_query($db, "INSERT INTO subscribed_temperature_sensors (address) VALUES ('$dev_addr')"))
            {
                mysqli_query($db, "DELETE FROM subscribed_temperature_sensors WHERE address='$dev_addr'");
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('TEMPERATURE', 'Unsubscribed sensor $dev_addr')");
            }else{
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('TEMPERATURE', 'Subscribed sensor $dev_addr')");
            }
            break;
        }
        header("refresh: 0;");
    }
?>

<html>
    <head>
        <title>Temperature monitor control</title>
        <link rel="icon" href="../images/icon.ico">
        <link rel="stylesheet" type="text/css" href="../css/main.css">
        <?php
        if(isMobile())
        {
        ?>
        <link rel="stylesheet" type="text/css" href="../css/mobile.css">
        <?php
        }else{
        ?>
        <link rel="stylesheet" type="text/css" href="../css/desktop.css">
        <?php
        }
        ?>
    </head>

    <body bgcolor = "#FFFFFF">
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Temperature monitor control</b></div><br/>
        <div style = "padding:3px;"><a href = "log.php">Temperature monitor log</a></div><br/>
        <div style = "padding:3px;"><a href = "sub.php">Subscribed temperature sensors editor</a></div><br/>
        <div style = "padding:3px;"><a href = "main.php">Back</a></div><br/>
        <form action = "" method = "post">
            <div style = "background-color:#555555; color:#FFFFFF; padding:3px;"><b>Temperature sensors</b></div><br/>
            <table cellpadding=5px><tr><th>subscription</th><th>address</th><th>description</th><th>value, C</th><th>last seen</th><th>voltage</th></tr>
                <?php
                    $result = mysqli_query($db, "SELECT 'yes' as subscribed, subscribed_temperature_sensors.address as address, subscribed_temperature_sensors.description, on_line_sensors.value, TIMEDIFF(NOW(), on_line_sensors.last_seen) as time, on_line_sensors.voltage FROM subscribed_temperature_sensors LEFT JOIN on_line_sensors ON subscribed_temperature_sensors.address=on_line_sensors.address and on_line_sensors.type='temp' UNION SELECT 'no' AS subscribed, address, NULL AS description, value, TIMEDIFF(NOW(),on_line_sensors.last_seen) as time, on_line_sensors.voltage FROM on_line_sensors WHERE type = 'temp' and address NOT IN  (SELECT address FROM subscribed_temperature_sensors) ORDER BY address");
                    while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
                    {
                        if($row["value"] == "key")
                        {
                            echo "<tr style='color:#ff0000;'>";
                        }else{
                            echo "<tr>";
                        }
                            echo "<td>".$row["subscribed"]."</td>";
                            if($row["value"] == "key")
                            {
                                echo "<td><input style='width:20ch; color:#ff0000;' name='action' type = 'submit' value = ".$row["address"]."></td>";
                            }else{
                                echo "<td><input style='width:20ch;' name='action' type = 'submit' value = ".$row["address"]."></td>";
                            }
                            echo "<td>".$row["description"]."</td>";
                            if($row["value"] != NULL)
                            {
                                echo "<td>".$row["value"]."</td>";
                            }
                            else
                            {
                                echo "<td>off-line</td>";
                            }
                            if($row["time"] != NULL)
                            {
                                echo "<td>".$row["time"]." ago</td>";
                            }
                            else
                            {
                                echo "<td>ancient times</td>";
                            }
                            if($row["voltage"] != NULL)
                            {
                                echo "<td>".$row["voltage"]."V</td>";
                            }
                            else
                            {
                                echo "<td>N/A</td>";
                            }
                        echo "</tr>";
                    }
                ?>
            </table>
        </form>
    </body>
</html>
