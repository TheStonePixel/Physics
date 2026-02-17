/**
 * Unit Conversion Utility
 *
 * Generic conversion functions for physics quantities.
 * All core physics uses SI (meters, kg, rad/s, etc).
 * This module converts to/from common display units.
 */

// --- Distance ---

export const M_PER_YARD = 0.9144;
export const M_PER_FOOT = 0.3048;
export const M_PER_MILE = 1609.344;
export const M_PER_KM = 1000;

export const mToYd = (m) => m / M_PER_YARD;
export const ydToM = (yd) => yd * M_PER_YARD;
export const mToFt = (m) => m / M_PER_FOOT;
export const ftToM = (ft) => ft * M_PER_FOOT;
export const mToMi = (m) => m / M_PER_MILE;
export const mToKm = (m) => m / M_PER_KM;

// --- Speed ---

export const MS_PER_MPH = 0.44704;
export const MS_PER_KPH = 1 / 3.6;
export const MS_PER_KNOT = 0.514444;

export const msToMph = (ms) => ms / MS_PER_MPH;
export const mphToMs = (mph) => mph * MS_PER_MPH;
export const msToKph = (ms) => ms * 3.6;
export const kphToMs = (kph) => kph / 3.6;
export const msToKnot = (ms) => ms / MS_PER_KNOT;

// --- Angular velocity ---

export const radToRpm = (rads) => (rads * 60) / (2 * Math.PI);
export const rpmToRad = (rpm) => (rpm * 2 * Math.PI) / 60;
export const radToDeg = (rad) => rad * (180 / Math.PI);
export const degToRad = (deg) => deg * (Math.PI / 180);
export const radToRev = (rad) => rad / (2 * Math.PI);
export const revToRad = (rev) => rev * 2 * Math.PI;

// --- Mass ---

export const kgToLb = (kg) => kg * 2.20462;
export const lbToKg = (lb) => lb / 2.20462;
export const kgToOz = (kg) => kg * 35.274;
export const ozToKg = (oz) => oz / 35.274;
export const kgToG = (kg) => kg * 1000;
export const gToKg = (g) => g / 1000;

// --- Temperature ---

export const cToF = (c) => c * 9 / 5 + 32;
export const fToC = (f) => (f - 32) * 5 / 9;

// --- Pressure (for air density adjustments) ---

export const paToAtm = (pa) => pa / 101325;
export const atmToPa = (atm) => atm * 101325;
export const paToPsi = (pa) => pa / 6894.757;
export const psiToPa = (psi) => psi * 6894.757;

// --- Formatting helpers ---

/**
 * Format a distance in the given unit system.
 * @param {number} meters - distance in meters
 * @param {'metric'|'imperial'} system
 * @param {number} [decimals=1]
 * @returns {{ value: number, unit: string, display: string }}
 */
export function formatDistance(meters, system = 'imperial', decimals = 1) {
  if (system === 'imperial') {
    const yd = mToYd(meters);
    return { value: +yd.toFixed(decimals), unit: 'yd', display: `${yd.toFixed(decimals)} yd` };
  }
  return { value: +meters.toFixed(decimals), unit: 'm', display: `${meters.toFixed(decimals)} m` };
}

/**
 * Format a speed in the given unit system.
 * @param {number} ms - speed in m/s
 * @param {'metric'|'imperial'} system
 * @param {number} [decimals=1]
 * @returns {{ value: number, unit: string, display: string }}
 */
export function formatSpeed(ms, system = 'imperial', decimals = 1) {
  if (system === 'imperial') {
    const mph = msToMph(ms);
    return { value: +mph.toFixed(decimals), unit: 'mph', display: `${mph.toFixed(decimals)} mph` };
  }
  return { value: +ms.toFixed(decimals), unit: 'm/s', display: `${ms.toFixed(decimals)} m/s` };
}

/**
 * Format a spin rate.
 * @param {number} rads - spin rate in rad/s
 * @param {'rpm'|'rad'} unit
 * @param {number} [decimals=0]
 * @returns {{ value: number, unit: string, display: string }}
 */
export function formatSpin(rads, unit = 'rpm', decimals = 0) {
  if (unit === 'rpm') {
    const rpm = radToRpm(rads);
    return { value: +rpm.toFixed(decimals), unit: 'RPM', display: `${rpm.toFixed(decimals)} RPM` };
  }
  return { value: +rads.toFixed(decimals), unit: 'rad/s', display: `${rads.toFixed(decimals)} rad/s` };
}
