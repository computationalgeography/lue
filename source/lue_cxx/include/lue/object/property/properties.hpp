#pragma once
#include "lue/info/identity/active_id.hpp"
#include "lue/info/identity/active_object_index.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/info/identity/id.hpp"


namespace lue {

class Properties:
    public hdf5::Group
{

public:

    friend Properties create_properties(hdf5::Group& parent);

    explicit       Properties          (hdf5::Group const& parent);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

    info::ID const& id                 () const;

    info::ActiveSetIndex const&
                   active_set_index    () const;

    info::ActiveObjectIndex const&
                   active_object_index () const;

    info::ActiveID const&
                   active_id           () const;

private:

                   Properties          (
                                hdf5::Group&& group,
                                info::ID&& id,
                                info::ActiveSetIndex&& active_set_index,
                                info::ActiveObjectIndex&& active_object_index,
                                info::ActiveID&& active_id);

    info::ID       _id;

    info::ActiveSetIndex _active_set_index;

    info::ActiveObjectIndex _active_object_index;

    info::ActiveID _active_id;

};


Properties         create_properties   (hdf5::Group& parent);

}  // namespace lue
