<?php

// http://127.0.0.1/api/api.php?startDate=2018-04-17&startTime=06:00:00&endDate=2018-04-17&endTime=18:00:00&sensors=I,TW

if(isset($_GET["startDate"])){
	
} else {
	die('{ "error" : "startDate is needed" }');
}

if(isset($_GET["startTime"])){
	
} else {
	die('{ "error" : "startTime is needed" }');
}

if(isset($_GET["endDate"])){
	
} else {
	die('{ "error" : "endDate is needed" }');
}

if(isset($_GET["endTime"])){
	
} else {
	die('{ "error" : "endTime is needed" }');
}

if(isset($_GET["sensors"])){
	
} else {
	die('{ "error" : "sensors is needed" }');
}

include('get_data.php');

echo get_data($_GET["startDate"], $_GET["startTime"], $_GET["endDate"], $_GET["endTime"], $_GET["sensors"]);

?>