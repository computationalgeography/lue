#include "lue/translate/format/geo_eas.hpp"
#include "lue/hl/time_series.hpp"
#include <boost/format.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/tokenizer.hpp>
#include <filesystem>
#include <fstream>


namespace lue {
namespace utility {
namespace {

class GeoEASHeader
{

public:

    using Variables = std::vector<std::string>;

    GeoEASHeader()=default;

    GeoEASHeader(
        std::string const& title,
        Variables const& variables)
        : _title{title},
          _variables{variables}
    {
    }

    GeoEASHeader(GeoEASHeader const&)=delete;

    GeoEASHeader(GeoEASHeader&&)=default;

    ~GeoEASHeader()=default;

    GeoEASHeader& operator=(GeoEASHeader const&)=delete;

    GeoEASHeader& operator=(GeoEASHeader&&)=default;

    std::string const& title() const
    {
        return _title;
    }

    Variables const& variables() const
    {
        return _variables;
    }

    std::size_t nr_variables() const
    {
        return _variables.size();
    }

private:

    std::string    _title;

    Variables      _variables;

};


// template<
//     typename T>
// struct TypeTraits
// {
//     std::string name();
// };


class ColumnDefinition
{

public:

    ColumnDefinition()=default;

    explicit ColumnDefinition(
        std::string const& value_type_name):

        _value_type_name{value_type_name}

    {
    }

    // template<
    //     typename T>
    // ColumnDefinition()
    //     : _value_type_name{TypeTraits<T>::name()}
    // {
    // }

    ~ColumnDefinition()=default;

    std::string const& value_type_name() const
    {
        return _value_type_name;
    }

private:

    std::string    _value_type_name;

};


class TableDefinition
{

public:

    using ColumnDefinitions = std::vector<ColumnDefinition>;

    TableDefinition()=default;

    TableDefinition(
        ColumnDefinitions const& column_definitions,
        char const separator)
        : _separator{separator},
          _column_definitions{column_definitions}
    {
    }

    ~TableDefinition()=default;

    char separator() const
    {
        return _separator;
    }

    ColumnDefinitions const& column_definitions() const
    {
        return _column_definitions;
    }

    std::size_t nr_columns() const
    {
        return _column_definitions.size();
    }

private:

    char           _separator;

    ColumnDefinitions _column_definitions;

};


class Table
{

public:

    template<
        typename T>
    using Column = std::vector<T>;
    using SomeColumn = boost::variant<Column<int>, Column<double>>;
    using Columns = std::vector<SomeColumn>;

    Table()=default;

    Table(Table const&)=delete;

    Table(Table&&)=default;

    Table(
        TableDefinition const& definition,
        Columns&& columns)
        : _definition{definition},
          _columns(std::move(columns))
    {
        assert(_columns.size() == _definition.nr_columns());
    }

    ~Table()=default;

    Table& operator=(Table const&)=delete;

    Table& operator=(Table&&)=default;

    TableDefinition const& definition() const
    {
        return _definition;
    }

    template<
        typename T>
    Column<T> const& column(std::size_t const idx) const
    {
        assert(idx < _columns.size());

        return boost::get<Column<T>>(_columns[idx]);
    }

    std::size_t nr_columns() const
    {
        return _columns.size();
    }

    std::size_t nr_records() const
    {
        assert(_definition.nr_columns() > 0);
        assert(!_columns.empty());

        auto const& column_definition = _definition.column_definitions()[0];
        std::size_t result = 0;

        if(column_definition.value_type_name() == "int") {
            result = column<int>(0).size();
        }
        else if(column_definition.value_type_name() == "double") {
            result = column<double>(0).size();
        }
        else {
            assert(false);
        }

        return result;
    }

private:

    TableDefinition _definition;

