<?php

require_once "../api/ApiClient.php";

$api = new ApiClient();

$username = $_POST["username"] ?? "";
$password = $_POST["password"] ?? "";

$token = $api->login($username, $password);

if (empty($token))
{
    http_response_code(401);
    die("Invalid credentials");
}

setcookie(
    "session_token",
    $token,
    [
        "path" => "/",
        "httponly" => true
    ]
);

header("Location: /site/dashboard.html");
exit;