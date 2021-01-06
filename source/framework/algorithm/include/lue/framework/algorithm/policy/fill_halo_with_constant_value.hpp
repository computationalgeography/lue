#pragma once
#include "lue/framework/core/define.hpp"


namespace lue {
namespace policy {

/*!
    @brief      Fill halo cells with a constant value
*/
template<
    typename Element>
class FillHaloWithConstantValue
{

    public:

        FillHaloWithConstantValue(
            Element const fill_value):

            _fill_value{fill_value}

        {
        }

        Element fill_value() const
        {
            return _fill_value;
        }

    private:

        Element const _fill_value;

};

}  // namespace policy
}  // namespace lue
