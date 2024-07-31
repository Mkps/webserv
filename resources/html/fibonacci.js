document.getElementById('fibonacciForm').addEventListener('submit', function(e) {
    e.preventDefault(); // Prevent the default form submission

    // Get form data
    const formData = new FormData(this);
    
    
    // Make request
    fetch('/fibonacci.php', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        console.log(data)
        document.getElementById('resFibonacci').innerHTML = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
});