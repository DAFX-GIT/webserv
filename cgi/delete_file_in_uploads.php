#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");

$method = $_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN';

echo "<html><body>";
echo "<h1>Hello from PHP CGI!</h1>";
echo "<p><b>HTTP Method Detected:</b> " . htmlspecialchars($method) . "</p>";

if ($method === 'DELETE') {
    $queryString = $_SERVER['QUERY_STRING'] ?? '';
    
    if (!empty($queryString)) {
        parse_str($queryString, $params);
        if (isset($params['file'])) {
            $targetFile = basename($params['file']); 
            
            $scriptDir = $_SERVER['BIN_PATH'];

			$filePath = $scriptDir . "uploads/" . $targetFile;

            echo "<h2>Processing Real File Deletion:</h2>";
            echo "<p>Target path: <code>" . htmlspecialchars($filePath) . "</code></p>";

            if (file_exists($filePath)) {
                if (unlink($filePath)) {
                    echo "<p style='color: green;'><b>Status:</b> Success! The file was physically deleted from the disk.</p>";
                } else {
                    echo "<p style='color: red;'><b>Status:</b> OS refused to delete the file (Check permissions!).</p>";
                }
            } else {
                echo "<p style='color: red;'><b>Status:</b> Error. File does not exist at " . htmlspecialchars($filePath) . "</p>";
            }
        }
    } else {
        echo "<p style='color: orange;'><b>Notice:</b> No file specified in query string.</p>";
    }
}
echo "</body></html>";
?>