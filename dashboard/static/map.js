const STATUS_COLORS = {
  CONGESTED: '#e74c3c',
  MODERATE: '#f1c40f',
  SMOOTH: '#2ecc71',
  STALE: '#64748b',
  UNKNOWN: '#64748b'
};

const CLASS_PRIORITY = {
  CONGESTED: 0,
  MODERATE: 1,
  SMOOTH: 2,
  STALE: 3,
  UNKNOWN: 4
};

const ROAD_LAYOUT = {
  Main_St: {
    points: [[140, 70], [140, 500]],
    label: 'Main St',
    labelPos: [160, 100],
    width: 16
  },
  Elm_St: {
    points: [[70, 160], [620, 160]],
    label: 'Elm St',
    labelPos: [630, 150],
    width: 14
  },
  Broadway: {
    points: [[220, 90], [580, 420]],
    label: 'Broadway',
    labelPos: [590, 360],
    width: 16
  },
  River_Rd: {
    points: [[80, 430], [650, 430]],
    label: 'River Rd',
    labelPos: [660, 420],
    width: 14
  },
  Maple_Ave: {
    points: [[310, 60], [310, 520]],
    label: 'Maple Ave',
    labelPos: [330, 90],
    width: 12
  },
  Oak_St: {
    points: [[470, 120], [470, 490]],
    label: 'Oak St',
    labelPos: [490, 150],
    width: 12
  },
  Pine_St: {
    points: [[110, 260], [620, 260]],
    label: 'Pine St',
    labelPos: [630, 250],
    width: 10
  },
  Cedar_Rd: {
    points: [[180, 520], [560, 340]],
    label: 'Cedar Rd',
    labelPos: [570, 350],
    width: 10
  }
};

const ALL_ROADS = Object.keys(ROAD_LAYOUT);
const canvas = document.getElementById('mapCanvas');
const ctx = canvas.getContext('2d');
let latestSummary = {};

function fetchSummary() {
  fetch('/summary', { credentials: 'same-origin' })
    .then((resp) => {
      if (!resp.ok) {
        throw new Error(`HTTP ${resp.status}`);
      }
      return resp.json();
    })
    .then((data) => {
      handleSummary(data);
    })
    .catch((err) => {
      console.error('Failed to fetch summary', err);
      document.getElementById('statusMessage').textContent = 'Unable to reach server...';
    });
}

function handleSummary(list) {
  latestSummary = {};
  let speedSum = 0;
  let speedCount = 0;

  list.forEach((item) => {
    latestSummary[item.road] = item;
    if (item.avg_speed_kmh > 0) {
      speedSum += item.avg_speed_kmh;
      speedCount += 1;
    }
  });

  drawMap();
  updateTable();
  updateMetrics(list.length, speedCount ? (speedSum / speedCount) : null);
  document.getElementById('updatedAt').textContent = new Date().toLocaleTimeString();
  document.getElementById('statusMessage').textContent = list.length ? 'Sensors streaming live updates' : 'Waiting for sensor telemetry...';
}

function updateMetrics(activeRoads, avgSpeed) {
  document.getElementById('metricActive').textContent = activeRoads;
  document.getElementById('metricAvgSpeed').textContent = avgSpeed !== null ? avgSpeed.toFixed(1) : '--';
}

function drawMap() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  ALL_ROADS.forEach((road) => {
    const snapshot = latestSummary[road];
    drawRoad(road, snapshot);
  });

  ALL_ROADS.forEach((road) => {
    const snapshot = latestSummary[road];
    drawVehicles(road, snapshot);
  });
}

function drawRoad(road, snapshot) {
  const layout = ROAD_LAYOUT[road];
  if (!layout) {
    return;
  }
  const classification = snapshot ? snapshot.classification : 'UNKNOWN';
  const color = STATUS_COLORS[classification] || STATUS_COLORS.UNKNOWN;
  const baseWidth = layout.width || 12;

  strokePath(layout.points, 'rgba(15, 23, 42, 0.85)', baseWidth + 6);
  strokePath(layout.points, color, baseWidth);

  const label = layout.label || prettifyRoadName(road);
  const labelPos = layout.labelPos || layout.points[Math.floor(layout.points.length / 2)];
  ctx.fillStyle = '#f8fafc';
  ctx.font = '14px "Segoe UI", sans-serif';
  ctx.fillText(label, labelPos[0], labelPos[1]);
}

