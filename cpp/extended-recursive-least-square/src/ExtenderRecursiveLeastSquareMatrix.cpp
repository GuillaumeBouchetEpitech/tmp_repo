
#include "ExtenderRecursiveLeastSquareMatrix.hpp"

#include "clamp.hpp"

#include <stdexcept>
#include <cassert>
#include <cmath>
#include <iostream>

namespace /*anonymous*/ {

constexpr float k_matrixCellValueRange = 1000.0f * 1000.0f;
// constexpr float k_matrixCellValueRange = 10.0f;

};

ExtenderRecursiveLeastSquareMatrix::ExtenderRecursiveLeastSquareMatrix(std::size_t inSize, float inScale)
  : _size(inSize)
{
  if (inSize < 1U) {
    throw std::invalid_argument("invalid size value");
  }
  if (inScale <= 0.0f) {
    throw std::invalid_argument("invalid scale value");
  }

  // start as an identity matrix
  this->_buffer.resize(inSize * inSize, 0.0f);
  // for (std::size_t ii = 0; ii < inSize; ++ii) {
  //   this->_buffer.at(ii * inSize + ii) = inScale;
  // }

  inScale = std::min(inScale, k_matrixCellValueRange);

  // identity matrix
  for (std::size_t yy = 0; yy < inSize; ++yy)
  for (std::size_t xx = 0; xx < inSize; ++xx) {
    this->_buffer.at(yy * inSize + xx) = ((xx == yy) ? inScale : 0.0f);
  }

}

void ExtenderRecursiveLeastSquareMatrix::mulVec(const std::vector<float>& input, std::vector<float>& output) const
{
  if (input.size() != this->_size) {
    throw std::invalid_argument("invalid vector length for mulVec() method call");
  }

  output.clear();
  output.reserve(this->_size);
  for (std::size_t yy = 0; yy < this->_size; ++yy) {
    float sum = 0.0f;
    for (std::size_t xx = 0; xx < this->_size; ++xx) {
      sum += this->getVal(xx, yy) * input.at(xx);
    }
    output.push_back(sum);
  }
}

// SLOW AND NAIVE APPROACH: BEGIN
// SLOW AND NAIVE APPROACH: BEGIN
// SLOW AND NAIVE APPROACH: BEGIN

// void ExtenderRecursiveLeastSquareMatrix::updateMatrix(const std::vector<float>& K, const std::vector<float>& phi, float lambda)
// {
//   // if (K.size() != this->_size) {
//   //   throw std::invalid_argument("invalid K length for updateMatrix() method call");
//   // }
//   // if (phi.size() != this->_size) {
//   //   throw std::invalid_argument("invalid phi length for updateMatrix() method call");
//   // }
//   assert(K.size() == this->_size);
//   assert(phi.size() == this->_size);

//   for (std::size_t yy1 = 0; yy1 < this->_size; ++yy1) {
//     for (std::size_t xx = 0; xx < this->_size; ++xx) {

//       float phiTP_j = 0.0f;
//       for (std::size_t yy2 = 0; yy2 < this->_size; ++yy2) {
//         // phiTP_j += phi.at(yy2) * this->getVal(xx, yy2);
//         phiTP_j += phi[yy2] * this->_buffer[yy2 * this->_size + xx];
//       }

//       // Update P: P = (P - K * phi^T * P) / lambda
//       // const float currValP = this->getVal(xx, yy1);
//       // this->setVal(xx, yy1, (currValP - K.at(yy1) * phiTP_j) / lambda);
//       const std::size_t savedIndex = yy1 * this->_size + xx;
//       const float currValP = this->_buffer[savedIndex];
//       this->_buffer[savedIndex] = (currValP - K[yy1] * phiTP_j) / lambda;
//     }
//   }
// }

// SLOW AND NAIVE APPROACH: END
// SLOW AND NAIVE APPROACH: END
// SLOW AND NAIVE APPROACH: END


void ExtenderRecursiveLeastSquareMatrix::updateMatrix(
  const std::vector<float>& K,
  const std::vector<float>& phi,
  float lambda)
{
#if 0
  // this is slower... :(
  if (K.size() != _size) {
    throw std::invalid_argument("invalid K length");
  }
  if (phi.size() != _size) {
    throw std::invalid_argument("invalid phi length");
  }
#else
  // this is only (marginally) slower in debug mode
  assert(K.size() == this->_size);
  assert(phi.size() == this->_size);
#endif

  const std::size_t n = _size;
  const float invLambda = 1.0f / lambda;

  // Raw pointer to underlying storage (assumes row-major contiguous layout).
  float* P = this->_buffer.data();
  const float* k = K.data();
  const float* p = phi.data();

  // Step 1: compute the row vector -> u = phi^T * P
  // u[j] = sum_i phi[i] * P[i, j]
  std::vector<float> u(n, 0.0f);
  for (std::size_t ii = 0; ii < n; ++ii) {
    const float phi_i = p[ii];
    const float* Prow = P + ii * n;
    for (std::size_t jj = 0; jj < n; ++jj) {
      u[jj] += phi_i * Prow[jj];
    }
  }

  // Step 2: rank-1 update + scale -> P = (P - K * u) / lambda
  for (std::size_t ii = 0; ii < n; ++ii) {
    const float ki = k[ii];
    float* Prow = P + ii * n;
    for (std::size_t jj = 0; jj < n; ++jj) {
      Prow[jj] = (Prow[jj] - ki * u[jj]) * invLambda;
      Prow[jj] = clamp(Prow[jj], -k_matrixCellValueRange, k_matrixCellValueRange);
    }
  }
}
