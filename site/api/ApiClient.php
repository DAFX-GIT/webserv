<?php

class ApiClient
{
    private string $baseUrl = "http://92.170.205.67:8000";

    public function login(string $username, string $password): string
    {
        $response = $this->request(
            "POST",
            "/login",
            [
                "username" => $username,
                "password" => $password
            ]
        );

        return $response["token"] ?? "";
    }

    public function getUser(string $token): string
    {
        $response = $this->request(
            "GET",
            "/me",
            null,
            [
                "Authorization: Bearer $token"
            ]
        );

        return $response["username"] ?? "";
    }

    public function register(string $username, string $password): bool
    {
        $response = $this->request(
            "POST",
            "/register",
            [
                "username" => $username,
                "password" => $password
            ]
        );

        return $response["success"] ?? false;
    }

    public function logout(string $token): bool
    {
        $response = $this->request(
            "POST",
            "/logout",
            null,
            [
                "Authorization: Bearer $token"
            ]
        );

        return $response["success"] ?? false;
    }

    private function request(
        string $method,
        string $endpoint,
        ?array $data = null,
        array $headers = []
    ): array
    {
        $ch = curl_init();

        $headers[] = "Content-Type: application/json";

        curl_setopt_array($ch, [
            CURLOPT_URL => $this->baseUrl . $endpoint,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_CUSTOMREQUEST => $method,
            CURLOPT_HTTPHEADER => $headers
        ]);

        if ($data !== null)
        {
            curl_setopt(
                $ch,
                CURLOPT_POSTFIELDS,
                json_encode($data)
            );
        }

        $result = curl_exec($ch);

        if ($result === false)
        {
            curl_close($ch);
            return [];
        }

        curl_close($ch);

        return json_decode($result, true) ?? [];
    }
}