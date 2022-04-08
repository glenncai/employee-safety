<?php

include "db.php";

$MondayData = [];
$TuesdayData = [];
$WednesdayData = [];
$ThursdayData = [];
$FridayData = [];
$SaturdayData = [];
$SundayData = [];

try {
    $stmt = $pdo->prepare('SELECT * FROM traffic_assistant');
    $stmt->execute();
    while ($row = $stmt->fetch()) {
        $date = $row->date;
        $dayOfWeek = date('w', strtotime($date));
        
        switch ($dayOfWeek) {
            case 1:
                array_push($MondayData, $row->speed);
                break;
            case 2:
                array_push($TuesdayData, $row->speed);
                break;
            case 3:
                array_push($WednesdayData, $row->speed);
                break;
            case 4:
                array_push($ThursdayData, $row->speed);
                break;
            case 5:
                array_push($FridayData, $row->speed);
                break;
            case 6:
                array_push($SaturdayData, $row->speed);
                break;
            case 0:
                array_push($SundayData, $row->speed);
                break;
        }
    }
} catch (PDOException $ex) {
    echo filterInput("Internal error, please contact support");
    error_log("index.php, SQL error=" . $ex->getMessage());
    return;
}

$MondayDataAmount = count($MondayData);
$TuesdayDataAmount = count($TuesdayData);
$WednesdayDataAmount = count($WednesdayData);
$ThursdayDataAmount = count($ThursdayData);
$FridayDataAmount = count($FridayData);
$SaturdayDataAmount = count($SaturdayData);
$SundayDataAmount = count($SundayData);

$MondaySpeed = array_filter($MondayData);
$MondayAverageSpeed = array_sum($MondaySpeed) / $MondayDataAmount;

$TuesdaySpeed = array_filter($TuesdayData);
$TuesdayAverageSpeed = array_sum($TuesdaySpeed) / $TuesdayDataAmount;

$WednesdaySpeed = array_filter($WednesdayData);
$WednesdayAverageSpeed = array_sum($WednesdaySpeed) / $WednesdayDataAmount;

$ThursdaySpeed = array_filter($ThursdayData);
$ThursdayAverageSpeed = array_sum($ThursdaySpeed) / $ThursdayDataAmount;

$FridaySpeed = array_filter($FridayData);
$FridayAverageSpeed = array_sum($FridaySpeed) / $FridayDataAmount;

$SaturdaySpeed = array_filter($SaturdayData);
$SaturdayAverageSpeed = array_sum($SaturdaySpeed) / $SaturdayDataAmount;

$SundaySpeed = array_filter($SundayData);
$SundayAverageSpeed = array_sum($SundaySpeed) / $SundayDataAmount;

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Traffic Assistant</title>
    <style>
        #chartdiv {
            width: 100%;
            height: 500px;
            margin-bottom: 20px;
        }
        h1, h2, h3 {
            text-align: center;
        }
        .iframe-container {
            text-align: center;
        }
        iframe {
            margin-right: 10px;
            margin-bottom: 10px;
        }
        }
    </style>
