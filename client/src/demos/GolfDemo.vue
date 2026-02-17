<script setup>
import { ref, computed, watch, onMounted } from 'vue';
import {
  CLUBS, CLUB_ORDER, SURFACES, SURFACE_ORDER,
  simulateShot, simulateFullBag,
} from '../lib/golf.js';
import { formatDistance, formatSpeed, formatSpin } from '../lib/units.js';

// --- Inputs ---
const clubKey = ref('driver');
const surfaceKey = ref('fairway');
const speed = ref(CLUBS.driver.speed);
const launchAngle = ref(CLUBS.driver.launch);
const spinRate = ref(CLUBS.driver.spin);
const sideAngle = ref(0);

// --- Units ---
const unitSystem = ref('imperial'); // 'imperial' | 'metric'
const isImperial = computed(() => unitSystem.value === 'imperial');

const dist = (m) => formatDistance(m, unitSystem.value);
const spd = (ms) => formatSpeed(ms, unitSystem.value);
const spin = (rads) => formatSpin(rads);

// --- Results ---
const result = ref(null);
const bagResults = ref(null);
const canvas = ref(null);

// Sync club preset when club changes
watch(clubKey, (key) => {
  const club = CLUBS[key];
  speed.value = club.speed;
  launchAngle.value = club.launch;
  spinRate.value = club.spin;
  sideAngle.value = 0;
});

function simulate() {
  result.value = simulateShot({
    club: clubKey.value,
    speed: speed.value,
    launchAngle: launchAngle.value,
    spinRate: spinRate.value,
    sideAngle: sideAngle.value,
    surface: surfaceKey.value,
  });
  draw();
}

function runFullBag() {
  bagResults.value = simulateFullBag(surfaceKey.value);
}

// --- Canvas drawing ---

function draw() {
  if (!result.value || !canvas.value) return;
  const ctx = canvas.value.getContext('2d');
  const W = canvas.value.width;
  const H = canvas.value.height;
  const pad = 50;

  const flightPts = result.value.flight.points;
  const rollPts = result.value.roll.points;

  // Compute bounds
  const allX = [...flightPts.map(p => p.x), ...rollPts.map(p => p.x)];
  const allY = flightPts.map(p => p.y);
  const maxX = Math.max(...allX, 1);
  const maxY = Math.max(...allY, 1);

  const scaleX = (W - pad * 2) / maxX;
  const scaleY = (H - pad * 2) / maxY;
  const scale = Math.min(scaleX, scaleY);

  const sx = (x) => pad + x * scale;
  const sy = (y) => H - pad - y * scale;

  // Background
  ctx.fillStyle = '#0a1628';
  ctx.fillRect(0, 0, W, H);

  // Grid
  ctx.strokeStyle = '#1a2a44';
  ctx.lineWidth = 0.5;
  const gridX = Math.pow(10, Math.floor(Math.log10(maxX / 4)));
  const gridY = Math.pow(10, Math.floor(Math.log10(maxY / 3)));
  for (let d = 0; d <= maxX; d += gridX) {
    ctx.beginPath(); ctx.moveTo(sx(d), pad); ctx.lineTo(sx(d), H - pad); ctx.stroke();
  }
  for (let h = 0; h <= maxY; h += gridY) {
    ctx.beginPath(); ctx.moveTo(pad, sy(h)); ctx.lineTo(W - pad, sy(h)); ctx.stroke();
  }

  // Axis labels
  ctx.fillStyle = '#3a5a7a';
  ctx.font = '10px monospace';
  ctx.textAlign = 'center';
  for (let d = 0; d <= maxX; d += gridX) {
    ctx.fillText(dist(d).display, sx(d), H - pad + 14);
  }
  ctx.textAlign = 'right';
  for (let h = gridY; h <= maxY; h += gridY) {
    ctx.fillText(dist(h).display, pad - 6, sy(h) + 3);
  }

  // Ground line
  ctx.strokeStyle = '#2d5a27';
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(pad, sy(0));
  ctx.lineTo(W - pad, sy(0));
  ctx.stroke();

  // Flight trajectory
  ctx.shadowColor = '#4af';
  ctx.shadowBlur = 6;
  ctx.strokeStyle = '#4af';
  ctx.lineWidth = 2;
  ctx.beginPath();
  flightPts.forEach((p, i) => {
    i === 0 ? ctx.moveTo(sx(p.x), sy(p.y)) : ctx.lineTo(sx(p.x), sy(p.y));
  });
  ctx.stroke();
  ctx.shadowBlur = 0;

  // Roll trajectory
  if (rollPts.length > 1) {
    ctx.strokeStyle = '#4f4';
    ctx.lineWidth = 2;
    ctx.setLineDash([4, 3]);
    ctx.beginPath();
    rollPts.forEach((p, i) => {
      i === 0 ? ctx.moveTo(sx(p.x), sy(0)) : ctx.lineTo(sx(p.x), sy(0));
    });
    ctx.stroke();
    ctx.setLineDash([]);
  }

  // Landing marker
  const land = flightPts[flightPts.length - 1];
  ctx.fillStyle = '#fa4';
  ctx.beginPath();
  ctx.arc(sx(land.x), sy(0), 4, 0, Math.PI * 2);
  ctx.fill();

  // Resting marker
  const rest = rollPts[rollPts.length - 1];
  ctx.fillStyle = '#f44';
  ctx.beginPath();
  ctx.arc(sx(rest.x), sy(0), 5, 0, Math.PI * 2);
  ctx.fill();

  // Labels
  ctx.font = '11px monospace';
  ctx.textAlign = 'center';
  ctx.fillStyle = '#fa4';
  ctx.fillText(`Carry ${dist(result.value.stats.carry).display}`, sx(land.x), sy(0) + 26);
  ctx.fillStyle = '#f44';
  ctx.fillText(`Total ${dist(result.value.stats.total).display}`, sx(rest.x), sy(0) + 40);
}

