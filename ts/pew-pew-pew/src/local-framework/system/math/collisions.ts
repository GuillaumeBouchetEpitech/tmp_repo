
import * as glm from 'gl-matrix';

export const intersectSegment = (A: glm.ReadonlyVec2, B: glm.ReadonlyVec2, I: glm.ReadonlyVec2, P: glm.ReadonlyVec2): number =>  {

  const D = glm.vec2.fromValues(0,0);
  const E = glm.vec2.fromValues(0,0);
  D[0] = B[0] - A[0];
  D[1] = B[1] - A[1];
  E[0] = P[0] - I[0];
  E[1] = P[1] - I[1];
  const denom = D[0]*E[1] - D[1]*E[0];
  if (denom==0) {
    return -1;   // erreur, cas limite
  }
  const t = - (A[0]*E[1]-I[0]*E[1]-E[0]*A[1]+E[0]*I[1]) / denom;
  if (t < 0 || t >= 1) {
    return 0;
  }
  const u = - (-D[0]*A[1]+D[0]*I[1]+D[1]*A[0]-D[1]*I[0]) / denom;
  if (u < 0 || u >= 1) {
    return 0;
  }
  return 1;
}

export const collisionLinesStrip = (tab: ReadonlyArray<glm.ReadonlyVec2>, P: glm.vec2): boolean => {

  const I = glm.vec2.fromValues(0, 0);
  I[0] = 10000 + Math.random() * 100;   // 10000 + un nombre aléatoire entre 0 et 99
  I[1] = 10000 + Math.random() * 100;
  let nbIntersections = 0;
  for(let ii = 0; ii < tab.length; ++ii)
  {
    let jj = (ii + 1) % tab.length;

    const result = intersectSegment(tab[ii],tab[jj],I,P);
    if (result == -1) {
      return collisionLinesStrip(tab,P);  // cas limite, on relance la fonction.
    }
    nbIntersections += result;
  }
  if ((nbIntersections % 2) == 1) {  // nbIntersections est-il impair ?
    return true;
  }
  return false;
}


// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
export const intersectRaySphere = (
  p: glm.ReadonlyVec3,
  d: glm.ReadonlyVec3,
  sc: glm.ReadonlyVec3,
  sr: number
): number => {
  // Vector m = p - sc;
  const m = glm.vec3.sub(glm.vec3.create(), p, sc);
  const b = glm.vec3.dot(m, d)
  const c = glm.vec3.dot(m, m) - sr * sr;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
  if (c > 0 && b > 0) {
    return -1;
  }
  const discr = b * b - c;

  // A negative discriminant corresponds to ray missing sphere
  if (discr < 0.0) {
    return -1;
  }

  // Ray now found to intersect sphere, compute smallest t value of intersection
  let outT = -b - Math.sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero
  if (outT < 0.0) {
    outT = 0.0;
  }
  // q = p + outT * d;

  return outT;
}

// export const Raycast01 = (rayp: glm.ReadonlyVec3, rayn: glm.ReadonlyVec3, spherep: glm.ReadonlyVec3, radius: number): number => {
//   const p0 = rayp;
//   const d = rayn;
//   const c = spherep;
//   const r = radius;

//   const e = glm.vec3.sub(glm.vec3.create(), c, p0);
//   // Using Length here would cause floating point error to creep in
//   const Esq = glm.vec3.squaredLength(e);
//   const a = glm.vec3.dot(e, d);
//   const b = Math.sqrt(Esq - (a * a));
//   const f = Math.sqrt((r * r) - (b * b));

//   // No collision
//   if (r * r - Esq + a * a < 0) {
//     return -1; // -1 is invalid.
//   }
//   // Ray is inside
//   if (Esq < r * r) {
//     return a + f; // Just reverse direction
//   }
//   // else Normal intersection
//   return a - f;
// }

// export const RaySphereIntersect = (p: glm.ReadonlyVec3, d: glm.ReadonlyVec3, r: number): ([number, number] | undefined) => {
//   // let det: number, b: number;
//   const b = -glm.vec3.dot(p, d);
//   let det = b * b - glm.vec3.dot(p, p) + r * r;
//   if (det < 0) {
//     return undefined;
//   }
//   det = Math.sqrt(det);
//   let i1 = b - det;
//   let i2 = b + det;
//   // intersecting with ray?
//   if (i2 < 0) {
//     return undefined;
//   }
//   if (i1 < 0) {
//     i1 = 0;
//   }
//   return [i1, i2];
// };