</head>
<body>
    <h1>Smart Traffic Assistant</h1>
    <h2>IERG4230 Session(A) Group10 Data Collection</h2>
    <h3>CAI Longhua / 1155126875</h2>
    <h3>YANG Xu / 1155107940</h2>
    
    <div id="chartdiv"></div>
    
    <div class="iframe-container">
         <iframe src="http://glenncai.com/Monday.php" title="Monday Data"></iframe>
         <iframe src="http://glenncai.com/Tuesday.php" title="Tuesday Data"></iframe>
         <iframe src="http://glenncai.com/Wednesday.php" title="Wednesday Data"></iframe>
         <iframe src="http://glenncai.com/Thursday.php" title="Thursday Data"></iframe>
         <iframe src="http://glenncai.com/Friday.php" title="Friday Data"></iframe>
         <iframe src="http://glenncai.com/Saturday.php" title="Saturday Data"></iframe>
         <iframe src="http://glenncai.com/Sunday.php" title="Sunday Data"></iframe>
    </div>
    
    <!-- Resources -->
    <script src="https://cdn.amcharts.com/lib/5/index.js"></script>
    <script src="https://cdn.amcharts.com/lib/5/xy.js"></script>
    <script src="https://cdn.amcharts.com/lib/5/themes/Animated.js"></script>
    
    <!-- Chart code -->
    <script>
    am5.ready(function() {
    
    // Create root element
    // https://www.amcharts.com/docs/v5/getting-started/#Root_element
    var root = am5.Root.new("chartdiv");
    
    // Set themes
    // https://www.amcharts.com/docs/v5/concepts/themes/
    root.setThemes([am5themes_Animated.new(root)]);
    
    // Create chart
    // https://www.amcharts.com/docs/v5/charts/xy-chart/
    var chart = root.container.children.push(
      am5xy.XYChart.new(root, {
        panX: false,
        panY: false,
        wheelX: "panX",
        wheelY: "zoomX",
        layout: root.verticalLayout
      })
    );
    
    // Add scrollbar
    // https://www.amcharts.com/docs/v5/charts/xy-chart/scrollbars/
    chart.set(
      "scrollbarX",
      am5.Scrollbar.new(root, {
        orientation: "horizontal"
      })
    );
    
    var data = [
      {
        year: "Monday",
        amount: <?php echo $MondayDataAmount; ?>,
        speed: <?php echo $MondayAverageSpeed; ?>
      },
      {
        year: "Tuesday",
        amount: <?php echo $TuesdayDataAmount; ?>,
        speed: <?php echo $TuesdayAverageSpeed; ?>
      },
      {
        year: "Wednesday",
        amount: <?php echo $WednesdayDataAmount; ?>,
        speed: <?php echo $WednesdayAverageSpeed; ?>
      },
      {
        year: "Thursday",
        amount: <?php echo $ThursdayDataAmount; ?>,
        speed: <?php echo $ThursdayAverageSpeed; ?>
      },
      {
        year: "Friday",
        amount: <?php echo $FridayDataAmount; ?>,
        speed: <?php echo $FridayAverageSpeed; ?>
      },
      {
        year: "Saturday",
        amount: <?php echo $SaturdayDataAmount; ?>,
        speed: <?php echo $SaturdayAverageSpeed; ?>
      },
      {
        year: "Sunday",
        amount: <?php echo $SundayDataAmount; ?>,
        speed: <?php echo $SundayAverageSpeed; ?>
      }
    ];
    
    // Create axes
    // https://www.amcharts.com/docs/v5/charts/xy-chart/axes/
    var xAxis = chart.xAxes.push(
      am5xy.CategoryAxis.new(root, {
        categoryField: "year",
        renderer: am5xy.AxisRendererX.new(root, {}),
        tooltip: am5.Tooltip.new(root, {})
      })
    );
    
    xAxis.data.setAll(data);
    
    var yAxis = chart.yAxes.push(
      am5xy.ValueAxis.new(root, {
        min: 0,
        extraMax: 0.1,
        renderer: am5xy.AxisRendererY.new(root, {})
      })
    );
    
    
    // Add series
    // https://www.amcharts.com/docs/v5/charts/xy-chart/series/
    var series1 = chart.series.push(
      am5xy.ColumnSeries.new(root, {
        name: "Amount",
        xAxis: xAxis,
        yAxis: yAxis,
        valueYField: "amount",
        categoryXField: "year",
        tooltip:am5.Tooltip.new(root, {
          pointerOrientation:"horizontal",
          labelText:"{name} in {categoryX}: {valueY} {info}"
        })
      })
    );
    
    series1.columns.template.setAll({
      tooltipY: am5.percent(10),
      templateField: "columnSettings"
    });
    
    series1.data.setAll(data);
    
    var series2 = chart.series.push(
      am5xy.LineSeries.new(root, {
        name: "Average Speed",
        xAxis: xAxis,
        yAxis: yAxis,
        valueYField: "speed",
        categoryXField: "year",
        tooltip:am5.Tooltip.new(root, {
          pointerOrientation:"horizontal",
          labelText:"{name} in {categoryX}: {valueY} {info}"
        })    
      })
    );
    
    series2.strokes.template.setAll({
      strokeWidth: 3,
      templateField: "strokeSettings"
    });
    
    
    series2.data.setAll(data);
    
    series2.bullets.push(function () {
      return am5.Bullet.new(root, {
        sprite: am5.Circle.new(root, {
          strokeWidth: 3,
          stroke: series2.get("stroke"),
          radius: 5,
          fill: root.interfaceColors.get("background")
        })
      });
    });
    
    chart.set("cursor", am5xy.XYCursor.new(root, {}));
    
    // Add legend
    // https://www.amcharts.com/docs/v5/charts/xy-chart/legend-xy-series/
    var legend = chart.children.push(
      am5.Legend.new(root, {
        centerX: am5.p50,
        x: am5.p50
      })
    );
    legend.data.setAll(chart.series.values);
    
    // Make stuff animate on load
    // https://www.amcharts.com/docs/v5/concepts/animations/
    chart.appear(1000, 100);
    series1.appear();
    
    }); // end am5.ready()
    </script>
    
</body>
</html>