function strokePath(points, color, width) {
  if (!points || points.length < 2) {
    return;
  }
  ctx.beginPath();
  ctx.moveTo(points[0][0], points[0][1]);
  for (let i = 1; i < points.length; i += 1) {
    ctx.lineTo(points[i][0], points[i][1]);
  }
  ctx.strokeStyle = color;
  ctx.lineWidth = width;
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  ctx.stroke();
}

function drawVehicles(road, snapshot) {
  if (!snapshot) {
    return;
  }
  // Use cars_5s if available, otherwise fall back to cars_60s for stale data
  const vehicleCount = snapshot.cars_5s > 0 ? snapshot.cars_5s : snapshot.cars_60s;
  if (vehicleCount <= 0) {
    return;
  }
  const layout = ROAD_LAYOUT[road];
  if (!layout || layout.points.length < 2) {
    return;
  }
  const segments = layout.points.length - 1;
  // Scale vehicle count for visualization (use cars_60s scale for stale data)
  const displayCount = snapshot.cars_5s > 0 
    ? Math.max(1, Math.min(5, Math.round(snapshot.cars_5s / 5)))
    : Math.max(1, Math.min(5, Math.round(snapshot.cars_60s / 12))); // Scale 60s data differently
  for (let i = 0; i < displayCount; i += 1) {
    const segIdx = Math.floor(Math.random() * segments);
    const start = layout.points[segIdx];
    const end = layout.points[segIdx + 1];
    const t = Math.random();
    const x = start[0] + (end[0] - start[0]) * t;
    const y = start[1] + (end[1] - start[1]) * t;
    ctx.beginPath();
    ctx.fillStyle = '#38bdf8';
    ctx.arc(x, y, 5, 0, Math.PI * 2);
    ctx.fill();
  }
}

function updateTable() {
  const tbody = document.getElementById('summary');
  tbody.innerHTML = '';

  const ordered = ALL_ROADS.slice().sort((a, b) => {
    const classA = (latestSummary[a] && latestSummary[a].classification) || 'UNKNOWN';
    const classB = (latestSummary[b] && latestSummary[b].classification) || 'UNKNOWN';
    const rankA = CLASS_PRIORITY[classA] || CLASS_PRIORITY.UNKNOWN;
    const rankB = CLASS_PRIORITY[classB] || CLASS_PRIORITY.UNKNOWN;
    if (rankA !== rankB) {
      return rankA - rankB;
    }
    const flowA = latestSummary[a] ? latestSummary[a].cars_5s : 0;
    const flowB = latestSummary[b] ? latestSummary[b].cars_5s : 0;
    return flowB - flowA;
  });

  ordered.forEach((road) => {
    const snapshot = latestSummary[road];
    const row = document.createElement('tr');
    const classification = snapshot ? snapshot.classification : 'UNKNOWN';
    const color = STATUS_COLORS[classification] || STATUS_COLORS.UNKNOWN;
    const label = prettifyRoadName(road);
    row.innerHTML = `
      <td>${label}</td>
      <td>
        <span class="tag">
          <span class="swatch" style="background:${color}"></span>
          ${formatClassification(classification)}
        </span>
      </td>
      <td>${snapshot ? snapshot.cars_5s : '--'}</td>
      <td>${snapshot ? snapshot.cars_60s : '--'}</td>
      <td>${snapshot ? snapshot.avg_speed_kmh.toFixed(1) : '--'} km/h</td>
    `;
    tbody.appendChild(row);
  });
}

function formatClassification(classification) {
  switch (classification) {
    case 'CONGESTED':
      return 'Congested';
    case 'MODERATE':
      return 'Moderate';
    case 'SMOOTH':
      return 'Smooth';
    case 'STALE':
      return 'Stale (No Recent Data)';
    default:
      return 'No Data';
  }
}

function prettifyRoadName(name) {
  return name.replace(/_/g, ' ');
}

fetchSummary();
setInterval(fetchSummary, 1000);