    Columns        _columns;

};


template<
    typename Iterator>
bool parse_header(
    Iterator& first,
    Iterator last,
    GeoEASHeader& header)
{
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;

    bool status;


    // First line contains the title
    qi::rule<Iterator, std::string()> rule = +(qi::alnum | qi::punct | qi::blank);
    std::string title;
    status = qi::parse(first, last, rule >> qi::eol, title);

    if(!status) {
        return false;
    }


    // Second line contains nr_variables
    std::size_t nr_variables;
    status = qi::parse(first, last, qi::uint_ >> qi::eol, nr_variables);

    if(!status) {
        return false;
    }


    // Next nr_variables lines contain the description of the variables
    rule = +(qi::alnum | qi::punct | qi::blank);
    GeoEASHeader::Variables variables;
    status = qi::parse(
        first, last,
        qi::repeat(nr_variables)[rule >> qi::eol],
        variables);

    if(!status) {
        return false;
    }


    header = GeoEASHeader(title, variables);

    return true;
}


bool token_is_int(
        std::string const& token)
{
    namespace qi = boost::spirit::qi;

    auto first = token.begin();
    auto last = token.end();

    return qi::parse(first, last, qi::int_) && first == last;
}


bool token_is_double(
        std::string const& token)
{
    namespace qi = boost::spirit::qi;

    auto first = token.begin();
    auto last = token.end();

    return qi::parse(first, last, qi::double_) && first == last;
}


template<
    typename Iterator>
bool parse_table_definition(
    Iterator first,
    Iterator last,
    TableDefinition& table_definition)
{
    // For now, assume separator is a space
    // Figure out:
    // - Number of columns
    // - Value type of columns

    auto eol = std::find(first, last, '\n');

    if(eol == last) {
        return false;
    }

    std::string line(first, eol);

    using Tokenizer = boost::tokenizer<boost::char_separator<char>>;
    using Separator = boost::char_separator<char>;

    char separator_character = ' ';
    Separator separator{" "};
    Tokenizer tokens{line, separator};

    TableDefinition::ColumnDefinitions column_definitions;

    for(auto const& token: tokens) {
        if(token_is_int(token)) {
            column_definitions.emplace_back("int");
        }
        else if(token_is_double(token)) {
            column_definitions.emplace_back("double");
        }
        else {
            assert(false);
        }
    }

    table_definition = TableDefinition(column_definitions, separator_character);

    return true;
}


template<
    typename Iterator>
bool parse_table(
    Iterator& first,
    Iterator last,
    TableDefinition const& definition,
    Table& table)
{
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;

    Table::Columns columns;
    columns.reserve(definition.nr_columns());

    using Rule = qi::rule<Iterator>;
    std::vector<Rule> column_parsers;
    column_parsers.reserve(definition.nr_columns());

    for(auto const& column_definition: definition.column_definitions()) {
        // - Add empty column to table
        // - Update parser
        // - Make it dump values in this specific column

        if(column_definition.value_type_name() == "int") {
            using Column = Table::Column<int>;

            columns.emplace_back(Column{});
            auto& column = boost::get<Column>(columns.back());

            column_parsers.emplace_back(
                qi::int_[phoenix::push_back(phoenix::ref(column), qi::_1)]);
        }
        else if(column_definition.value_type_name() == "double") {
            using Column = Table::Column<double>;

            columns.emplace_back(Column{});
            auto& column = boost::get<Column>(columns.back());

            column_parsers.emplace_back(
                qi::double_[phoenix::push_back(phoenix::ref(column), qi::_1)]);
        }
        else {
            assert(false);
        }
    }

    // Parse table rows
    Rule row_parser = column_parsers[0].copy();

    for(std::size_t i = 1; i < column_parsers.size(); ++i) {
        row_parser =
            row_parser.copy() >> +qi::blank >> column_parsers[i].copy();
    }

    row_parser = *qi::blank >> row_parser.copy() >> *qi::blank >> qi::eol;

    bool status = qi::parse(first, last, +row_parser >> qi::eoi);

    if(!status) {
        return false;
    }

    table = Table(definition, std::move(columns));

    return true;
}

}  // Anonymous namespace


void translate_geo_eas_to_lue(
    std::string const& geo_eas_pathname,
    std::string const& lue_pathname,
    Metadata const& metadata)
{
    std::ifstream stream(geo_eas_pathname);

    if(!stream) {
        throw std::runtime_error("Cannot open " + geo_eas_pathname);
    }

    stream.unsetf(std::ios::skipws);

    boost::spirit::istream_iterator begin(stream);
    boost::spirit::istream_iterator end;

    GeoEASHeader geo_eas_header;

    if(!parse_header(begin, end, geo_eas_header)) {
        throw std::runtime_error("Cannot parse Geo-EAS header");
    }

    TableDefinition table_definition;

    if(!parse_table_definition(begin, end, table_definition)) {
        throw std::runtime_error("Cannot parse Geo-EAS table definition");
    }

    assert(table_definition.nr_columns() > 1);
    assert(table_definition.nr_columns() == geo_eas_header.nr_variables());
    assert(table_definition.column_definitions()[0].value_type_name() == "int");
    for(std::size_t idx = 1; idx < table_definition.nr_columns(); ++idx) {
        assert(
            table_definition.column_definitions()[idx].value_type_name() ==
            "double");
    }

    Table table;

    if(!parse_table(begin, end, table_definition, table) || begin != end) {
        throw std::runtime_error("Cannot parse Geo-EAS table body");
    }

    // - First column represents time steps of discretized time box
    // - Subsequent columns represent temporal property values for
    //   nr_columns-1 items

    // Create phenomenon
    // - Name of phenomenon is obtained from supplied meta-data

    // Create property-set
    // - Name of property-set is obtained from supplied meta-data

    std::string const geo_eas_dataset_name =
        std::filesystem::path(geo_eas_pathname).stem().string();
    std::string phenomenon_name = geo_eas_dataset_name;
    std::string property_set_name = "point";

    auto const& root_json = metadata.object();

    auto const dataset_json = json::object(
        root_json, JSONPointer{"/datasets"}, "name",
        geo_eas_dataset_name);

    if(json::has_key(dataset_json, "phenomenon")) {
        auto const phenomenon_pointer =
            json::pointer(dataset_json, "phenomenon");
        auto const phenomenon_json =
            json::object(root_json, phenomenon_pointer);
        phenomenon_name = json::string(phenomenon_json, "name");

        if(json::has_key(phenomenon_json, "property_set")) {
            property_set_name = json::string(phenomenon_json,
                    JSON::json_pointer("/property_set/name"));
        }
    }

    auto lue_dataset = create_dataset(lue_pathname);

    // Create time domain
    // - TODO: Verify first column is monotonically increasing
    // - Set-up time domain and discretization
    // - Clock, units and time period is obtained from supplied meta-data

    auto const time_json = json::object(dataset_json, "time");
    auto const clock_json = json::object(time_json, "clock");
    auto const clock = json::clock(clock_json);
    auto const box_json = json::object(time_json, "box");
    time::DurationCount const start_time_point = box_json.at(0);
    time::DurationCount const end_time_point = box_json.at(1);
    auto const nr_steps = table.nr_records();


    // Create space domain
    // - Space points are obtained from supplied meta-data
    // - Number of space points must equal nr_columns-1
    auto const space_json = json::object(dataset_json, "space");
    auto const point_json = json::object(space_json, "point");
    auto const nr_points = point_json.size();
    auto const nr_items = table.nr_columns() - 1;

    if(nr_points != nr_items) {
        throw std::runtime_error(boost::str(
            boost::format(
                "Number of space points in meta-data file (%1%) must equal the "
                "number of items (number of columns - 1) in the GeoEAS dataset "
                "(%2%)") %
            nr_points % nr_items));
    }

    auto const crs = json::string(space_json, "crs");
    hl::TimeSeries::SpaceDomain::Coordinates coordinates;
    coordinates.reserve(2 * nr_points);

    for(auto point: point_json) {
        coordinates.emplace_back(point[0]);
        coordinates.emplace_back(point[1]);
    }

    assert(coordinates.size() == 2 * nr_points);

    hl::TimeSeries::SpaceDomain const space_domain{crs, std::move(coordinates)};

    auto const property_name = json::string(
        dataset_json, JSONPointer{"/property/name"});

    hl::TimeSeriesDomain const time_domain{
        clock, start_time_point, end_time_point};
    hl::TimeSeriesDiscretization const time_discretization{nr_steps};


    auto time_series = hl::create_time_series(
        lue_dataset, phenomenon_name, property_set_name,
        time_domain, space_domain, property_name, time_discretization);


    // Create name property
    // - Name is 'name'. Use increment in case such property already exists
    // - Value is column name from table header

    // auto memory_datatype = hdf5::create_string_datatype();
    // time_series.add_property("name", memory_datatype);

    // TODO
    // time_series.add_names(geo_eas_header.variables());

    // Create value property
    for(std::size_t item_idx = 0; item_idx < nr_items; ++item_idx) {
        time_series.write(item_idx, table.column<double>(item_idx + 1).data());
    }
}

}  // namespace utility
}  // namespace lue
