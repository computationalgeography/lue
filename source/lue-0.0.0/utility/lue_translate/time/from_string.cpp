#include "lue/utility/lue_translate/time/to_string.h"
#include <boost/spirit/home/x3.hpp>


namespace lue {

std::tuple<bool, MonthTimePoint, MonthDuration> parse_months(
    std::string const& time_point_string,
    std::string const& duration_string)
{
    namespace x3 = boost::spirit::x3;

    std::string tokens;
    {
        auto const rule = x3::rule<class pattern, std::string>() =
            // Year
            x3::digit >> x3::digit >> x3::digit >> x3::digit >>

            x3::lit('-') >>

            // Month
            x3::digit >> x3::digit
            ;

        auto first = time_point_string.begin();
        auto const last = time_point_string.end();

        if(!x3::parse(first, last, rule, tokens)) {
            return std::make_tuple(false, MonthTimePoint{}, MonthDuration{});
        }

        assert(first == last);
        assert(tokens.size() == 6);
    }

    MonthTimePoint::Year year = std::stoi(std::string(tokens.data(), 4));
    MonthTimePoint::Month month = std::stoi(std::string(tokens.data() + 4, 2));

    MonthTimePoint time_point(year, month);


    int nr_months;
    {
        auto const rule = x3::rule<class pattern, int>() =
            // This is period of...
            x3::lit('P') >>
            // 1-9...
            x3::int_ >>
            // months
            x3::lit('M')
            ;

        auto first = duration_string.begin();
        auto const last = duration_string.end();

        if(!x3::parse(first, last, rule, nr_months)) {
            return std::make_tuple(false, MonthTimePoint{}, MonthDuration{});
        }

        assert(first == last);
    }

    MonthDuration duration(nr_months);

    return std::make_tuple(true, time_point, duration);
}

}  // namespace lue