export const intersectTriangle = (
  ray_origin: glm.ReadonlyVec3,
  ray_direction: glm.ReadonlyVec3,
  v0: glm.ReadonlyVec3,
  v1: glm.ReadonlyVec3,
  v2: glm.ReadonlyVec3,
  // out float outDistance,
  // out vec3 normal
): number => {
  // float nearValue = 0.001; // TODO: hardcoded
  // float farValue = 100.0; // TODO: hardcoded

  const v1v0 = glm.vec3.subtract(glm.vec3.create(), v1, v0);
  const v2v0 = glm.vec3.subtract(glm.vec3.create(), v2, v0);
  const rov0 = glm.vec3.subtract(glm.vec3.create(), ray_origin, v0);

  const n = glm.vec3.cross(glm.vec3.create(), v1v0, v2v0);

  const nDotRayDirection = glm.vec3.dot(ray_direction, n);
  if (Math.abs(nDotRayDirection) < 0.0001) { // Almost 0
    return -1; // They are parallel, so they don't intersect!
  }

  const q = glm.vec3.cross(glm.vec3.create(), rov0, ray_direction);
  const d = 1.0 / nDotRayDirection;
  const u = d * glm.vec3.dot(glm.vec3.negate(glm.vec3.create(), q), v2v0);
  const v = d * glm.vec3.dot(q, v1v0);
  const t = d * glm.vec3.dot(glm.vec3.negate(glm.vec3.create(), n), rov0);

  // console.log(glm.vec3.dot(ray_direction, n));
  // console.log(ray_direction);
  // console.log(n);
  // console.log({d});
  // console.log({t});

  // if (u < 0.0 || v < 0.0 || (u + v) > 1.0 || t < nearValue || t > farValue) {
  //   return false;
  // }
  if (u < 0.0 || v < 0.0 || (u + v) > 1.0 || t < 0 || t > 1) {
    return -1;
  }

  // const length = glm.vec3.length(normal);

  // normal = normalize(-n);
  // outDistance = t;
  return t;
};



export interface IIntersectTriangle_raw_res {
  distance: number,
  normal: glm.vec3
};

export const intersectTriangle_raw = (
  ray_origin: glm.ReadonlyVec3,
  ray_direction: glm.ReadonlyVec3,
  v0: glm.ReadonlyVec3,
  v1: glm.ReadonlyVec3,
  v2: glm.ReadonlyVec3,
  outData: IIntersectTriangle_raw_res,
): number => {
  // float nearValue = 0.001; // TODO: hardcoded
  // float farValue = 100.0; // TODO: hardcoded

  const v1v0 = glm.vec3.subtract(glm.vec3.create(), v1, v0);
  const v2v0 = glm.vec3.subtract(glm.vec3.create(), v2, v0);
  const rov0 = glm.vec3.subtract(glm.vec3.create(), ray_origin, v0);

  const n = glm.vec3.cross(glm.vec3.create(), v1v0, v2v0);

  const nDotRayDirection = glm.vec3.dot(ray_direction, n);
  if (Math.abs(nDotRayDirection) < 0.0001) { // Almost 0
    return -1; // They are parallel, so they don't intersect!
  }

  const q = glm.vec3.cross(glm.vec3.create(), rov0, ray_direction);
  const d = 1.0 / nDotRayDirection;
  const u = d * glm.vec3.dot(glm.vec3.negate(glm.vec3.create(), q), v2v0);
  const v = d * glm.vec3.dot(q, v1v0);
  const t = d * glm.vec3.dot(glm.vec3.negate(glm.vec3.create(), n), rov0);

  // console.log(glm.vec3.dot(ray_direction, n));
  // console.log(ray_direction);
  // console.log(n);
  // console.log({d});
  // console.log({t});

  // if (u < 0.0 || v < 0.0 || (u + v) > 1.0 || t < nearValue || t > farValue) {
  //   return false;
  // }
  if (u < 0.0 || v < 0.0 || (u + v) > 1.0 || t < 0 || t > 1) {
    outData.distance = -1;
    glm.vec3.set(outData.normal, 0, 0, 0);
    return -1;
  }

  // const length = glm.vec3.length(normal);

  outData.distance = t;

  glm.vec3.negate(outData.normal, n);
  // glm.vec3.copy(outData.normal, n);

  const length = glm.vec3.length(outData.normal);
  if (length > 0) {
    glm.vec3.scale(outData.normal, outData.normal, 1/length);
  }

  // outDistance = t;
  return t;
};


