
#include <cstdio> // <= EXIT_SUCCESS
#include <iostream>
#include <string>
#include <regex>

int main()
{
    std::cout << "=====" << std::endl;

    std::string content = "burger.letters.com - - [01/Jul/1995:00:00:11 -0400] \"GET /shuttle/countdown/liftoff.html HTTP/1.0\" 304";
    std::regex regexp(R"(^(\S*)\s*-\s*-\s*(\[.*\])\s*\"(\S*)\s*\S*\/(\S*)\s*\S*\"\s*(\S*))");

    std::smatch match;
    std::regex_search(content, match, regexp);

    int index = 0;
    for (std::ssub_match sub_match : match)
    {
        std::string base = sub_match.str();
        std::cout << "[" << index++ << "] => (" << base.size() << ") " << base << std::endl;
    }

    std::cout << "=====" << std::endl;

    return EXIT_SUCCESS;
}
