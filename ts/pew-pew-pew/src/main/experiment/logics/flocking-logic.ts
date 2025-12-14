

import * as glm from 'gl-matrix';
import { FlockFaction } from 'main/experiment/logics/FlockManager';






export const _limitVec3 = (
  vec3: glm.vec3,
  maxVec3: number
): glm.vec3 => {

  const length = glm.vec3.length(vec3);
  if (length > maxVec3) {
    glm.vec3.scale(vec3, vec3, 1 / length); // normalize
    glm.vec3.scale(vec3, vec3, maxVec3);
  }
  return vec3;
}



export const _maximizeVec3 = (
  vec3: glm.vec3,
  maxVec3: number
): glm.vec3 => {

  const length = glm.vec3.length(vec3);
  if (length > 0) {
    glm.vec3.scale(vec3, vec3, 1 / length); // normalize
    glm.vec3.scale(vec3, vec3, maxVec3);
  }
  return vec3;
}





export interface IFlockingAgent {
  getPosition(): glm.ReadonlyVec3;
  takeDamage(damage: number): void;
  isTrackable(): boolean;
}




//#region flocking utils







export interface IResult {
  agent: IFlockingAgent;
  diff: glm.vec3;
  distance: number;
};

export const _findClosestAgent = (
  currAgent: IFlockingAgent,
  flockFaction: FlockFaction,
  maxRadius?: number,
): IResult | undefined => {


  let allAgents = flockFaction.allAgents;
  if (maxRadius !== undefined && maxRadius > 0) {
    allAgents = flockFaction.searchByRadius(currAgent.getPosition(), maxRadius);
  }

  // find closest target
  let closestAgent: IFlockingAgent | undefined = undefined;
  let closestDiff = glm.vec3.fromValues(0,0,0);
  let closestDistance: number = 999999999;
  for (const tmpAgent of allAgents) {

    if (!tmpAgent.isTrackable()) {
      continue;
    }

    glm.vec3.sub(closestDiff, tmpAgent.getPosition(), currAgent.getPosition());
    const distance = glm.vec3.squaredLength(closestDiff);

    if (closestDistance > distance) {
      closestDistance = distance;
      closestAgent = tmpAgent;
    }
  }

  if (!closestAgent) {
    return; // no target? -> stop
  }

  return {
    agent: closestAgent,
    diff: closestDiff,
    distance: Math.sqrt(closestDistance)
  };
};

export const _findClosestAgents = (
  currAgent: IFlockingAgent,
  flockFaction: FlockFaction,
  maxRadius: number,
): IResult[] => {

  const results: IResult[] = [];

  // find closest target
  let closestDiff = glm.vec3.fromValues(0,0,0);

  const subResults = flockFaction.searchByRadius(currAgent.getPosition(), maxRadius);

  for (const tmpAgent of subResults) {

    if (!tmpAgent.isTrackable()) {
      continue;
    }

    glm.vec3.sub(closestDiff, tmpAgent.getPosition(), currAgent.getPosition());
    const distance = glm.vec3.squaredLength(closestDiff);

    if (distance > maxRadius) {
      continue;
    }

    results.push({
      agent: tmpAgent,
      diff: glm.vec3.copy(glm.vec3.create(), closestDiff),
      distance
    });
  }

  return results.sort((a, b) => a.distance - b.distance);
};















export const _seek = (
  currAgent: IFlockingAgent,
  target: glm.ReadonlyVec3,
  coefficient: number,
  outAcceleration: glm.vec3
): void  => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const diffVec3 = glm.vec3.create();
  glm.vec3.sub(diffVec3, target, currAgent.getPosition());
  const length = glm.vec3.length(diffVec3);

  // is it invalid?
  if (length <= 0.0) {
    return; // will divide by 0 during normalization -> skip
  }

  glm.vec3.scale(diffVec3, diffVec3, 1 / length); // normalize

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);
}

export const _flee = (
  currAgent: IFlockingAgent,
  target: glm.ReadonlyVec3,
  coefficient: number,
  outAcceleration: glm.vec3
): void  => {

  _seek(
    currAgent,
    target,
    -coefficient,
    outAcceleration,
  );
}

