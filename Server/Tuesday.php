<?php

include "db.php";

echo "
    <h1>Tuesday</h1>
    <table>
      <tr>
        <th>Speed</th>
        <th>Distance1</th>
        <th>Distance2</th>
        <th>Date</th>
      </tr>
";


try {
    $stmt = $pdo->prepare('SELECT * FROM traffic_assistant');
    $stmt->execute();
    while ($row = $stmt->fetch()) {
        $dayOfWeek = date('w', strtotime($row->date));
        if ($dayOfWeek == 2) {
            echo "
              <tr>
                <td>$row->speed</td>
                <td>$row->distance1</td>
                <td>$row->distance2</td>
                <td>$row->date</td>
              </tr>
            ";
        }
    }
} catch (PDOException $ex) {
    echo filterInput("Internal error, please contact support");
    error_log("Monday.php, SQL error=" . $ex->getMessage());
    return;
}

echo "</table>";
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monday Data</title>
    <style>
        h1, table {
            text-align: center;
            display: flex;
            justify-content: center;
        }
        tr:nth-child(even){
            background-color: #f2f2f2;
        }
        th {
          background-color: #41B883;
          color: white;
        }
    </style>
</head>