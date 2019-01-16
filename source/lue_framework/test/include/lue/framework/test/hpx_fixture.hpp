#pragma once


namespace lue {

class HPXFixture
{

public:

                   HPXFixture          ();

                   HPXFixture          (HPXFixture const&)=delete;

                   HPXFixture          (HPXFixture&&)=delete;

                   ~HPXFixture         ();

   HPXFixture&     operator=           (HPXFixture const&)=delete;

   HPXFixture&     operator=           (HPXFixture&&)=delete;

private:

};

}  // namespace lue
