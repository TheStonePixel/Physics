import PhysicsModule from './physics_wasm.js';

let _instance = null;

/**
 * Initialize the physics WASM module. Call once, await the result.
 * Subsequent calls return the cached instance.
 * @returns {Promise<PhysicsEngine>}
 */
async function init() {
  if (_instance) return _instance;
  const wasm = await PhysicsModule();
  _instance = new PhysicsEngine(wasm);
  return _instance;
}

class PhysicsEngine {
  constructor(wasm) {
    this._wasm = wasm;
    this._call = wasm.ccall.bind(wasm);
  }

  /**
   * Simulate a spinning body trajectory with full aerodynamics (drag + Magnus).
   * @param {Object} opts
   * @param {number[]} opts.velocity       - [x, y, z] launch velocity (m/s)
   * @param {number}   opts.mass           - body mass (kg)
   * @param {number}   opts.radius         - body radius (m), for Magnus effect
   * @param {number}   opts.dragCoeff      - drag coefficient (Cd)
   * @param {number}   opts.crossArea      - cross-sectional area (m^2)
   * @param {number[]} [opts.position]     - [x, y, z] starting position (meters)
   * @param {number[]} [opts.spinAxis]     - [x, y, z] spin axis (normalized)
   * @param {number}   [opts.spinRate=0]   - spin rate (rad/s)
   * @param {number}   [opts.liftCoeff=0.15] - lift coefficient (Cl)
   * @param {number}   [opts.airDensity=1.225] - air density (kg/m^3)
   * @param {number}   [opts.spinDecay=0]  - spin decay rate per second
   * @param {number}   [opts.restitution=0.5] - coefficient of restitution
   * @param {number}   [opts.friction=0.3] - friction coefficient
   * @param {number}   [opts.groundY=0]    - ground plane height
   * @param {number}   [opts.dt=0.005]     - time step (seconds)
   * @returns {{ points: {x,y,z,t}[], count: number }}
   */
  simulate(opts) {
    const [px, py, pz] = opts.position || [0, 0, 0];
    const [vx, vy, vz] = opts.velocity;
    const [sx, sy, sz] = opts.spinAxis || [0, 0, 1];
    const spinRate = opts.spinRate || 0;
    const mass = opts.mass;
    const restitution = opts.restitution ?? 0.5;
    const friction = opts.friction ?? 0.3;
    const dragCoeff = opts.dragCoeff;
    const liftCoeff = opts.liftCoeff ?? 0.15;
    const crossArea = opts.crossArea;
    const radius = opts.radius;
    const airDensity = opts.airDensity ?? 1.225;
    const spinDecay = opts.spinDecay ?? 0;
    const groundY = opts.groundY ?? 0;
    const dt = opts.dt || 0.005;

    const argTypes = new Array(21).fill('number');
    const count = this._call('aero_simulate', 'number', argTypes,
      [px, py, pz, vx, vy, vz, sx, sy, sz, spinRate,
       mass, restitution, friction,
       dragCoeff, liftCoeff, crossArea, radius, airDensity, spinDecay,
       groundY, dt]);

    const points = new Array(count);
    for (let i = 0; i < count; i++) {
      points[i] = {
        x: this._call('aero_get_x', 'number', ['number'], [i]),
        y: this._call('aero_get_y', 'number', ['number'], [i]),
        z: this._call('aero_get_z', 'number', ['number'], [i]),
        t: this._call('aero_get_time', 'number', ['number'], [i]),
      };
    }
    return { points, count };
  }

  /**
   * Simulate ground interaction: bouncing + rolling on a surface.
   * Use the landing position/velocity from simulate() as input.
   * @param {Object} opts
   * @param {number[]} opts.velocity         - [x, y, z] landing velocity (m/s)
   * @param {number}   opts.radius           - body radius (m)
   * @param {number}   opts.mass             - body mass (kg)
   * @param {number}   opts.rollingFriction  - rolling resistance coefficient
   * @param {number}   opts.surfaceRestitution - bounce energy retained (0-1)
   * @param {number}   opts.firmness         - surface firmness (0=soft, 1=hard)
   * @param {number[]} [opts.position]       - [x, y, z] landing position (meters)
   * @param {number[]} [opts.spinAxis]       - [x, y, z] spin axis (normalized)
   * @param {number}   [opts.spinRate=0]     - spin rate (rad/s)
   * @param {number[]} [opts.surfaceNormal]  - [x, y, z] surface normal (for slopes)
   * @param {number}   [opts.dt=0.005]       - time step (seconds)
   * @returns {{ points: {x,y,z,spin,t}[], count: number }}
   */
  simulateRoll(opts) {
    const [px, py, pz] = opts.position || [0, 0, 0];
    const [vx, vy, vz] = opts.velocity;
    const [sx, sy, sz] = opts.spinAxis || [0, 0, 1];
    const spinRate = opts.spinRate || 0;
    const radius = opts.radius;
    const mass = opts.mass;
    const rollingFriction = opts.rollingFriction;
    const surfaceRestitution = opts.surfaceRestitution;
    const firmness = opts.firmness;
    const [nx, ny, nz] = opts.surfaceNormal || [0, 1, 0];
    const dt = opts.dt || 0.005;

    const argTypes = new Array(19).fill('number');
    const count = this._call('rolling_simulate', 'number', argTypes,
      [px, py, pz, vx, vy, vz, sx, sy, sz, spinRate,
       radius, mass, rollingFriction, surfaceRestitution, firmness,
       nx, ny, nz, dt]);

    const points = new Array(count);
    for (let i = 0; i < count; i++) {
      points[i] = {
        x: this._call('rolling_get_x', 'number', ['number'], [i]),
        y: this._call('rolling_get_y', 'number', ['number'], [i]),
        z: this._call('rolling_get_z', 'number', ['number'], [i]),
        spin: this._call('rolling_get_spin', 'number', ['number'], [i]),
        t: this._call('rolling_get_time', 'number', ['number'], [i]),
      };
    }
    return { points, count };
  }
}

export default init;
export { init, PhysicsEngine };
