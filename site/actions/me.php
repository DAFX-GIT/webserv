<?php

require_once '../api/ApiClient.php';

header('Content-Type: application/json');

$token = $_COOKIE['session_token'] ?? '';

if (empty($token))
{
    http_response_code(401);

    echo json_encode([
        'error' => 'No session'
    ]);

    exit;
}

$api = new ApiClient();

$username = $api->getUser($token);

if (empty($username))
{
    http_response_code(401);

    echo json_encode([
        'error' => 'Invalid session'
    ]);

    exit;
}

echo json_encode([
    'username' => $username
]);