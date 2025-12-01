#include "lue/hdf5/error_stack.hpp"
#include <cassert>


namespace lue::hdf5 {
    namespace {

        // extern "C" herr_t error_handler(
        //     hid_t /* stack_id */,
        //     void* /* client_data */)
        // {
        //     // auto* error_stack = static_cast<ErrorStack*>(client_data);
        //     // error_stack->add_message("meh");
        //
        //     return 0;
        // }


        extern "C" auto error_walker(unsigned int idx, H5E_error2_t const* error, void* client_data)
            -> herr_t;


        auto error_walker([[maybe_unused]] unsigned int idx, H5E_error2_t const* error, void* client_data)
            -> herr_t
        {
            auto* messages = static_cast<std::vector<std::string>*>(client_data);

            // typedef struct H5E_error2_t {
            //     hid_t cls_id;  /* class ID */
            //     hid_t maj_num;  /* major error ID */
            //     hid_t min_num;  /* minor error number */
            //     unsigned line;  /* line in file where error occurs */
            //     const char *func_name;  /* function in which error occurred */
            //     const char *file_name;  /* file in which error occurred */
            //     const char *desc;  /* optional supplied description */
            // } H5E_error2_t;
            messages->emplace_back(error->desc);

            return 0;
        }

    }  // Anonymous namespace


    ErrorStack::ErrorStack():

        ErrorStack{Identifier{H5E_DEFAULT, []([[maybe_unused]] hid_t const id) -> int { return 0; }}}

    {
    }


    ErrorStack::ErrorStack(Identifier id):

        _id{std::move(id)},
        _original_error_handler{nullptr},
        _original_client_data{nullptr}

    {
        // Save current error handler
        H5Eget_auto2(_id, &_original_error_handler, &_original_client_data);

        // Install our error handler
        H5Eset_auto2(_id, nullptr /* error_handler */, this);
    }


    // NOLINTNEXTLINE(modernize-use-equals-default)
    ErrorStack::~ErrorStack()
    {
        // Restore previous error handler
        H5Eset_auto2(_id, _original_error_handler, _original_client_data);
    }


    auto ErrorStack::empty() const -> bool
    {
        ssize_t const nr_error_messages = H5Eget_num(_id);
        assert(nr_error_messages >= 0);

        return nr_error_messages == 0;
    }


    void ErrorStack::clear() const
    {
        [[maybe_unused]] herr_t const result = H5Eclear2(_id);
        assert(result >= 0);
    }


    auto ErrorStack::messages() const -> std::vector<std::string>
    {
        std::vector<std::string> result{};

        [[maybe_unused]] herr_t const status = H5Ewalk2(_id, H5E_WALK_DOWNWARD, error_walker, &result);
        assert(status >= 0);

        return result;
    }

}  // namespace lue::hdf5
