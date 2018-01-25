<?php
    include("session.php");
?>

<html>
    <head>
        <title>Welcome</title>
        <link rel="icon" href="images/icon.ico">
        <link rel="stylesheet" type="text/css" href="css/main.css">
        <?php
        if(isMobile())
        {
        ?>
        <link rel="stylesheet" type="text/css" href="css/mobile.css">
        <?php
        }else{
        ?>
        <link rel="stylesheet" type="text/css" href="css/desktop.css">
        <?php
        }
        ?>
    </head>

    <body bgcolor = "#FFFFFF">
        <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Welcome <?php echo $login_session; ?></b></div><br/>
        <div style = "padding:3px;"><a href = "waterleak/main.php">Water leak</a></div><br/>
        <div style = "padding:3px;"><a href = "temperature/main.php">Temperature</a></div><br/>
        <div style = "padding:3px;"><a href = "log.php">System Log</a></div><br/>
        <div style = "padding:3px;"><a href = "chpass.php">Change Password</a></div><br/>
        <div style = "padding:3px;"><a href = "logout.php">Sign Out</a></div>
    </body>
</html>
