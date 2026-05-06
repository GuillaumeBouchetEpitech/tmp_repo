
#pragma once

#include <vector>

class ExtenderRecursiveLeastSquareMatrix {

public:
  ExtenderRecursiveLeastSquareMatrix(std::size_t inSize, float inScale);

public:
  void mulVec(const std::vector<float>& input, std::vector<float>& output) const;
  void updateMatrix(const std::vector<float>& K, const std::vector<float>& phi, float lambda);

public:
  void setVal(std::size_t xx, std::size_t yy, float value) { this->_buffer.at(yy * this->_size + xx) = value; }

public:
  float getVal(std::size_t xx, std::size_t yy) const { return this->_buffer.at(yy * this->_size + xx); }

private:
  std::vector<float> _buffer;
  std::size_t _size;

};
