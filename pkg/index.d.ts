export interface TrajectoryPoint {
  x: number;
  y: number;
  z: number;
  t: number;
}

export interface SimulateOptions {
  /** Launch velocity [x, y, z] in m/s */
  velocity: [number, number, number];
  /** Body mass in kg */
  mass: number;
  /** Body radius in meters (used for Magnus effect) */
  radius: number;
  /** Drag coefficient (Cd) */
  dragCoeff: number;
  /** Cross-sectional area in m^2 */
  crossArea: number;
  /** Starting position [x, y, z] in meters. Default: [0, 0, 0] */
  position?: [number, number, number];
  /** Spin axis [x, y, z], normalized. Default: [0, 0, 1] */
  spinAxis?: [number, number, number];
  /** Spin rate in rad/s. Default: 0 */
  spinRate?: number;
  /** Lift coefficient (Cl). Default: 0.15 */
  liftCoeff?: number;
  /** Air density in kg/m^3. Default: 1.225 (sea level) */
  airDensity?: number;
  /** Spin decay rate per second. Default: 0 */
  spinDecay?: number;
  /** Coefficient of restitution. Default: 0.5 */
  restitution?: number;
  /** Friction coefficient. Default: 0.3 */
  friction?: number;
  /** Ground plane Y height. Default: 0 */
  groundY?: number;
  /** Simulation time step in seconds. Default: 0.005 */
  dt?: number;
}

export interface SimulateResult {
  points: TrajectoryPoint[];
  count: number;
}

export interface RollingPoint {
  x: number;
  y: number;
  z: number;
  spin: number;
  t: number;
}

export interface RollOptions {
  /** Landing velocity [x, y, z] in m/s */
  velocity: [number, number, number];
  /** Body radius in meters */
  radius: number;
  /** Body mass in kg */
  mass: number;
  /** Rolling resistance coefficient */
  rollingFriction: number;
  /** Bounce energy retained (0=dead, 1=perfect) */
  surfaceRestitution: number;
  /** Surface firmness (0=soft/absorptive, 1=hard/reflective) */
  firmness: number;
  /** Landing position [x, y, z] in meters. Default: [0, 0, 0] */
  position?: [number, number, number];
  /** Spin axis [x, y, z], normalized. Default: [0, 0, 1] */
  spinAxis?: [number, number, number];
  /** Spin rate in rad/s. Default: 0 */
  spinRate?: number;
  /** Surface normal [x, y, z] for slopes. Default: [0, 1, 0] (flat) */
  surfaceNormal?: [number, number, number];
  /** Simulation time step in seconds. Default: 0.005 */
  dt?: number;
}

export interface RollResult {
  points: RollingPoint[];
  count: number;
}

export declare class PhysicsEngine {
  simulate(opts: SimulateOptions): SimulateResult;
  simulateRoll(opts: RollOptions): RollResult;
}

export declare function init(): Promise<PhysicsEngine>;
export default init;
