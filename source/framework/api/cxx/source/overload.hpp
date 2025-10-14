#pragma once


namespace lue::api {

    // Inherit from each T passed in as a template parameter and add its operator() to the overload set
    template<class... Ts>
    struct overload: Ts...
    {
            using Ts::operator()...;
    };

}  // namespace lue::api
