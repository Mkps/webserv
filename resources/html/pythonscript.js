document.getElementById("getFormPy").addEventListener("submit", function (e) {
  e.preventDefault(); // Prevent the default form submission

  // Get form data
  const formData = new FormData(this);

  // Convert FormData to URL parameters
  const params = new URLSearchParams(formData);

  // Make GET request
  fetch("/pytest.py?" + params.toString(), {
    method: "GET",
  })
    .then((response) => response.text())
    .then((data) => {
      document.getElementById("resGetPy").innerHTML = data;
    })
    .catch((error) => {
      console.error("Error:", error);
    });
});

document.getElementById("postFormPy").addEventListener("submit", function (e) {
  e.preventDefault(); // Prevent the default form submission

  // Get form data
  const formData = new FormData(this);

  // Make request
  fetch("/pytest.py", {
    method: "POST",
    body: formData,
  })
    .then((response) => response.text())
    .then((data) => {
      document.getElementById("resPostPy").innerHTML = data;
    })
    .catch((error) => {
      console.error("Error:", error);
    });
});
