async function fetchSummary() {
  try {
    const response = await fetch('http://127.0.0.1:5000/summary');
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    const data = await response.json();
    const tbody = document.getElementById('summary');
    tbody.innerHTML = '';
    data.forEach((item) => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${item.road}</td>
        <td>${item.classification}</td>
        <td>${item.cars_5s}</td>
        <td>${item.cars_60s}</td>
        <td>${item.avg_speed_kmh.toFixed(1)}</td>
      `;
      tbody.appendChild(tr);
    });
  } catch (err) {
    console.error('Failed to fetch summary', err);
  }
}

setInterval(fetchSummary, 1000);
fetchSummary();


