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
   * Simulate a golf shot with full aerodynamics (drag + Magnus effect).
   * @param {Object} opts
   * @param {number[]} opts.position    - [x, y, z] starting position (meters)
   * @param {number[]} opts.velocity    - [x, y, z] launch velocity (m/s)
   * @param {number[]} opts.spinAxis    - [x, y, z] spin axis (normalized)
   * @param {number}   opts.spinRate    - spin rate (rad/s), ~300 for driver
   * @param {number}   [opts.groundY=0] - ground plane height
   * @param {number}   [opts.dt=0.005]  - time step (seconds)
   * @returns {{ points: {x,y,z,t}[], count: number }}
   */
  golfSimulate(opts) {
    const [px, py, pz] = opts.position || [0, 0, 0];
    const [vx, vy, vz] = opts.velocity;
    const [sx, sy, sz] = opts.spinAxis || [0, 0, 1];
    const spinRate = opts.spinRate || 300;
    const groundY = opts.groundY ?? 0;
    const dt = opts.dt || 0.005;

    const count = this._call('golf_simulate', 'number',
      ['number','number','number',
       'number','number','number',
       'number','number','number',
       'number','number','number'],
      [px, py, pz, vx, vy, vz, sx, sy, sz, spinRate, groundY, dt]);

    const points = new Array(count);
    for (let i = 0; i < count; i++) {
      points[i] = {
        x: this._call('golf_get_x', 'number', ['number'], [i]),
        y: this._call('golf_get_y', 'number', ['number'], [i]),
        z: this._call('golf_get_z', 'number', ['number'], [i]),
        t: this._call('golf_get_time', 'number', ['number'], [i]),
      };
    }
    return { points, count };
  }
}

export default init;
export { init, PhysicsEngine };
