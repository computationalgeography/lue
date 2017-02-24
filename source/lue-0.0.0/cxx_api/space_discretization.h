#pragma once
// #include "lue/cxx_api/hdf5/group.h"
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        space_discretization_type_to_string(
                                        SpaceDiscretizationType const type);

SpaceDiscretizationType
                   parse_space_discretization_type(
                                        std::string const& string);


// bool               space_discretization_exists(
//                                         hdf5::Identifier const& location);
// 
// 
// /*!
//     @ingroup    lue_cxx_api_group
// */
// class SpaceDiscretization:
//     public hdf5::Group
// {
// 
// public:
// 
//                    SpaceDiscretization (hdf5::Identifier const& location);
// 
//                    SpaceDiscretization (hdf5::Identifier&& location);
// 
//                    SpaceDiscretization (SpaceDiscretization const& other)
//                                             =delete;
// 
//                    SpaceDiscretization (SpaceDiscretization&& other)=default;
// 
//                    ~SpaceDiscretization()=default;
// 
//     SpaceDiscretization&
//                    operator=           (SpaceDiscretization const& other)
//                                             =delete;
// 
//     SpaceDiscretization&
//                    operator=           (SpaceDiscretization&& other)=default;
// 
// private:
// 
// };
// 
// 
// SpaceDiscretization create_space_discretization(
//                                         hdf5::Identifier const& location);

} // namespace lue
