<?php
    include("session.php");
    if($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $act_user  = mysqli_real_escape_string($db,$_POST['username']);
        $act_pass  = mysqli_real_escape_string($db,$_POST['oldpass']);
        $new_pass  = mysqli_real_escape_string($db,$_POST['newpass']);
        $conf_pass = mysqli_real_escape_string($db,$_POST['confpass']);
        
        if($act_user != "")
        {
            if($_REQUEST['action'] == "Add/Update")
            {
                $result = mysqli_query($db,"SELECT password FROM users WHERE user_name = '$act_user'");
                if(mysqli_num_rows($result) == 1)
                {
                    $row = mysqli_fetch_array($result, MYSQLI_ASSOC);
                    if($row['password'] == $act_pass)
                    {
                        if(strlen($new_pass) >= 6)
                        {
                            if($new_pass == $conf_pass)
                            {
                                mysqli_query($db,"UPDATE users SET password='$new_pass' WHERE user_name='$act_user'");
                                mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('ADMIN', 'Password changed from WEB. [$act_user:$new_pass]')");
                                $error = "Password changed.";
                            }
                            else
                            {
                                $new_pass = "";
                                $conf_pass = "";
                                $error = "New passwords don't match.";
                            }
                        }
                        else
                        {
                            $new_pass = "";
                            $conf_pass = "";
                            $error = "New password is too short. Should be 6 characters at least.";
                        }
                    }
                    else
                    {
                        $act_pass = "";
                        $error = "Invalid password for user ".$act_user.".";
                    }
                }
                else
                {
                    if(strlen($new_pass) >= 6)
                    {
                        if($new_pass == $conf_pass)
                        {
                            mysqli_query($db,"INSERT INTO users (user_name, password) VALUES ('$act_user', '$new_pass')");
                            mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('ADMIN', 'New user created from WEB. [$act_user:$new_pass]')");
                            $error = "New user created.";
                        }
                        else
                        {
                            $new_pass = "";
                            $conf_pass = "";
                            $error = "New passwords don't match.";
                        }
                    }
                    else
                    {
                        $new_pass = "";
                        $conf_pass = "";
                        $error = "New password is too short. Should be 6 characters at least.";
                    }
                }
            }
            else if ($_REQUEST['action'] == "Delete")
            {
                $result = mysqli_query($db,"SELECT password FROM users WHERE user_name = '$act_user'");
                if(mysqli_num_rows($result) == 1)
                {
                    $row = mysqli_fetch_array($result, MYSQLI_ASSOC);
                    if($row['password'] == $act_pass)
                    {
                        $result1 = mysqli_query($db,"SELECT user_name, password FROM users");
                        if(mysqli_num_rows($result1) > 1)
                        {
                            mysqli_query($db, "DELETE FROM users WHERE user_name='$act_user'");
                            mysqli_query($db,"INSERT INTO data_log (unit, message) VALUES ('ADMIN', 'User removed from WEB. [$act_user]')");
                            $error = "User removed.";
                        }
                        else
                        {
                            $error = "It's latest user.";
                        }
                    }
                    else
                    {
                        $act_pass = "";
                        $error = "Invalid password for user ".$act_user.".";
                    }

                }
                else
                {
                    $error = "User not exists.";
                }
            }
        }
        else
        {
            $error = "User name is empty.";
        }
    }
?>

<html>
    <head>
        <title>Change Password</title>
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
        <div align = "center">
            <div style = "width:29ch; border: solid 1px #333333; " align = "left">
                <div style = "background-color:#333333; color:#FFFFFF; padding:3px;"><b>Change password</b></div>
                    <div style = "margin:2ch">
                        <form action = "" method = "post">
                            <label>User name        :</label><input style='width:100%;' type = "text"     name = "username" class = "box" value = <?php echo $act_user; ?> ><br/><br/>
                            <label>Old password     :</label><input style='width:100%;' type = "password" name = "oldpass"  class = "box" value = <?php echo $act_pass; ?> ><br/><br/>
                            <label>New password     :</label><input style='width:100%;' type = "password" name = "newpass"  class = "box" value = <?php echo $new_pass; ?> ><br/><br/>
                            <label>Confirm password :</label><input style='width:100%;' type = "password" name = "confpass" class = "box" value = <?php echo $conf_pass; ?> ><br/><br/>
                            <input style='width:49%;' name='action' type = "submit" value = "Add/Update">
                            <input style='width:49%;' name='action' type = "submit" value = "Delete"><br/>
                        </form>
                    <div style = "color:#cc0000; margin-top:10px"><?php echo $error; ?></div>
                </div>
            </div>
            <div style = "padding:3px;"><a href = "welcome.php">Back</a></div>
        </div>
    </body>
</html>