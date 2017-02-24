#pragma once
#include "lue/hdf5/datatype.h"


namespace lue {
namespace hdf5 {

class Attributes
{

public:

                   Attributes          (Identifier const& id);

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

    Datatype       datatype            (std::string const& name) const;

private:

    std::reference_wrapper<Identifier const> _id;

};


} // namespace hdf5
} // namespace lue
