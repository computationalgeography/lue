#include "lue/framework/benchmark/regex.hpp"
#include <regex>


namespace lue::benchmark {

    std::vector<Index> parse_idxs(std::string const& string)
    {
        std::vector<Index> idxs{};

        std::regex expression{
            "[[:space:]]*([[:digit:]]+)[[:space:]]*(?:,[[:space:]]*([[:digit:]]+)[[:space:]]*)*"};
        std::smatch pieces_match{};

        if (!std::regex_match(string, pieces_match, expression))
        {
            throw std::runtime_error("String with indices not formatted correctly");
        }

        for (size_t i = 1; i < pieces_match.size(); ++i)
        {
            idxs.push_back(std::stoi(pieces_match[i].str()));
        }

        return idxs;
    }

}  // namespace lue::benchmark
