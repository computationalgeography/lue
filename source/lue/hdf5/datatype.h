#pragma once
#include "lue/hdf5/identifier.h"
#include "lue/hdf5/type_traits.h"


namespace lue {
namespace hdf5 {

class Datatype
{

public:

                   Datatype            (size_t const nr_bytes);

    template<
        typename T>
                   Datatype            (T const&);

                   Datatype            (hid_t const id);

                   Datatype            (Identifier&& id);

                   Datatype            (Datatype const& other)=default;

                   Datatype            (Datatype&& other)=default;

                   ~Datatype           ()=default;

    Datatype&      operator=           (Datatype const& other)=default;

    Datatype&      operator=           (Datatype&& other)=default;

    Identifier const& id               () const;

    void           set_size            (size_t const nr_bytes);

    size_t         size                () const;

    ::H5T_cset_t   encoding            () const;

    bool           is_standard         () const;

    bool           is_native           () const;

private:

    Identifier     _id;

};


template<
    typename T>
inline Datatype::Datatype(
    T const&)

    : _id(TypeTraits<T>::datatype, [](hid_t){})

{
}


bool               operator==          (Datatype const& lhs,
                                        Datatype const& rhs);

bool               operator!=          (Datatype const& lhs,
                                        Datatype const& rhs);


std::string        native_datatype_as_string(
                                        Datatype const& datatype);

std::string        standard_datatype_as_string(
                                        Datatype const& datatype);

} // namespace hdf5
} // namespace lue
