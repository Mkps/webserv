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

    fetch('http://localhost:8081/upload', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('result').innerText = 'File uploaded successfully: ' + data;
    })
    .catch(error => {
        document.getElementById('result').innerText = 'Error uploading file: ' + error;
    });
});
