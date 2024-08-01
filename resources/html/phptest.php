<?php
header("Content-Type: text/html");

echo "<!DOCTYPE html>\n";
echo "<html lang='en'>\n";
echo "<head>\n";
echo "    <meta charset='UTF-8'>\n";
echo "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
echo "    <title>CGI PHP Script</title>\n";
echo "</head>\n";
echo "<body>\n";
//$post = file_get_contents('php://input');
// Check the request method
$method = $_SERVER['REQUEST_METHOD'];
//var_dump($_POST);
// parse_str(file_get_contents("php://input"), $data);
// var_dump($data);
//var_dump($_POST);
echo "<h1>Request Method: $method</h1>\n";
if ($method === 'GET') {
    // Handle GET request
    echo "<h2>GET Parameters:</h2>\n";
    echo "<ul>\n";
    foreach ($_GET as $key => $value) {
        echo "    <li>$key: $value</li>\n";
    }
    echo 'Hello ' . htmlspecialchars($_GET["firstName"]) . '!';
    echo "</ul>\n";
} elseif ($method === 'POST') {
    // Handle POST request
    echo "<h2>POST Parameters:</h2>\n";
    echo "<ul>\n";
    foreach ($_POST as $key => $value) {
        echo "    <li>$key: $value</li>\n";
    }
    echo 'Hello ' . htmlspecialchars($_POST["firstName"]) . '!';
    echo "</ul>\n";
} else {
    echo "<p>Unsupported request method.</p>\n";
}

echo "</body>\n";
echo "</html>";
?>
