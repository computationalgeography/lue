#pragma once


namespace lue {

    class HPXFixture
    {

        public:

            HPXFixture() = default;

            HPXFixture(HPXFixture const&) = delete;

            HPXFixture(HPXFixture&&) = delete;

            virtual ~HPXFixture() = default;

            HPXFixture& operator=(HPXFixture const&) = delete;

            HPXFixture& operator=(HPXFixture&&) = delete;

        private:
    };

}  // namespace lue
