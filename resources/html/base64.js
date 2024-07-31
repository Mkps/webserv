function submitForm(operation) {
    const form = document.getElementById('base64Form');
    const formData = new FormData(form);
    formData.append('operation', operation);

    fetch('base64_script.py', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('resBase64').innerHTML = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
}