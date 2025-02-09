
#include <cstdio> // <= EXIT_SUCCESS
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <stdexcept>

int main()
{
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
    }

    {
        std::cout << "=====" << std::endl;

        std::string content = "#include \"_common-fog-logic.vert\"";
        std::regex regexp(R"(#include\s+\"(\S+)\")");

        std::smatch match;
        std::regex_search(content, match, regexp);

        std::cout << "match.empty()=" << match.empty() << std::endl;
        std::cout << "match.size()=" << match.size() << std::endl;

        if (!match.empty())
        {
            std::cout << "match[1]=" << match[1].str() << std::endl;


            int index = 0;
            for (std::ssub_match sub_match : match)
            {
                std::string base = sub_match.str();
                std::cout << "[" << index++ << "] => (" << base.size() << ") " << base << std::endl;
            }
        }

        std::cout << "=====" << std::endl;
    }

    {
        std::cout << "=====" << std::endl;
        std::cout << "=====" << std::endl;
        std::cout << "=====" << std::endl;
        std::cout << "=====" << std::endl;
        std::cout << "=====" << std::endl;

        std::string content = "#NEW_BONE     name=\"head\"     length=\"3\"     translation=\"1.33,  0   ,-6\"      rotation=\"90,0,0,1\"     scale=\"1,1,1\" parent=\"chest\"";

        // std::regex regexp(R"(#include\s+\"(\S+)\")\s*\"(\S+)\")");


        // std::regex regexp_3f(R"((-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?))");
        // std::regex regexp_3f(R"((-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?))");




        auto getStreamStr = [](std::ostream& stream) -> std::string
        {
            std::string data = dynamic_cast<std::ostringstream&>(stream).str();
            return data;
        };

        #define D_SSTR(stream) getStreamStr(std::ostringstream() << stream)




        std::cout << " --> " << D_SSTR("test " << 666 << " -> " << 777.777f) << std::endl;


        const std::string matchMaybeSpace = R"(\s*?)";
        const std::string matchStrName = R"(([\w\-]+))";
        const std::string matchStrValue = R"(\"(.+?)\")";
        const std::string matchI = R"((-?\d+))";
        const std::string match1F = R"((-?\d+(?:\.\d+)?))";

        const std::string matchS1F = D_SSTR(matchMaybeSpace << match1F << matchMaybeSpace);
        const std::string matchS3F = D_SSTR(matchS1F << "," << matchS1F << "," << matchS1F);

        std::regex regexp_1f(matchS1F);
        std::regex regexp_3f(matchS3F);

        std::regex regexpMain(D_SSTR(matchStrName << "=" << matchStrValue));

        std::smatch match;
        std::smatch subMatch;

        struct vec3 { float x, y, z; };

        auto get1F = [&regexp_1f, &subMatch](const std::string& toMatch) -> float
        {
            std::regex_search(toMatch, subMatch, regexp_1f);
            if (subMatch.empty())
                throw std::runtime_error("LOL?");

            std::string strX = subMatch[1].str();
            return std::atof(strX.c_str());
        };

        auto get3F = [&regexp_3f, &subMatch](const std::string& toMatch) -> vec3
        {
            std::regex_search(toMatch, subMatch, regexp_3f);
            if (subMatch.empty())
                throw std::runtime_error("LOL?");

            vec3 newVal;
            std::string strX = subMatch[1].str();
            newVal.x = std::atof(strX.c_str());
            std::string strY = subMatch[2].str();
            newVal.y = std::atof(strY.c_str());
            std::string strZ = subMatch[3].str();
            newVal.z = std::atof(strZ.c_str());

            return newVal;
        };

        std::cout << "#############" << std::endl;
        std::cout << "#" << std::endl;
        std::cout << "# " << content << std::endl;
        std::cout << "#" << std::endl;
        std::cout << "#############" << std::endl;

        std::string::const_iterator searchStart( content.cbegin() );
        while (std::regex_search(searchStart, content.cend(), match, regexpMain))
        {
            std::cout << " => " << match[0];
            for (std::size_t ii = 1; ii < match.size(); ++ii)
                std::cout << " (" << match[ii] << ")";
            std::cout << std::endl;

            std::string key = match[1].str();
            std::string value = match[2].str();


            // matchStrName

            if (key == "length")
            {
                std::cout << "   -> (" << get1F(value) << ")" << std::endl;
            }
            else
            if (key == "translation")
            {
                vec3 val = get3F(value);
                std::cout << "   -> (" << val.x << ") / (" << val.y << ") / (" << val.z << ")" << std::endl;
            }

            searchStart = match.suffix().first;
        }
        std::cout << std::endl;

        // std::regex_search(content, match, regexp);

        // std::cout << "match.empty()=" << match.empty() << std::endl;
        // std::cout << "match.size()=" << match.size() << std::endl;

        // if (!match.empty())
        // {
        //     std::cout << "match[1]=" << match[1].str() << std::endl;

        //     int index = 0;
        //     for (std::ssub_match sub_match : match)
        //     {
        //         std::string base = sub_match.str();
        //         std::cout << "[" << index++ << "] => (" << base.size() << ") " << base << std::endl;
        //     }
        // }

        std::cout << "=====" << std::endl;
    }

    return EXIT_SUCCESS;
}
