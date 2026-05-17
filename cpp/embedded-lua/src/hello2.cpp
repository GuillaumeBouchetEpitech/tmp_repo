
#include <LuaCpp.hpp>
#include <iostream>

using namespace LuaCpp;
using namespace std;

int main(int argc, char** argv)
{
  LuaContext ctx;

  // Creating a shared string pointer
  auto str = make_shared<Engine::LuaTString>("world from C++!");

  ctx.AddGlobalVariable("world", str);
  ctx.CompileString("test",
                  "print('Hello '..world)\n"
                  "world = 'world from lua!'");

  try {
    ctx.Run("test");
  } catch (runtime_error& e) {
    cout << e.what() << '\n';
  }

  cout << "Hello " << str->getValue() << "\n";
  return 0;
}