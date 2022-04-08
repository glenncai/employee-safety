<?php

include "db.php";

$speed = $_POST['speed'];
$distance1 = $_POST['distance1'];
$distance2 = $_POST['distance2'];
$date = date("Y-m-d H:i:s");

if (isset($speed) && isset($distance1) && isset($distance2)) {
    $stmt = $pdo->prepare('INSERT INTO traffic_assistant (speed, distance1, distance2, date) VALUES (:speed, :distance1, :distance2, :date)');
    if ($speed == 432 || $speed == 216) {
        $speed = 0.39;
        $stmt->bindParam(':speed', $speed, PDO::PARAM_STR);
        $stmt->bindParam(':distance1', $distance1, PDO::PARAM_STR);
        $stmt->bindParam(':distance2', $distance2, PDO::PARAM_STR);
        $stmt->bindParam(':date', $date, PDO::PARAM_STR);
        $stmt->execute();
        echo 'Insert data to database successfully!!!';
    } else {
        $stmt->bindParam(':speed', $speed, PDO::PARAM_STR);
        $stmt->bindParam(':distance1', $distance1, PDO::PARAM_STR);
        $stmt->bindParam(':distance2', $distance2, PDO::PARAM_STR);
        $stmt->bindParam(':date', $date, PDO::PARAM_STR);
        $stmt->execute();
        echo 'Insert data to database successfully!!!';
    }
}

?>