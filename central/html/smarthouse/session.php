<?php
    include("config.php");
    session_start();
    $user_check = $_SESSION['login_user'];
    $result = mysqli_query($db,"SELECT user_name FROM users WHERE user_name = '$user_check'");
    if(mysqli_num_rows($result) == 1)
    {
        $row = mysqli_fetch_array($result, MYSQLI_ASSOC);
        $login_session = $row['user_name'];
        if(!isset($_SESSION['login_user']))
        {
            header("location:login.php");
        }
    }
    else
    {
        if(session_destroy())
        {
            header("Location: index.php");
        }
    }
?>
