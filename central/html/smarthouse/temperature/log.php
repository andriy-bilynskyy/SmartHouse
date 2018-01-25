<?php
    header("Refresh:5");
    include("../session.php");
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        if(mysqli_query($db,"DELETE FROM data_log WHERE unit='BLUETOOTH' or unit='TEMPERATURE'"))
        {
            mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('ADMIN', 'Temperature log erased from WEB.')");
            header("Refresh:0");
        }
    }
?>

<html> 
    <head>
        <title>Temperature monitor log</title>
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
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Temperature monitor log</b></div><br/>
        <div style = "padding:3px;"><a href = "control.php">Temperature monitor control</a></div><br/>
        <div style = "padding:3px;"><a href = "sub.php">Subscribed temperature sensors editor</a></div><br/>
        <div style = "padding:3px;"><a href = "main.php">Back</a></div><br/>
        <form action = "" method = "post">
            <input type = "submit" value = "Clear log"/>
        </form>
        <table cellpadding=5px><col style="width:19ch"><tr><th>time</th><th>message</th></tr>
            <?php
                $result = mysqli_query($db,"SELECT date_time, message FROM data_log WHERE unit='BLUETOOTH' or unit='TEMPERATURE'");
                while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
                {
                    echo "<tr><td>".$row["date_time"]."</td><td>".$row["message"]."</td></tr>";
                }
            ?>
        </table>
    </body>
</html>