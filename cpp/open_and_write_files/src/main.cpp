
#include <iostream>
#include <cstdio> // <= EXIT_SUCCESS

#include <fstream>

int main()
{
    std::string filename = "./assets/tmp_test.txt";

    std::ofstream ostream(filename);

    if (ostream.fail())
      return EXIT_FAILURE;

    for (int ii = 0; ii < 5; ++ii)
        ostream << "test " << ii << std::endl;

    ostream.close();

    return EXIT_SUCCESS;
}

