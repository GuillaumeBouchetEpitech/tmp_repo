
#include "ExtenderRecursiveLeastSquareMatrix.hpp"

#include <stdexcept>

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

void ExtenderRecursiveLeastSquareMatrix::updateMatrix(const std::vector<float>& K, const std::vector<float>& phi, float lambda)
{
  if (K.size() != this->_size) {
    throw std::invalid_argument("invalid K length for updateMatrix() method call");
  }
  if (phi.size() != this->_size) {
    throw std::invalid_argument("invalid phi length for updateMatrix() method call");
  }

  for (std::size_t yy1 = 0; yy1 < this->_size; ++yy1) {
    for (std::size_t xx = 0; xx < this->_size; ++xx) {
      float currValP = this->getVal(xx, yy1);

      float phiTP_j = 0.0f;
      for (std::size_t yy2 = 0; yy2 < phi.size(); ++yy2) {
        phiTP_j += phi.at(yy2) * this->getVal(xx, yy2);
      }

      // Update P: P = (P - K * phi^T * P) / lambda
      this->setVal(xx, yy1, (currValP - K.at(yy1) * phiTP_j) / lambda);
    }
  }
}

