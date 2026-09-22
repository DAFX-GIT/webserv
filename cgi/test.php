#!/usr/bin/php-cgi
<?php
// Force CGI to output the content type header
header("Content-Type: text/html");

echo "<html><body>";
echo "<h1>PHP CGI is Working!</h1>";

// Test if your webserv is passing Query Strings correctly (?user=jack)
if (isset($_GET['user'])) {
    echo "<p>Hello, " . htmlspecialchars($_GET['user']) . "!</p>";
} else {
    echo "<p>No user query string detected.</p>";
}

// Display full environment details to check your other variables
echo "<h2>Environment Variables:</h2><pre>";
print_r($_SERVER);
echo "</pre></body></html>";
?>