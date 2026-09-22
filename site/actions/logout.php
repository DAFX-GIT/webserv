<?php

require_once "../api/ApiClient.php";

$api = new ApiClient();

$token = $_COOKIE["session_token"] ?? "";

if (!$api->logout($token))
{
    http_response_code(401);
    die("Logout fail");
}

setcookie(
    "session_token",
    "",
    [
        "expires" => time() - 3600,
        "path" => "/"
    ]
);

header("Location: /site/login.html");
exit;