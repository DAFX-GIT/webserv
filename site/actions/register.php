<?php

require_once "../api/ApiClient.php";

$api = new ApiClient();

$username = $_POST["username"] ?? "";
$password = $_POST["password"] ?? "";

$success = $api->register($username, $password);

if (!$success)
{
    http_response_code(401);
    die("Register fail");
}

header("Location: /site/login.html");
exit;