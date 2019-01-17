<?php
$host = "localhost";
$user = "root";
$pass = "";
$db = "sim_test";
$mysqli = new mysqli($host, $user, $pass, $db);
if ($mysqli->connect_errno) {
	die('Colud not connect to host: '.$host.' :'. $mysqli->connect_error);
}
$result = $mysqli->query("SET character_set_results = 'utf8', character_set_client = 'utf8', character_set_connection = 'utf8', character_set_database = 'utf8', character_set_server = 'utf8'");
if(!$result){
	die('Colud not set te charset :'. $mysqli->connect_error);
}
?>