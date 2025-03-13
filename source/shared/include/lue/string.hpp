#pragma once
#include <sstream>


namespace lue {

    template<typename Collection>
    auto join(Collection const& elements, std::string const& separator) -> std::string
    {
        std::ostringstream stream{};

        if (!std::empty(elements))
        {
            auto element_it = std::begin(elements);

            stream << *element_it;

            for (++element_it; element_it != std::end(elements); ++element_it)
            {
                stream << separator << *element_it;
            }
        }

        return stream.str();
    }

}  // namespace lue
