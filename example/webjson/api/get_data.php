<?php
require('connect.php');

function get_data($start_date, $start_time, $end_date, $end_time, $sensors_str){
	global $mysqli;
	$query_str = sprintf("SELECT *  FROM `data` WHERE CONCAT(`date`,' ', `time`) >= '%s %s' AND CONCAT(`date`,' ', `time`) <= '%s %s'", $start_date, $start_time, $end_date, $end_time);
	$result = $mysqli->query($query_str);
	$JSON_str = '{"data":[';
	if($mysqli->field_count){
		while($row = $result->fetch_assoc()) {
			$obj_str = '{"date": "' . $row['date'] . '",';
			$obj_str .= '"time": "' . $row['time'] . '",';
			
			$sensors = explode(",", $sensors_str);
			foreach ($sensors as &$sensor) {
				$data = get_sensor_data($row['sensors'], $sensor);
				
				if(strlen($data) > 0){
					$obj_str .=  $data . ',';
				}
			}

			$obj_str = rtrim($obj_str, ",");
			$obj_str .= '}'; 
			
			$JSON_str .= $obj_str . ',';
		}
	}
	$JSON_str = rtrim($JSON_str, ",");
	$JSON_str .= ']}'; 
	return $JSON_str;
}

function get_sensor_data($str, $sensor){
	$sensor_data = explode(";", $str);
	foreach ($sensor_data as &$data) {
		$element = explode("=", $data);
		
		if($element[0] == $sensor){
			return '"' . $sensor . '" : ' . $element[1];
		}
	}
	
	return "";
}
?>