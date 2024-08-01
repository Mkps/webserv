document.getElementById('postFormPHP').addEventListener('submit', function(e) {
    e.preventDefault(); // Prevent the form from submitting normally

    // Get form data
    const formData = new FormData(this);

    // Send POST request
    fetch('http://localhost:8080/phptest.php', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        console.log(data);
        document.getElementById('resPostPHP').innerHTML = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
});

document.getElementById('getFormPHP').addEventListener('submit', function(e) {
    e.preventDefault(); // Prevent the default form submission

    // Get form data
    const formData = new FormData(this);
    
    // Convert FormData to URL parameters
    const params = new URLSearchParams(formData);
    
    // Make GET request
    fetch('/phptest.php?' + params.toString(), {
        method: 'GET',
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('resGetPHP').innerHTML = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
});