export const _stayInRange = (
  currAgent: IFlockingAgent,
  target: glm.ReadonlyVec3,
  range: number,
  coefficient: number,
  outAcceleration: glm.vec3
): void => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const diffVec3 = glm.vec3.create();
  glm.vec3.sub(diffVec3, target, currAgent.getPosition());
  const length = glm.vec3.length(diffVec3);

  // is it invalid?
  if (length <= 0.0) {
    return; // will divide by 0 during normalization -> skip
  }

  // console.log('length', length);

  glm.vec3.scale(diffVec3, diffVec3, 1 / length); // normalize

  if (length < range - 1) {

    // flee
    coefficient = -coefficient;
  }

  // console.log('diffVec3', diffVec3);

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);
}



export const _stayInRangeFlock = (
  currAgent: IFlockingAgent,
  flockFaction: FlockFaction,
  minRange: number,
  maxRange: number,
  coefficient: number,
  outAcceleration: glm.vec3
): IResult | undefined => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const result = _findClosestAgent(currAgent, flockFaction);
  if (!result) {
    return;
  }

  if (result.distance === 0.0) {
    return; // would divide by 0 during normalization -> skip
  }

  const diffVec3 = glm.vec3.create();
  glm.vec3.scale(diffVec3, result.diff, 1 / result.distance); // normalize

  if (result.distance > minRange && result.distance < maxRange) {
    return result; // in range -> do nothing
  }

  if (result.distance < maxRange) {
    // flee
    coefficient = -coefficient;
  }

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);

  return result;
}




export const _strafe = (
  currAgent: IFlockingAgent,
  target: glm.ReadonlyVec3,
  radius: number,
  coefficient: number,
  outAcceleration: glm.vec3
): void => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const diffVec3 = glm.vec3.create();
  glm.vec3.sub(diffVec3, target, currAgent.getPosition());
  const length = glm.vec3.length(diffVec3);

  // is it invalid?
  if (length <= 0.0) {
    return; // will divide by 0 during normalization -> skip
  }

  // console.log('length', length);

  glm.vec3.scale(diffVec3, diffVec3, 1 / length); // normalize

  if (length > radius - 2 && length < radius + 2) {

    // strafe

    const angle = Math.atan2(diffVec3[1], diffVec3[0]);
    diffVec3[0] = Math.cos(angle + Math.PI * 0.5);
    diffVec3[1] = Math.sin(angle + Math.PI * 0.5);

  } else if (length < radius - 2) {

    // flee
    coefficient = -coefficient;
  }

  // console.log('diffVec3', diffVec3);

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);
}

export const _strafeFlock = (
  currAgent: IFlockingAgent,
  flockFaction: FlockFaction,
  minRange: number,
  maxRange: number,
  coefficient: number,
  outAcceleration: glm.vec3
): IResult | undefined => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const result = _findClosestAgent(currAgent, flockFaction);
  if (!result) {
    return;
  }

  // is it invalid?
  if (result.distance <= 0.0) {
    return; // will divide by 0 during normalization -> skip
  }

  const diffVec3 = glm.vec3.copy(glm.vec3.create(), result.diff);
  glm.vec3.scale(diffVec3, result.diff, 1 / result.distance); // normalize

  if (result.distance > minRange && result.distance < maxRange) {
    // strafe
    const strafingAngle = Math.atan2(diffVec3[1], diffVec3[0]) + Math.PI * 0.5;
    diffVec3[0] = Math.cos(strafingAngle);
    diffVec3[1] = Math.sin(strafingAngle);
  }
  else if (result.distance < minRange) {
    // flee
    coefficient = -coefficient;
  }
  else {
    // seek
    // -> nothing to do
  }

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);

  return result;
}

