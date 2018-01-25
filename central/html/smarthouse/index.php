<?php
    include("config.php");
    session_start();
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $myusername = mysqli_real_escape_string($db,$_POST['username']);
        $mypassword = mysqli_real_escape_string($db,$_POST['password']); 
        $result = mysqli_query($db,"SELECT user_name FROM users WHERE user_name = '$myusername' and password = '$mypassword'");
        if(mysqli_num_rows($result) == 1)
        {
            $_SESSION['login_user'] = $myusername;
            header("location: welcome.php");
        }else{
            mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('ADMIN', 'Unsuccessfull login from WEB. [$myusername:$mypassword]')");
            $error = "Login name or password is invalid";
        }
    }
?>

<html> 
    <head>
        <title>Login Page</title>
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
    <?php
    if(isset($_SESSION['login_user']))
    {
        header("location: welcome.php");
    }
    ?>
    <body bgcolor = "#FFFFFF">
        <div align = "center">
            <div style = "width:40ch; border: solid 1px #333333; " align = "left">
                <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Login</b></div>
                    <div style = "margin:2ch">
                        <form action = "" method = "post">
                            <label>User name :</label><input input style='width:100%;' type = "text" name = "username" class = "box"><br/><br/>
                            <label>Password  :</label><input input style='width:100%;' type = "password" name = "password" class = "box"><br/><br/>
                            <input type = "submit" value = "Submit"><br/>
                        </form>
                    <div style = "color:#cc0000; margin-top:10px"><?php echo $error; ?></div>
                </div>
            </div>
        </div>
    </body>
</html>