<?php
    include("../session.php");
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $result = mysqli_query($db, "SELECT address FROM subscribed_wet_sensors");
        while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
        {
            $addr = $row["address"];
            $desc = mysqli_real_escape_string($db,$_REQUEST[$row["address"]]);
            if(mysqli_query($db,"UPDATE subscribed_wet_sensors SET description='$desc' WHERE address='$addr'"))
            {
                if(mysqli_affected_rows($db) > 0)
                {
                    mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('WATER VALVE', 'Updated sensor $addr description - $desc')");
                }
            }
        }
    }
?>

<html> 
    <head>
        <title>Subscribed water leak sensors</title>
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
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Subscribed water leak sensors</b></div><br/>
        <div style = "padding:3px;"><a href = "control.php">Water leak monitor control</a></div><br/>
        <div style = "padding:3px;"><a href = "log.php">Water leak monitor log</a></div><br/>
        <div style = "padding:3px;"><a href = "main.php">Back</a></div><br/>
        <form action = "" method = "post">
            <input type = "submit" value = "Update"/>
            <br/><br/>
            <table cellpadding=5px><col style="width:15ch"><col style="width:100%"><tr><th>address</th><th>description</th></tr>
                <?php
                    $result = mysqli_query($db,"SELECT * FROM subscribed_wet_sensors");
                    while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
                    {
                        echo "<tr>";
                            echo "<td>".$row["address"]."</td>";
                            echo "<td>";
                                echo "<input style='width:100%;' name=".$row["address"]." type = 'input' value = '".$row["description"]."'>";
                            echo "</td>";
                        echo "</tr>";
                    }
                ?>
            </table>
        </form>
    </body>
</html>