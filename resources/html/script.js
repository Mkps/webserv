document.getElementById('uploadForm').addEventListener('submit', function(event) {
    event.preventDefault();

    var fileInput = document.getElementById('fileInput');
    var file = fileInput.files[0];
    if (!file) {
        alert('Please select a file to upload');
        return;
    }

    var formData = new FormData();
    formData.append('file', file);

    fetch('http://localhost:8080/upload/', {
        method: 'POST',
        body: formData
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.text();
    })
    .then(data => {
        document.getElementById('result').innerHTML = 'File uploaded successfully';
    })
    .catch(error => {
        document.getElementById('result').innerHTML = 'Error uploading file: ' + error;
    });
});

