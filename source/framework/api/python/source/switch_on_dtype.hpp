#include <pybind11/numpy.h>
#include <format>


namespace lue::api {

    template<typename Result, typename CaseHandler, typename... Arguments>
    auto switch_on_dtype(pybind11::dtype const& dtype, Arguments&&... arguments) -> Result
    {
        auto const kind = dtype.kind();
        auto const size = dtype.itemsize();  // bytes
        std::optional<Result> result{};

        // NOLINTBEGIN(bugprone-unhandled-code-paths, bugprone-switch-missing-default-case)
        switch (kind)
        {
            case 'i':
            {
                // Signed integer
                switch (size)
                {
                    case 1:
                    {
                        result = CaseHandler::template operator()<std::int8_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 2:
                    {
                        result = CaseHandler::template operator()<std::int16_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 4:
                    {
                        result = CaseHandler::template operator()<std::int32_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 8:
                    {
                        result = CaseHandler::template operator()<std::int64_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                }

                break;
            }
            case 'u':
            {
                // Unsigned integer
                switch (size)
                {
                    case 1:
                    {
                        result = CaseHandler::template operator()<std::uint8_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 2:
                    {
                        result = CaseHandler::template operator()<std::uint16_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 4:
                    {
                        result = CaseHandler::template operator()<std::uint32_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 8:
                    {
                        result = CaseHandler::template operator()<std::uint64_t>(
                            std::forward<Arguments>(arguments)...);
                        break;
                    }
                }

                break;
            }
            case 'f':
            {
                // Floating-point
                switch (size)
                {
                    case 4:
                    {
                        result =
                            CaseHandler::template operator()<float>(std::forward<Arguments>(arguments)...);
                        break;
                    }
                    case 8:
                    {
                        result =
                            CaseHandler::template operator()<double>(std::forward<Arguments>(arguments)...);
                        break;
                    }
                }

                break;
            }
        }
        // NOLINTEND(bugprone-unhandled-code-paths, bugprone-switch-missing-default-case)

        if (!result)
        {
            throw std::runtime_error(std::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
        }

        return std::move(*result);
    }

}  // namespace lue::api