watch([speed, launchAngle, spinRate, sideAngle, surfaceKey], simulate);
watch(unitSystem, draw);
onMounted(() => {
  simulate();
  runFullBag();
});
watch(surfaceKey, runFullBag);
</script>

<template>
  <div class="golf-demo">
    <div class="demo-header">
      <h2>Golf Shot Simulator</h2>
      <div class="unit-toggle">
        <button :class="{ active: isImperial }" @click="unitSystem = 'imperial'">Imperial</button>
        <button :class="{ active: !isImperial }" @click="unitSystem = 'metric'">Metric</button>
      </div>
    </div>

    <canvas ref="canvas" width="920" height="360" />

    <div class="controls">
      <div class="control-group">
        <label>
          Club
          <select v-model="clubKey">
            <option v-for="key in CLUB_ORDER" :key="key" :value="key">
              {{ CLUBS[key].name }}
            </option>
          </select>
        </label>
        <label>
          Landing Surface
          <select v-model="surfaceKey">
            <option v-for="key in SURFACE_ORDER" :key="key" :value="key">
              {{ SURFACES[key].name }}
            </option>
          </select>
        </label>
      </div>

      <div class="control-group">
        <label>
          Ball Speed: <strong>{{ spd(speed).display }}</strong>
          <input type="range" v-model.number="speed" min="15" max="90" step="0.5" />
        </label>
        <label>
          Launch Angle: <strong>{{ launchAngle }}°</strong>
          <input type="range" v-model.number="launchAngle" min="2" max="50" step="0.5" />
        </label>
      </div>

      <div class="control-group">
        <label>
          Spin Rate: <strong>{{ spin(spinRate).display }}</strong>
          <input type="range" v-model.number="spinRate" min="0" max="1200" step="10" />
        </label>
        <label>
          Side Angle: <strong>{{ sideAngle }}°</strong>
          <input type="range" v-model.number="sideAngle" min="-15" max="15" step="0.5" />
        </label>
      </div>
    </div>

    <!-- Stats -->
    <div v-if="result" class="stats">
      <div class="stat">
        <span class="val">{{ dist(result.stats.carry).display }}</span>
        <span class="lbl">Carry</span>
      </div>
      <div class="stat">
        <span class="val roll">+{{ dist(result.stats.rollDist).display }}</span>
        <span class="lbl">Roll</span>
      </div>
      <div class="stat">
        <span class="val total">{{ dist(result.stats.total).display }}</span>
        <span class="lbl">Total</span>
      </div>
      <div class="stat">
        <span class="val">{{ dist(result.stats.apex).display }}</span>
        <span class="lbl">Apex</span>
      </div>
      <div class="stat">
        <span class="val">{{ result.stats.flightTime }}s</span>
        <span class="lbl">Flight</span>
      </div>
      <div class="stat">
        <span class="val">{{ dist(result.stats.lateral).display }}</span>
        <span class="lbl">Lateral</span>
      </div>
    </div>

    <!-- Full bag table -->
    <div v-if="bagResults" class="bag-section">
      <h3>Full Bag — {{ SURFACES[surfaceKey].name }}</h3>
      <table>
        <thead>
          <tr>
            <th>Club</th>
            <th>Speed</th>
            <th>Launch</th>
            <th>Spin</th>
            <th>Carry</th>
            <th>Roll</th>
            <th>Total</th>
            <th>Apex</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="row in bagResults" :key="row.club"
              :class="{ highlight: row.club === clubKey }">
            <td class="club-name">{{ row.name }}</td>
            <td>{{ spd(CLUBS[row.club].speed).display }}</td>
            <td>{{ CLUBS[row.club].launch }}°</td>
            <td>{{ spin(CLUBS[row.club].spin).display }}</td>
            <td>{{ dist(row.stats.carry).display }}</td>
            <td class="roll-col">+{{ dist(row.stats.rollDist).display }}</td>
            <td class="total-col">{{ dist(row.stats.total).display }}</td>
            <td>{{ dist(row.stats.apex).display }}</td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<style scoped>
