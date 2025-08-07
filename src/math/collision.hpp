#pragma once

// TODO: WORK ON THIS

// TODO: rename to instersectAABB
// inline bool detectAABBCollision(
//         const Position3f& posA, const Scale2f& scaleA,
//         const Position3f& posB, const Scale2f& scaleB
// ) {
//     return !(posA.x + scaleA.x <= posB.x || posA.x >= posB.x + scaleB.x || 
//              posA.y + scaleA.y <= posB.y || posA.y >= posB.y + scaleB.y);
// }

// TODO: change to intersectCircle
// inline void resolveCircleCollision(
//         Position3f& posA, const float radiusA,
//         Position3f& posB, const float radiusB
// ) {
//     float dx = posB.x - posA.x;
//     float dy = posB.y - posA.y;
//     float distSquared = (dx * dx) + (dy * dy);
//     float radiusSum = radiusA + radiusB;

//     // if no collision then exit
//     if (distSquared >= (radiusSum * radiusSum)) return;

//     // compute distance
//     float dist = std::sqrt(distSquared);

//     // TODO: As an added speed hack, you can calculate the distance with
//     // Pythagoras' theorem, but don't calculate the expensive square root,
//     // just keep your total centre-to-centre distance and compare to the 
//     // square of the sum of the radii. 

//     // avoid division by zero for perfectly overlapping circles
//     constexpr float epsilon = 1e-6f;
//     if (dist < epsilon) {
//         dx = 1.0f; // arbitrary push direction
//         dy = 0.0f;
//         dist = 1.0f;
//     }

//     // compute penetration depth (overlap distance)
//     float overlap = radiusSum - dist;

//     // normalize the displacement vector
//     float invDist = 1.0f / dist;
//     float nx = dx * invDist;
//     float ny = dy * invDist;

//     // push both units away proportionally
//     float correction = overlap * 0.5f;
//     posA.x -= nx * correction;
//     posA.y -= ny * correction;
//     posB.x += nx * correction;
//     posB.y += ny * correction;
// }