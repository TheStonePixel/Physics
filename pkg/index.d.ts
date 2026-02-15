export interface TrajectoryPoint {
  x: number;
  y: number;
  z: number;
  t: number;
}

export interface GolfSimulateOptions {
  /** Starting position [x, y, z] in meters */
  position?: [number, number, number];
  /** Launch velocity [x, y, z] in m/s */
  velocity: [number, number, number];
  /** Spin axis [x, y, z], normalized. Default: [0, 0, 1] (backspin) */
  spinAxis?: [number, number, number];
  /** Spin rate in rad/s. Default: 300 (~2865 RPM) */
  spinRate?: number;
  /** Ground plane Y height. Default: 0 */
  groundY?: number;
  /** Simulation time step in seconds. Default: 0.005 */
  dt?: number;
}

export interface GolfSimulateResult {
  points: TrajectoryPoint[];
  count: number;
}

export declare class PhysicsEngine {
  golfSimulate(opts: GolfSimulateOptions): GolfSimulateResult;
}

export declare function init(): Promise<PhysicsEngine>;
