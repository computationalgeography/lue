#pragma once
#include <boost/filesystem.hpp>


namespace lue {
namespace utility {

class StackName
{

public:

    explicit       StackName           (std::string const& name);

                   StackName           (StackName const&)=default;

                   StackName           (StackName&&)=default;

                   ~StackName          ()=default;

    StackName&     operator=           (StackName const&)=default;

    StackName&     operator=           (StackName&&)=default;

    std::string    operator[]          (std::size_t idx) const;

private:

    boost::filesystem::path _parent_path;

    boost::filesystem::path _stem;

    boost::filesystem::path _extension;

};

}  // namespace utility
}  // namespace lue