.demo-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 12px;
}

.golf-demo h2 {
  font-size: 1.1rem;
  color: #ccc;
  margin: 0;
}

.unit-toggle {
  display: flex;
  gap: 0;
  border-radius: 4px;
  overflow: hidden;
  border: 1px solid #2a3a54;
}

.unit-toggle button {
  background: #1a2a44;
  color: #8a9ab5;
  border: none;
  padding: 4px 12px;
  font-size: 0.75rem;
  cursor: pointer;
  transition: background 0.15s, color 0.15s;
}

.unit-toggle button.active {
  background: #2a4a6a;
  color: #e0e0e0;
}

canvas {
  width: 100%;
  border-radius: 8px;
  border: 1px solid #1a2a44;
  display: block;
}

.controls {
  display: flex;
  flex-direction: column;
  gap: 12px;
  margin-top: 16px;
}

.control-group {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 16px;
}

label {
  display: flex;
  flex-direction: column;
  gap: 4px;
  font-size: 0.82rem;
  color: #8a9ab5;
}

label strong { color: #e0e0e0; }

select {
  background: #1a2a44;
  color: #e0e0e0;
  border: 1px solid #2a3a54;
  border-radius: 4px;
  padding: 6px 8px;
  font-size: 0.82rem;
}

input[type='range'] {
  accent-color: #4af;
  width: 100%;
}

.stats {
  display: flex;
  gap: 24px;
  margin-top: 16px;
  padding: 16px;
  background: #111922;
  border-radius: 8px;
  justify-content: center;
  flex-wrap: wrap;
}

.stat {
  display: flex;
  flex-direction: column;
  align-items: center;
  min-width: 70px;
}

.val {
  font-size: 1.4rem;
  font-weight: 700;
  color: #4af;
}

.val.roll { color: #4f4; }
.val.total { color: #f44; }

.lbl {
  font-size: 0.7rem;
  text-transform: uppercase;
  letter-spacing: 0.1em;
  color: #5a7a9a;
  margin-top: 2px;
}

.bag-section {
  margin-top: 28px;
}

.bag-section h3 {
  font-size: 0.95rem;
  color: #8a9ab5;
  margin-bottom: 10px;
}

table {
  width: 100%;
  border-collapse: collapse;
  font-size: 0.8rem;
  font-family: monospace;
}

th {
  text-align: left;
  padding: 6px 8px;
  color: #5a7a9a;
  font-weight: 500;
  border-bottom: 1px solid #1a2a44;
}

td {
  padding: 5px 8px;
  color: #b0b8c8;
}

tr.highlight td {
  background: #1a2a44;
  color: #e0e0e0;
}

.club-name { color: #e0e0e0; }
.roll-col { color: #4f4; }
.total-col { color: #f44; font-weight: 600; }
</style>
