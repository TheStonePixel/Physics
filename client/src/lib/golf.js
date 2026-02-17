/**
 * Golf Physics Utility
 *
 * Domain-specific layer on top of @thestonepixel/physics.
 * Provides golf ball constants, club presets, surface presets,
 * and convenience functions for simulating full golf shots.
 *
 * As the physics library grows, add new domain utilities alongside
 * this one (e.g. lib/billiards.js, lib/baseball.js).
 */

import { init } from '@thestonepixel/physics';

// --- Golf ball physical properties (USGA spec) ---

export const BALL = Object.freeze({
  mass: 0.04593,        // 45.93 g
  radius: 0.02135,      // 42.67 mm diameter
  crossArea: 0.00143,   // pi * radius^2
  dragCoeff: 0.25,      // dimpled sphere
  liftCoeff: 0.15,      // backspin lift
  airDensity: 1.225,    // sea level, 15°C
  spinDecay: 0.04,      // ~4% per second
  restitution: 0.6,
  friction: 0.4,
});

// --- Club presets (tour averages) ---

export const CLUBS = Object.freeze({
  driver:  { name: 'Driver',  speed: 71.5, launch: 10.5, spin: 280 },
  '3wood': { name: '3 Wood',  speed: 64.0, launch: 11.0, spin: 340 },
  '5iron': { name: '5 Iron',  speed: 56.0, launch: 14.0, spin: 460 },
  '7iron': { name: '7 Iron',  speed: 48.5, launch: 18.0, spin: 620 },
  '9iron': { name: '9 Iron',  speed: 41.0, launch: 24.0, spin: 780 },
  pw:      { name: 'PW',      speed: 37.0, launch: 28.0, spin: 880 },
  sw:      { name: 'SW',      speed: 31.0, launch: 34.0, spin: 980 },
});

export const CLUB_ORDER = ['driver', '3wood', '5iron', '7iron', '9iron', 'pw', 'sw'];

// --- Surface presets ---

export const SURFACES = Object.freeze({
  fairway:   { name: 'Fairway',    rollingFriction: 0.065, surfaceRestitution: 0.45, firmness: 0.65 },
  rough:     { name: 'Rough',      rollingFriction: 0.20,  surfaceRestitution: 0.30, firmness: 0.35 },
  green:     { name: 'Green',      rollingFriction: 0.045, surfaceRestitution: 0.35, firmness: 0.55 },
  fringe:    { name: 'Fringe',     rollingFriction: 0.09,  surfaceRestitution: 0.40, firmness: 0.55 },
  bunker:    { name: 'Bunker',     rollingFriction: 0.40,  surfaceRestitution: 0.15, firmness: 0.15 },
  cartPath:  { name: 'Cart Path',  rollingFriction: 0.03,  surfaceRestitution: 0.70, firmness: 0.95 },
});

export const SURFACE_ORDER = ['fairway', 'rough', 'green', 'fringe', 'bunker', 'cartPath'];

// --- Utility: degrees to radians ---

const DEG = Math.PI / 180;

// --- Core simulation functions ---

let _physics = null;

/**
 * Initialize the physics engine. Must be called once before any simulation.
 * Returns a promise that resolves when WASM is ready.
 */
export async function load() {
  if (_physics) return;
  _physics = await init();
}

/**
 * Compute launch velocity from speed, launch angle, and side angle.
 */
export function launchVelocity(speed, launchDeg, sideDeg = 0) {
  const la = launchDeg * DEG;
  const sa = sideDeg * DEG;
  return [
    speed * Math.cos(la) * Math.cos(sa),
    speed * Math.sin(la),
    speed * Math.cos(la) * Math.sin(sa),
  ];
}

/**
 * Simulate ball flight (aerodynamic trajectory).
 * @param {Object} opts
 * @param {number} opts.speed        - ball speed off the face (m/s)
 * @param {number} opts.launchAngle  - launch angle (degrees)
 * @param {number} [opts.sideAngle=0] - side angle (degrees, negative = draw)
 * @param {number} [opts.spinRate=0]  - backspin rate (rad/s)
 * @returns {{ points, count }}
 */
