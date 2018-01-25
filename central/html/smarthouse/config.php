<?php
    include("credentials.php");
    define('DB_SERVER',   'localhost');
    define('DB_DATABASE', 'smart_house');
    $db = mysqli_connect(DB_SERVER,DB_USERNAME,DB_PASSWORD,DB_DATABASE);
    if(!$db)
    {
        echo "Error: Can't connect to MySQL server." . PHP_EOL;
        echo "Error code: " . mysqli_connect_errno() . PHP_EOL;
        echo "Error text: " . mysqli_connect_error() . PHP_EOL;
        exit;
    }

    function isMobile()
    {
        return preg_match("/(android|webos|avantgo|iphone|ipad|ipod|blackbe‌​rry|iemobile|bolt|bo‌​ost|cricket|docomo|f‌​one|hiptop|mini|oper‌​a mini|kitkat|mobi|palm|phone|pie|tablet|up\.browser|up\.link|‌​webos|wos)/i", $_SERVER["HTTP_USER_AGENT"]);
    }

    function phpAlert($msg) {
        echo '<script type="text/javascript">alert("' . $msg . '")</script>';
    }
?>
