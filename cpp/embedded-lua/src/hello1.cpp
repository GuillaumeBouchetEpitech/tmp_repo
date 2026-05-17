
#include <LuaCpp.hpp>
#include <iostream>

using namespace LuaCpp;
using namespace std;

int main(int argc, char** argv)
{
  cout << "Hi from C++, this is a demo how LuaCpp can be used\n";

  LuaContext lua;

  try {
    lua.CompileStringAndRun("print('The fastest way to start using lua in a project (LOL <3)')");
  } catch (std::runtime_error& e) {
    std::cout << e.what() << '\n';
  }
  return 0;
}