export function simulateFlight(opts) {
  const velocity = launchVelocity(opts.speed, opts.launchAngle, opts.sideAngle ?? 0);
  return _physics.simulate({
    velocity,
    spinRate: opts.spinRate ?? 0,
    spinAxis: [0, 0, 1],
    mass: BALL.mass,
    radius: BALL.radius,
    dragCoeff: BALL.dragCoeff,
    liftCoeff: BALL.liftCoeff,
    crossArea: BALL.crossArea,
    airDensity: BALL.airDensity,
    spinDecay: BALL.spinDecay,
    restitution: BALL.restitution,
    friction: BALL.friction,
  });
}

/**
 * Simulate ball rolling after landing.
 * @param {Object} landing - { position: [x,y,z], velocity: [x,y,z] } from flight
 * @param {number} spinRate - remaining spin at landing (rad/s)
 * @param {string} surfaceKey - key from SURFACES (e.g. 'fairway', 'rough')
 * @returns {{ points, count }}
 */
export function simulateRoll(landing, spinRate, surfaceKey = 'fairway') {
  const surface = SURFACES[surfaceKey] || SURFACES.fairway;
  return _physics.simulateRoll({
    position: landing.position,
    velocity: landing.velocity,
    spinRate,
    spinAxis: [0, 0, 1],
    radius: BALL.radius,
    mass: BALL.mass,
    ...surface,
  });
}

/**
 * Simulate a full golf shot: flight + roll.
 * @param {Object} opts
 * @param {string} [opts.club='driver'] - club key from CLUBS
 * @param {number} [opts.speed]         - override ball speed (m/s)
 * @param {number} [opts.launchAngle]   - override launch angle (degrees)
 * @param {number} [opts.spinRate]      - override spin rate (rad/s)
 * @param {number} [opts.sideAngle=0]   - side angle (degrees)
 * @param {string} [opts.surface='fairway'] - landing surface key
 * @returns {{ flight, roll, stats }}
 */
export function simulateShot(opts = {}) {
  const club = CLUBS[opts.club ?? 'driver'];
  const speed = opts.speed ?? club.speed;
  const launchAngle = opts.launchAngle ?? club.launch;
  const spinRate = opts.spinRate ?? club.spin;
  const sideAngle = opts.sideAngle ?? 0;
  const surfaceKey = opts.surface ?? 'fairway';

  // Flight phase
  const flight = simulateFlight({ speed, launchAngle, sideAngle, spinRate });
  const flightPts = flight.points;
  const landing = flightPts[flightPts.length - 1];

  // Estimate spin at landing (decays during flight)
  const flightTime = landing.t;
  const spinAtLanding = spinRate * Math.pow(1 - BALL.spinDecay, flightTime);

  // Roll phase
  const landingState = {
    position: [landing.x, landing.y, landing.z],
    velocity: estimateLandingVelocity(flightPts),
  };
  const roll = simulateRoll(landingState, spinAtLanding, surfaceKey);
  const rollPts = roll.points;
  const restingPoint = rollPts[rollPts.length - 1];

  // Compute stats
  const carry = Math.sqrt(landing.x ** 2 + landing.z ** 2);
  const rollDist = Math.sqrt(
    (restingPoint.x - landing.x) ** 2 +
    (restingPoint.z - landing.z) ** 2
  );
  const total = Math.sqrt(restingPoint.x ** 2 + restingPoint.z ** 2);
  const apex = Math.max(...flightPts.map(p => p.y));
  const lateral = restingPoint.z;

  return {
    flight,
    roll,
    stats: {
      carry: +carry.toFixed(1),
      rollDist: +rollDist.toFixed(1),
      total: +total.toFixed(1),
      apex: +apex.toFixed(1),
      flightTime: +flightTime.toFixed(2),
      lateral: +lateral.toFixed(1),
    },
  };
}

/**
 * Simulate every club in the bag on a given surface.
 * @param {string} [surfaceKey='fairway']
 * @returns {Array<{ club, name, stats, flight, roll }>}
 */
export function simulateFullBag(surfaceKey = 'fairway') {
  return CLUB_ORDER.map(key => {
    const result = simulateShot({ club: key, surface: surfaceKey });
    return {
      club: key,
      name: CLUBS[key].name,
      ...result,
    };
  });
}

// --- Internal helpers ---

function estimateLandingVelocity(pts) {
  if (pts.length < 2) return [0, 0, 0];
  const a = pts[pts.length - 2];
  const b = pts[pts.length - 1];
  const dt = b.t - a.t;
  if (dt < 1e-6) return [0, 0, 0];
  return [
    (b.x - a.x) / dt,
    (b.y - a.y) / dt,
    (b.z - a.z) / dt,
  ];
}
