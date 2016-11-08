#pragma once
#include "lue/cxx_api/hdf5/identifier.h"
#include <functional>
#include <string>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
*/
class Attributes
{

public:

                   Attributes          (Identifier const& location);

                   Attributes          (Attributes const& other)=delete;

                   Attributes          (Attributes&& other)=default;

                   ~Attributes         ()=default;

    Attributes&    operator=           (Attributes const& other)=delete;

    Attributes&    operator=           (Attributes&& other)=default;

    template<
        typename T>
    void           write               (std::string const& name,
                                        T const& value);

    template<
        typename T>
    T              read                (std::string const& name) const;

    bool           exists              (std::string const& name) const;

    Identifier     type_id             (std::string const& name) const;

private:

    std::reference_wrapper<Identifier const> _location;

    template<
        typename T>
    void           create              (std::string const& name);

};

} // namespace hdf5
} // namespace lue
