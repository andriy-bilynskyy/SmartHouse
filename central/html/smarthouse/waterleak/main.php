<?php
    include("../session.php");
?>

<html>
    <head>
        <title>Water leak</title>
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
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Water leak</b></div><br/>
        <div style = "padding:3px;"><a href = "control.php">Water leak monitor control</a></div><br/>
        <div style = "padding:3px;"><a href = "sub.php">Subscribed water leak sensors editor</a></div><br/>
        <div style = "padding:3px;"><a href = "log.php">Water leak monitor log</a></div><br/>
        <div style = "padding:3px;"><a href = "../welcome.php">Back</a></div><br/>
    </body>
</html>
