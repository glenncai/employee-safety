<?php

$timezone = date_default_timezone_set("Asia/Hong_Kong");

$db['DB_HOST'] = "localhost";
$db['DB_USER'] = "glenkftu_ierg4230";
$db['DB_PASS'] = "t^SGQ9AnBBAg";
$db['DB_NAME'] = "glenkftu_ierg4230";

// Set them to be constant, it is more secure to myself
foreach($db as $key => $value) {
    define($key, $value);
}

// Set DSN
$dsn = 'mysql:host=' . DB_HOST . ';port=3306' . ';dbname=' . DB_NAME;

try {
    // Create a PDO instance
    $pdo = new PDO($dsn, DB_USER, DB_PASS);
    $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    // if ($pdo) {
    //     echo 'Connected to DB successfully!' . '<br>';
    // }
} catch (PDOException $ex) {
    error_log("db.php, SQL error=" . $ex->getMessage());
    return;
}

?>