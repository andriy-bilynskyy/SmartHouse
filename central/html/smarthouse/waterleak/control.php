<?php
    header("refresh: 3;");
    include("../session.php");
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        switch($_REQUEST['action'])
        {
        case 'water opened':
            if(mysqli_query($db, "UPDATE valves SET state=0 WHERE name='main water tap'"))
            {
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('WATER VALVE', 'Water closed from WEB.')");
            }
            break;
        case 'water closed':
            if(mysqli_query($db, "UPDATE valves SET state=1 WHERE name='main water tap'"))
            {
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('WATER VALVE', 'Water opened from WEB.')");
            }
            break;
        default:
            $dev_addr = mysqli_real_escape_string($db,$_REQUEST['action']);
            if(!mysqli_query($db, "INSERT INTO subscribed_wet_sensors (address) VALUES ('$dev_addr')"))
            {
                mysqli_query($db, "DELETE FROM subscribed_wet_sensors WHERE address='$dev_addr'");
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('WATER VALVE', 'Unsubscribed sensor $dev_addr')");
            }else{
                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('WATER VALVE', 'Subscribed sensor $dev_addr')");
            }
            break;
        }
        header("refresh: 0;");
    }
?>

<html>
    <head>
        <title>Water leak monitor control</title>
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
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Water leak monitor control</b></div><br/>
        <div style = "padding:3px;"><a href = "log.php">Water leak monitor log</a></div><br/>
        <div style = "padding:3px;"><a href = "sub.php">Subscribed water leak sensors editor</a></div><br/>
        <div style = "padding:3px;"><a href = "main.php">Back</a></div><br/>
        <form action = "" method = "post">
            <div style = "background-color:#555555; color:#FFFFFF; padding:3px;"><b>Water tap control</b></div><br/>
            <?php
            $result = mysqli_query($db,"SELECT state, op_time FROM valves WHERE name='main water tap'");
            if(mysqli_num_rows($result) == 1)
            {
                if($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
                {
                    if($row['state'])
                    {
                        echo "<input style='width:13ch; 'name='action' type = 'submit' value = 'water opened'/>";
                    }
                    else
                    {
                        echo "<input style='width:13ch; 'name='action' type = 'submit' value = 'water closed'/>";
                    }
                    echo "&nbsp;&nbsp;&nbsp;&nbsp;Last operation at ".$row['op_time'];
                }
            }
            ?>
            <br/><br/><br/>
            <div style = "background-color:#555555; color:#FFFFFF; padding:3px;"><b>Water leak sensors</b></div><br/>
            <table cellpadding=5px><tr><th>subscription</th><th>address</th><th>description</th><th>state</th><th>last seen</th><th>voltage</th></tr>
            <?php
            $result = mysqli_query($db, "SELECT 'yes' as subscribed, subscribed_wet_sensors.address as address, subscribed_wet_sensors.description, on_line_sensors.value, TIMEDIFF(NOW(), on_line_sensors.last_seen) as time, on_line_sensors.voltage FROM subscribed_wet_sensors LEFT JOIN on_line_sensors ON subscribed_wet_sensors.address=on_line_sensors.address and on_line_sensors.type='w_leak' UNION SELECT 'no' AS subscribed, address, NULL AS description, value, TIMEDIFF(NOW(),on_line_sensors.last_seen) as time, on_line_sensors.voltage FROM on_line_sensors WHERE type = 'w_leak' and address NOT IN  (SELECT address FROM subscribed_wet_sensors) ORDER BY address");
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
