<?php
header("Content-Type: text/html");

function fibonacci($n) {
    if ($n <= 1) return $n;
    $a = 0;
    $b = 1;
    for ($i = 2; $i <= $n; $i++) {
        $c = $a + $b;
        $a = $b;
        $b = $c;
    }
    return $b;
}

echo "<!DOCTYPE html>\n";
echo "<html lang='en'>\n";
echo "<head>\n";
echo "    <meta charset='UTF-8'>\n";
echo "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
echo "    <title>Fibonacci Script</title>\n";
echo "</head>\n";
echo "<body>\n";


$method = $_SERVER['REQUEST_METHOD'];
$number = isset($_POST['number']) ? intval($_POST['number']) : null;
if ($method === 'POST') {
    if ($number !== null && $number >= 0) {
        echo "<h2>Fibonacci number at position " . htmlspecialchars($number) . "</h2>";
        $result = fibonacci($number);
        echo "<p>Result is: $result<p>";
    } else {
        http_response_code(400);
        echo '<p><strong>Please provide a valid integer</strong></p>';
    }
} else {
    http_response_code(405);
    echo "<p><strong>Unsupported request method</strong></p>";
}

echo "</body>\n";
echo "</html>";
?>