export const _separation = (
  currAgent: IFlockingAgent,
  flockFaction: FlockFaction,
  minRange: number,
  maxRange: number,
  coefficient: number,
  outAcceleration: glm.vec3
): void => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  let totalToEvade: number = 0;

  const subResult = flockFaction.searchByRadius(currAgent.getPosition(), maxRange);

  // console.log(subResult.length)

  // const subResult = flockFaction.allAgents;

  // loop and evade other agents
  for (const tmpAgent of subResult) {

    // ignore self
    if (currAgent === tmpAgent) {
      continue;
    }

    const diffVec3 = glm.vec3.create();
    glm.vec3.sub(diffVec3, currAgent.getPosition(), tmpAgent.getPosition());
    const length = glm.vec3.squaredLength(diffVec3);

    // is in range?
    if (length < (minRange*minRange) || length > (maxRange*maxRange)) {
      continue; // not in range -> skip
    }

    // is it invalid?
    if (length <= 0.0) {
      continue; // will divide by 0 during normalization -> skip
    }

    glm.vec3.scale(diffVec3, diffVec3, 1 / Math.sqrt(length)); // normalize

    glm.vec3.add(outAcceleration, outAcceleration, diffVec3);
    totalToEvade += 1;
  }

  if (totalToEvade > 0) {
    // normalize accumulated acceleration
    glm.vec3.scale(outAcceleration, outAcceleration, 1 / totalToEvade);

    // apply coefficient
    glm.vec3.scale(outAcceleration, outAcceleration, coefficient);
  }
}

// export const _cohesion = (
//   currAgent: IFlockingAgent,
//   allAgents: ReadonlyArray<IFlockingAgent>,
//   minRange: number,
//   maxRange: number,
//   coefficient: number,
//   outAcceleration: glm.vec3
// ): void => {

//   _separation(
//     currAgent,
//     allAgents,
//     minRange,
//     maxRange,
//     -coefficient, // <- only difference
//     outAcceleration,
//   );
// }

// export const _alignment = (
//   currAgent: IFlockingAgent,
//   allAgents: ReadonlyArray<IFlockingAgent>,
//   // minRange: number,
//   // maxRange: number,
//   coefficient: number,
//   outAcceleration: glm.vec3
// ): void => {

//   // reset
//   glm.vec3.set(outAcceleration, 0, 0, 0);

//   let totalToAlign: number = 0;

//   // loop and evade other agents
//   for (const tmpAgent of allAgents) {

//     // ignore self
//     if (currAgent === tmpAgent) {
//       continue;
//     }

//     const diffVec3 = glm.vec3.create();
//     glm.vec3.sub(diffVec3, currAgent.getForwardAxis(), tmpAgent.getForwardAxis());
//     const length = glm.vec3.length(diffVec3);

//     // // is in range?
//     // if (length < minRange || length > maxRange) {
//     //   continue; // not in range -> skip
//     // }

//     // is it invalid?
//     if (length <= 0.0) {
//       continue; // will divide by 0 during normalization -> skip
//     }

//     glm.vec3.scale(diffVec3, diffVec3, 1 / length); // normalize

//     glm.vec3.add(outAcceleration, outAcceleration, diffVec3);
//     totalToAlign += 1;
//   }

//   if (totalToAlign > 0) {
//     // normalize accumulated acceleration
//     glm.vec3.scale(outAcceleration, outAcceleration, 1 / totalToAlign);

//     // apply coefficient
//     glm.vec3.scale(outAcceleration, outAcceleration, coefficient);
//   }
// }



export const _stayInBattle = (
  currAgent: IFlockingAgent,
  center: glm.ReadonlyVec3,
  halfSize: glm.ReadonlyVec3,
  coefficient: number,
  outAcceleration: glm.vec3
): boolean => {

  // reset
  glm.vec3.set(outAcceleration, 0, 0, 0);

  const isOutside = (
    currAgent.getPosition()[0] > center[0] + halfSize[0] ||
    currAgent.getPosition()[1] > center[1] + halfSize[1] ||
    currAgent.getPosition()[2] > center[2] + halfSize[2] ||
    currAgent.getPosition()[0] < center[0] - halfSize[0] ||
    currAgent.getPosition()[1] < center[1] - halfSize[1] ||
    currAgent.getPosition()[2] < center[2] - halfSize[2]
  );

  if (!isOutside) {
    return false; // is in battle
  }

  const diffVec3 = glm.vec3.create();
  glm.vec3.sub(diffVec3, center, currAgent.getPosition());
  const length = glm.vec3.length(diffVec3);

  // is it invalid?
  if (length <= 0.0) {
    // will divide by 0 during normalization -> skip
    return true; // is out of battle
  }

  // if (length < radius) {
  //   return; // in battle -> skip
  // }

  // out of range -> seek

  glm.vec3.scale(diffVec3, diffVec3, 1 / length); // normalize

  // apply coefficient
  glm.vec3.scale(outAcceleration, diffVec3, coefficient);
  return true; // is out of battle
}




//#endregion

