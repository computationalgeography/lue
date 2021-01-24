#pragma once
#include <functional>
#include <memory>
#include <vector>


namespace lue {

class CommandLine
{

    public:

        CommandLine();

        int argc() const;

        char** argv() const;

    private:

        using ArgumentPointer = std::unique_ptr<char, std::function<void(char*)>>;

        // For each argument an array of characters
        std::vector<ArgumentPointer> _argument_smart_pointers;

        // For each argument a pointer to the array of characters
        std::vector<char*> _argument_pointers;

        // Command line arguments, to be used by HPX runtime startup code
        int _argc;
        char** _argv;

};

}  // namespace lue
