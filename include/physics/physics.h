#ifndef PHYSICS_H
#define PHYSICS_H

/*
 * Physics Library
 * A pure-computation physics library for game development, simulations, and
 * scientific use. No GUI, no dependencies beyond the C standard library.
 * Compiles to native code and WebAssembly (via Emscripten).
 *
 * All public symbols are prefixed with phys_.
 * Matrices are column-major for OpenGL/WebGL compatibility.
 * Coordinate system is right-handed, Y-up.
 */

#include "types.h"
#include "vec.h"
#include "mat.h"
#include "kinematics.h"
#include "dynamics.h"
#include "aerodynamics.h"
#include "collision.h"
#include "surface.h"

#endif /* PHYSICS_H */
