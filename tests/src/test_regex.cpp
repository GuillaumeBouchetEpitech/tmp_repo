
#include <iostream>
#include <string>
#include <regex>

void test_regex()
{
    {
        const std::string fnames[] = {"foo.txt", "bar.txt", "baz.dat", "zoidberg"};

        // Extraction of several sub-matches
        const std::regex pieces_regex("([a-z]+)\\.([a-z]+)");
        std::smatch pieces_match;

        for (const auto &fname : fnames)
        {
            if (std::regex_match(fname, pieces_match, pieces_regex))
            {
                std::cout << fname << '\n';
                for (size_t i = 0; i < pieces_match.size(); ++i)
                {
                    std::ssub_match sub_match = pieces_match[i];
                    std::string piece = sub_match.str();
                    std::cout << "  submatch " << i << ": " << piece << '\n';
                }
            }
        }
    }

    {
        std::cout << std::endl;
        std::cout << "custom test" << std::endl;
        std::cout << std::endl;

        const std::string lineToTest = "<tag1 name1 = \"test1\" name2 = \"test2\">";
        // const std::string lineToTest = "<tag1>";

        std::stringstream sstr;
        sstr << lineToTest;

        std::string tagName;
        sstr >> tagName;
        tagName.erase(tagName.begin());
        std::cout << "tagName=" << tagName << std::endl;

        std::string word;
        while (sstr >> word)
            std::cout << word << std::endl;

        // const std::regex pieces_regex("<([a-z0-9]+)\\s([a-z0-9]+)\\s=\\s\"([a-z0-9]+)\">");
        // // const std::regex pieces_regex("<([a-z0-9]+)>");
        // std::smatch pieces_match;
        // if (std::regex_match(lineToTest, pieces_match, pieces_regex))
        // {
        //     std::cout << lineToTest << '\n';
        //     for (size_t ii = 0; ii < pieces_match.size(); ++ii)
        //     {
        //         std::ssub_match sub_match = pieces_match[ii];
        //         std::string piece = sub_match.str();
        //         std::cout << "  submatch " << ii << ": " << piece << '\n';
        //     }
        // }

        std::cout << std::endl;
        std::cout << "/custom test" << std::endl;
        std::cout << std::endl;
    }
}
