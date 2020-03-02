#include "lue/validate.hpp"
#include "lue/navigate.hpp"
#include "lue/hdf5/validate.hpp"
#include <algorithm>
// #include <execution> Not in gcc 7 yet
#include <numeric>
#include <sstream>


namespace lue {
namespace data_model {
namespace {

// void throw_not_supported_yet(
//     std::string const& message)
// {
//     throw std::runtime_error(fmt::format(
//         "This feature is not supported yet: {}\n"
//         "You may want to open an issue here: "
//         "https://github.com/pcraster/lue/issues",
//         message));
// }


void not_supported_yet(
    hdf5::Identifier const& id,
    hdf5::Issues& issues,
    std::string const& message)
{
    issues.add_warning(
        id,
        fmt::format(
            "validation of this feature is not supported yet: {} -- "
            "you may want to open an issue here: "
            "https://github.com/pcraster/lue/issues",
            message));
}


template<
    typename Property>
static bool is_collection_property(
    Property& property)
{
    return property_sets(property).id().name() == collection_property_sets_tag;
}

}  // Anonymous namespace


std::string message(
    hdf5::Issues const& issues)
{
    std::stringstream stream;

    // Aggregate all issues by id. Print issues per id.
    // The idea is to print important messages first.

    // Collection of ids, ordered by errors.
    std::vector<hdf5::Identifier> ids;

    // Mapping of id to messages.
    std::map<hdf5::Identifier, std::vector<std::string>> messages;

    // Handle all errors. These will be printed first.
    for(auto const& error: issues.errors()) {
        if(messages.find(error.id()) == messages.end()) {
            ids.push_back(error.id());
        }

        messages[error.id()].push_back("error: " + error.message());
    }

    // Handle all warnings. Warnings will be added to the list of messages
    // created above. For objects without errors, the warnings will be
    // stored last.
    for(auto const& warning: issues.warnings()) {
        if(messages.find(warning.id()) == messages.end()) {
            ids.push_back(warning.id());
        }

        messages[warning.id()].push_back("warning: " + warning.message());
    }


    // Print the issues. First the errors (and possibly warnings) are printed,
    // and after that the warnings for objects without errors.
    {
        std::size_t issue_count = 0;
        std::size_t const nr_issues =
            issues.errors().size() + issues.warnings().size();

        for(auto const& id: ids) {
            stream << fmt::format("{}:\n", id.pathname());

            for(auto const& message: messages[id]) {
                stream << fmt::format(
                        "- [{}/{}] {}\n",
                        ++issue_count, nr_issues, message
                    );
            }
        }
    }

    return stream.str();
}


static bool assert_ids_are_unique(
    hdf5::Identifier const& id,
    std::vector<ID> ids,
    hdf5::Issues& issues)
{
    std::sort(ids.begin(), ids.end());

    auto const it = std::adjacent_find(ids.begin(), ids.end());

    bool unique = it == ids.end();

    if(!unique) {
        issues.add_error(id, fmt::format(
            "Object IDs must be unique, but at least one duplicate was found "
            "({})",
            *it
        ));
    }

    return unique;
}


template<
    typename Collection>
bool assert_strictly_increasing(
    hdf5::Identifier const& id,
    Collection const& collection,
    hdf5::Issues& issues)
{
    using Value = typename Collection::value_type;

    // Find two adjacent values where the first value is larger or equal to
    // the second. In that case the collection is not strictly increasing.
    auto const it = std::adjacent_find(
        collection.begin(), collection.end(),
        [](Value const& lhs, Value const& rhs) {
            return lhs >= rhs;
        }
    );

    bool strictly_increasing = it == collection.end();

    if(!strictly_increasing) {
        issues.add_error(id, fmt::format(
            "Values must be strictly increasing, "
            "but at least two adjacent values where not "
            "({} >= {})",
            *it, *(it + 1)
        ));
    }

    return strictly_increasing;
}


template<
    typename Collection>
bool assert_increasing(
    hdf5::Identifier const& id,
    Collection const& collection,
    hdf5::Issues& issues)
{
    using Value = typename Collection::value_type;

    // Find two adjacent values where the first value is larger to
    // the second. In that case the collection is not increasing.
    auto const it = std::adjacent_find(
        collection.begin(), collection.end(),
        [](Value const& lhs, Value const& rhs) {
            return lhs > rhs;
        }
    );

    bool increasing = it == collection.end();

    if(!increasing) {
        issues.add_error(id, fmt::format(
            "Values must be (nonstrictly) increasing, "
            "but at least two adjacent values where not "
            "({} > {})",
            *it, *(it + 1)
        ));
    }

    return increasing;
}


static void validate_value(
    ObjectID const& object_id,
    same_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per object array, the object tracker contains an object ID.
    //   The number of object arrays must equal the number of IDs.

    auto const nr_objects = object_id.nr_objects();
    auto const nr_object_arrays = value.nr_arrays();

    if(nr_object_arrays != nr_objects) {
        issues.add_error(value.id(), fmt::format(
            "Number of object arrays in value array does not equal "
            "the number of IDs ({} != {})",
            nr_object_arrays, nr_objects
        ));
    }
}


static void validate_value(
    ObjectID const& object_id,
    different_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per object array, the object tracker contains an object ID.
    //   The number of object arrays (== number of value arrays) must
    //   equal the number of IDs.

    auto const nr_objects = object_id.nr_objects();
    auto const nr_object_arrays = value.nr_objects();

    if(nr_object_arrays != nr_objects) {
        issues.add_error(value.id(), fmt::format(
            "Number of object arrays in value does not equal "
            "the number of IDs ({} != {})",
            nr_object_arrays, nr_objects
        ));
    }
    else {
        // For each object ID stored in the object tracker, the value
        // must contain a values array
        std::vector<ID> ids(object_id.nr_objects());
        object_id.read(ids.data());

        for(auto const id: ids) {
            if(!value.contains(id)) {
                issues.add_error(value.id(), fmt::format(
                    "For at least one object, no value is stored (id={})",
                    id
                ));
            }
        }
    }
}


static void validate_value(
    ObjectTracker const& object_tracker,
    same_shape::constant_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per location in time, the object tracker contains information
    //   about which objects are active. The sum of the sizes of the
    //   these active sets must equal the number of object arrays.
    {
        auto const nr_active_objects = object_tracker.active_object_id().nr_ids();
        auto const nr_object_arrays = value.nr_arrays();

        if(nr_object_arrays != nr_active_objects) {
            issues.add_error(value.id(), fmt::format(
                "Number of object arrays in value array does not equal "
                "the number of active object IDs in object tracker "
                "({} != {})",
                nr_object_arrays, nr_active_objects
            ));
        }
    }
}


static void validate_value(
    ObjectTracker const& object_tracker,
    different_shape::constant_shape::Value& value,
    hdf5::Issues& issues)
{
    // For each object there is a single value array. An object's
    // value array contains an object array for each location in time
    // that the object was active.
    //
    // Object tracking is handle by
    // - active_set_index
    // - active_object_index
    // - active_object_id

    // - The number of value arrays must equal the size of the set of
    //   objects tracked
    // - For each object, there must be a value array
    // - For each active set of objects, there must be corresponding
    //   object arrays in the value arrays

    auto const nr_value_arrays = value.nr_objects();

    // Read IDs of all objects that have been active
    auto& active_object_id = object_tracker.active_object_id();

    std::vector<ID> active_object_ids(active_object_id.nr_ids());
    active_object_id.read(active_object_ids.data());

    // Determine the number of objects that have been active at least
    // once. *The set of unique_object_ids can be empty.*
    std::set<ID> unique_object_ids(
        active_object_ids.begin(), active_object_ids.end());
    auto const nr_objects = unique_object_ids.size();

    if(nr_value_arrays != nr_objects) {

        issues.add_error(value.id(), fmt::format(
            "Number of value arrays must be equal to the number of "
            "objects that have been active ({} != {})",
            nr_value_arrays, nr_objects
        ));

    }
    else if(
            // For each unique object ID, verify that a value array exists
            auto it = std::find_if(
                unique_object_ids.begin(), unique_object_ids.end(),
                [&](auto const id) {
                    return !value.exists(id);
                });
            it != unique_object_ids.end()) { // && !unique_object_ids.empty()) {

        issues.add_error(value.id(), fmt::format(
            "Each active object must have a corresponding value array, "
            "but at least value array for object with ID {} is missing",
            *it
        ));

    }
    else {

        // Iterate over each active set and verify that a value is
        // available for each object in the set
        auto& active_set_index = object_tracker.active_set_index();
        auto& active_object_index = object_tracker.active_object_index();

        std::vector<Index> active_set_idxs(active_set_index.nr_indices());
        active_set_index.read(active_set_idxs.data());

        std::vector<Index> active_object_idxs(active_object_index.nr_indices());
        active_object_index.read(active_object_idxs.data());

        // Add an end index to ease the iteration over ranges of IDs
        active_set_idxs.push_back(active_object_id.nr_ids());
        auto begin_idx = active_set_idxs[0];

        // TODO This happens when the active object idxs are empty. In
        // that case, validation of the object tracker succeeded, because
        // it doesn't now that this collection mustn't be empty, because
        // it doesn't know it is required. It is required in the case of
        // different_shape::constant_shape::Value. Not sure how to fix
        // this conveniently yet.
        assert(active_object_ids.size() == active_object_idxs.size());

        for(std::size_t i = 1; i < active_set_idxs.size(); ++i) {
            auto const end_idx = active_set_idxs[i];

            for(std::size_t o = begin_idx; o < end_idx; ++o) {
                assert(o < active_object_ids.size());
                assert(o < active_object_idxs.size());

                auto const object_id = active_object_ids[o];
                auto const object_array_idx = active_object_idxs[o];
                auto value_array = value[object_id];

                if(object_array_idx >= value_array.nr_arrays()) {
                    issues.add_error(value.id(), fmt::format(
                        "Number of object arrays stored in each value array "
                        "must equal the number of times the object is active "
                        "(at least value array of object with ID {} does not "
                        "have enough object arrays)",
                        object_id
                    ));

                    break;
                }
            }

            begin_idx = end_idx;
        }
    }
}


static void validate_value(
    ObjectTracker const& object_tracker,
    same_shape::variable_shape::Value& value,
    hdf5::Issues& issues)
{
    // - For each location in time, a value array is stored in the
    //   value. The number of these values arrays must be equal to the
    //   number of active sets stored in the object tracker.
    // - The size of each value array must equal the size of each active set.

    auto const& active_set_index = object_tracker.active_set_index();

    auto const nr_active_sets = active_set_index.nr_indices();
    auto const nr_value_arrays = value.nr_locations_in_time();

    if(nr_value_arrays != nr_active_sets) {
        issues.add_error(value.id(), fmt::format(
            "Number of value arrays in value does not equal "
            "the number of active sets in object tracker "
            "({} != {})",
            nr_value_arrays, nr_active_sets
        ));
    }
    else {

        // Iterate over each active set and verify that the size of the
        // value array equals the size of the set
        std::vector<Index> active_set_idxs(active_set_index.nr_indices());
        active_set_index.read(active_set_idxs.data());

        // Add an end index to ease the iteration over ranges of IDs
        active_set_idxs.push_back(object_tracker.active_object_id().nr_ids());
        auto begin_idx = active_set_idxs[0];

        for(std::size_t i = 1; i < active_set_idxs.size(); ++i) {
            auto const end_idx = active_set_idxs[i];
            auto const active_set_size = end_idx - begin_idx;

            auto const value_array = value[i - 1];
            auto const nr_object_arrays = value_array.nr_arrays();

            if(nr_object_arrays != active_set_size) {
                issues.add_error(value.id(), fmt::format(
                    "Number of object arrays stored does not equal "
                    "the size of the active set "
                    "({} != {})",
                    nr_object_arrays, active_set_size
                ));

                break;
            }

            begin_idx = end_idx;
        }
    }
}


static void validate_value(
    ObjectTracker const& object_tracker,
    different_shape::variable_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - For each location in time, a value array is stored in the
    //   value. The number of these values arrays must be equal to the
    //   number of active sets stored in the object tracker.

    auto const& active_set_index = object_tracker.active_set_index();

    auto const nr_active_sets = active_set_index.nr_indices();
    auto const nr_value_arrays = value.nr_locations_in_time();

    if(nr_value_arrays != nr_active_sets) {
        issues.add_error(value.id(), fmt::format(
            "Number of value arrays in value does not equal "
            "the number of active sets in object tracker "
            "({} != {})",
            nr_value_arrays, nr_active_sets
        ));
    }
    else {

        // Iterate over each active set and verify that the size of the
        // value array equals the size of the set
        std::vector<Index> active_set_idxs(active_set_index.nr_indices());
        active_set_index.read(active_set_idxs.data());

        // Add an end index to ease the iteration over ranges of IDs
        active_set_idxs.push_back(object_tracker.active_object_id().nr_ids());
        auto begin_idx = active_set_idxs[0];

        // FIXME Implement
        not_supported_yet(
            value.id(), issues,
            "validation of different_shape::variable_shape::Value");

        for(std::size_t i = 1; i < active_set_idxs.size(); ++i) {
            auto const end_idx = active_set_idxs[i];
            [[maybe_unused]] auto const active_set_size = end_idx - begin_idx;

        //     auto const value_array = value[i - 1];
        //     auto const nr_object_arrays = value_array.nr_arrays();

        //     if(nr_object_arrays != active_set_size) {
        //         issues.add_error(value.id(), fmt::format(
        //             "Number of object arrays stored does not equal "
        //             "the size of the active set "
        //             "({} != {})",
        //             nr_object_arrays, active_set_size
        //         ));

        //         break;
        //     }

            begin_idx = end_idx;
        }
    }
}


static void validate_id(
    hdf5::Identifier const& id,
    std::vector<ID> const& ids,
    hdf5::Issues& issues)
{
    assert_ids_are_unique(id, ids, issues);
}


static void validate_active_ids(
    hdf5::Identifier const& active_set_index_id,
    std::vector<Index> active_set_idxs,
    hdf5::Identifier const& active_object_id_id,
    std::vector<ID> const& active_object_ids,
    hdf5::Issues& issues)
{
    if(active_set_idxs.empty()) {
        if(!active_object_ids.empty()) {
            issues.add_error(active_set_index_id, fmt::format(
                "{} IDs of active objects are stored, "
                "but indices of active sets are missing",
                active_object_ids.size()
            ));
        }
    }
    else {
        // Actually, this is not an error. It is allowed to only
        // store empty active sets.
        // if(active_object_ids.empty()) {
        //     issues.add_error(active_set_index_id, fmt::format(
        //         "{} indices of active sets are stored, "
        //         "but IDs of active objects are missing",
        //         active_set_idxs.size()
        //     ));
        // }

        // Validate indices of active sets and IDs of active objects
        // Add an end index to ease the iteration over ranges of IDs
        active_set_idxs.push_back(active_object_ids.size());
        auto begin_idx = active_set_idxs[0];

        for(std::size_t i = 1; i < active_set_idxs.size(); ++i) {
            auto const end_idx = active_set_idxs[i];

            // Validate active set indices are valid
            if(end_idx < begin_idx) {
                issues.add_error(active_set_index_id, fmt::format(
                    "Start indices of active sets must be "
                    "monotonic increasing, but "
                    "next index {} < current index {}",
                    end_idx, begin_idx
                ));

                break;
            }

            // Validate active set is stored
            if(end_idx > active_object_ids.size()) {
                issues.add_error(active_set_index_id, fmt::format(
                    "Part of the collection of active object IDs is "
                    "missing (end index {} > collection size {})",
                    end_idx, active_object_ids.size()
                ));

                break;
            }

            // Validate active object IDs are valid
            if(!assert_ids_are_unique(
                    active_object_id_id,
                    std::vector<ID>(
                        active_object_ids.begin() + begin_idx,
                        active_object_ids.begin() + end_idx),
                    issues)) {
                break;
            }

            begin_idx = end_idx;
        }
    }
}


static void validate_active_object_idxs(
    hdf5::Identifier const& active_set_index_id,
    std::vector<Index> active_set_idxs,
    hdf5::Identifier const& active_object_index_id,
    std::vector<Index> const& active_object_idxs,
    hdf5::Issues& issues)
{
    if(active_set_idxs.empty()) {
        if(!active_object_idxs.empty()) {
            issues.add_error(active_set_index_id, fmt::format(
                "{} Indices of active objects are stored, "
                "but indices of active sets are missing",
                active_object_idxs.size()
            ));
        }
    }
    else if(!active_object_idxs.empty()) {
        // Validate indices of active sets and indices of active objects
        // Add an end index to ease the iteration over ranges of indices
        active_set_idxs.push_back(active_object_idxs.size());
        auto begin_idx = active_set_idxs[0];

        for(std::size_t i = 1; i < active_set_idxs.size(); ++i) {
            auto const end_idx = active_set_idxs[i];

            // Validate active set indices are valid
            if(end_idx < begin_idx) {
                issues.add_error(active_set_index_id, fmt::format(
                    "Start indices of active sets must be "
                    "monotonic increasing, but "
                    "next index {} < current index {}",
                    end_idx, begin_idx
                ));

                break;
            }

            // Validate active set is stored
            if(end_idx > active_object_idxs.size()) {
                issues.add_error(active_object_index_id, fmt::format(
                    "Part of the collection of active object indices is "
                    "missing (end index {} > collection size {})",
                    end_idx, active_object_idxs.size()
                ));

                break;
            }

            begin_idx = end_idx;
        }
    }
}


static void validate_object_tracker(
    ObjectID const& object_id,
    ObjectTracker const& object_tracker,
    hdf5::Issues& issues)
{
    // active set index
    // - for each active set an index
    // - the size equals the number of active sets
    //
    // active object id
    // - For each object in each active set an ID
    // - The size equals the sum of all active object in each active set.
    //     Active objects can be active in multiple active sets.
    //
    // active object index
    // - For each object in each active set an index into its value array.
    // - The size equals the sum of all active object in each active set.
    //     Active objects can be active in multiple active sets.

    std::vector<ID> ids(object_id.nr_objects());
    object_id.read(ids.data());

    auto const& active_set_index{object_tracker.active_set_index()};
    std::vector<Index> active_set_idxs(active_set_index.nr_indices());
    active_set_index.read(active_set_idxs.data());

    auto const& active_object_id{object_tracker.active_object_id()};
    std::vector<ID> active_object_ids(active_object_id.nr_ids());
    active_object_id.read(active_object_ids.data());

    auto const& active_object_index{object_tracker.active_object_index()};
    std::vector<Index> active_object_idxs(active_object_index.nr_indices());
    active_object_index.read(active_object_idxs.data());

    validate_id(object_id.id(), ids, issues);
    validate_active_ids(
        active_set_index.id(), active_set_idxs,
        active_object_id.id(), active_object_ids,
        issues);
    validate_active_object_idxs(
        active_set_index.id(), active_set_idxs,
        active_object_index.id(), active_object_idxs,
        issues);

    // If the collection of static IDs is not empty, all of the active
    // IDs must also be part of the collection of static IDs
    if(!ids.empty()) {
        for(auto id: active_object_ids) {
            if(std::find(ids.begin(), ids.end(), id) == ids.end()) {
                issues.add_error(active_object_id.id(), fmt::format(
                    "All active object IDs must also be part of the "
                    "collection of static object IDs "
                    "(at least {} is not)", id
                ));

                break;

            }
        }
    }
}


static void validate_time_box(
    ObjectTracker const& object_tracker,
    TimeBox const& time_box,
    hdf5::Issues& issues)
{
    auto const nr_boxes = time_box.nr_boxes();

    {
        std::vector<time::DurationCount> boxes(2 * nr_boxes);
        time_box.read(boxes.data());

        // TODO Actually, if we allow boxes to overlap, we should test the
        //      start coordinates and end coordinates of the boxes
        //      seperately. A start coordinate of a next box can be
        //      smaller than an end coordinate of a previous box.
        assert_increasing(time_box.id(), boxes, issues);
    }

    // Per location in time, the object tracker contains information
    // about which objects are active. The number of these active sets
    // must equal the number of boxes.
    {
        auto const nr_active_sets =
            object_tracker.active_set_index().nr_indices();

        if(nr_boxes != nr_active_sets) {
            issues.add_error(time_box.id(), fmt::format(
                "Number of boxes in time domain does not equal "
                "the number of active set indices in object tracker "
                "({} != {})",
                nr_boxes, nr_active_sets
            ));
        }
    }
}


static void validate_time_cell(
    ObjectTracker const& object_tracker,
    TimeCell const& time_cell,
    hdf5::Issues& issues)
{
    auto const nr_boxes = time_cell.nr_boxes();
    auto const nr_counts = time_cell.nr_counts();

    // The coordinates of the time boxes must be strictly increasing
    {
        std::vector<time::DurationCount> boxes(2 * nr_boxes);
        time_cell.read(boxes.data());

        // TODO Actually, if we allow boxes to overlap, we should test the
        //      start coordinates and end coordinates of the boxes
        //      seperately. A start coordinate of a next box can be
        //      smaller than an end coordinate of a previous box.
        assert_increasing(time_cell.id(), boxes, issues);
    }

    // The number of counts must equal the number of time boxes
    if(nr_counts != nr_boxes) {
        issues.add_error(time_cell.id(), fmt::format(
            "For each box a count must be written, but "
            "the number of counts does not equal the number of boxes "
            "({} != {})",
            nr_counts, nr_boxes
        ));
    }

    // Per location in time, the object tracker contains information
    // about which objects are active. The number of these active sets
    // must equal the number of cells.
    else {
        std::vector<Count> count(nr_boxes);
        time_cell.count().read(count.data());

        // Not in gcc 7 yet
        // auto nr_cells = std::reduce(
        //     std::execution::par, count.begin(), count.end());
        auto const nr_cells = std::accumulate(
            count.begin(), count.end(), Count{0});
        auto const nr_active_sets =
            object_tracker.active_set_index().nr_indices();

        if(nr_cells != nr_active_sets) {
            issues.add_error(time_cell.id(), fmt::format(
                "Number of cells in time domain does not equal "
                "the number of active set indices in object tracker "
                "({} != {})",
                nr_cells, nr_active_sets
            ));
        }
    }
}


static void validate_time_point(
    ObjectTracker const& object_tracker,
    TimePoint const& time_point,
    hdf5::Issues& issues)
{
    {
        std::vector<time::DurationCount> points(time_point.nr_points());
        time_point.read(points.data());

        assert_increasing(time_point.id(), points, issues);
    }

    // - Per location in time, the object tracker contains information
    //   about which objects are active. The number of these active sets
    //   must equal the number of object arrays.
    {
        auto const nr_active_sets =
            object_tracker.active_set_index().nr_indices();
        auto const nr_points = time_point.nr_points();

        if(nr_points != nr_active_sets) {
            issues.add_error(time_point.id(), fmt::format(
                "Number of points in time domain does not equal "
                "the number of active set indices in object tracker "
                "({} != {})",
                nr_points, nr_active_sets
            ));
        }
    }
}


static void validate(
    ObjectTracker const& object_tracker,
    TimeDomain& time_domain,
    hdf5::Issues& issues)
{
    auto const configuration = time_domain.configuration();

    switch(configuration.value<TimeDomainItemType>()) {
        case TimeDomainItemType::box: {
            validate_time_box(
                object_tracker, time_domain.value<TimeBox>(), issues);
            break;
        }
        case TimeDomainItemType::cell: {
            validate_time_cell(
                object_tracker, time_domain.value<TimeCell>(), issues);
            break;
        }
        case TimeDomainItemType::point: {
            validate_time_point(
                object_tracker, time_domain.value<TimePoint>(), issues);
            break;
        }
    }
}


static void validate(
    ObjectID const& object_id,
    StationarySpaceBox const& space_box,
    hdf5::Issues& issues)
{
    // A stationary space box is a same_shape::Value
    using Value = same_shape::Value;

    validate_value(object_id, dynamic_cast<Value const&>(space_box), issues);
}


static void validate(
    ObjectID const& object_id,
    StationarySpacePoint const& space_point,
    hdf5::Issues& issues)
{
    // A stationary space point is a same_shape::Value
    using Value = same_shape::Value;

    validate_value(object_id, dynamic_cast<Value const&>(space_point), issues);
}


static void validate(
    ObjectTracker const& object_tracker,
    MobileSpaceBox const& space_box,
    hdf5::Issues& issues)
{
    // A mobile space box is a same_shape::constant_shape::Value
    using Value = same_shape::constant_shape::Value;

    validate_value(
        object_tracker, dynamic_cast<Value const&>(space_box), issues);
}


static void validate(
    ObjectTracker const& object_tracker,
    MobileSpacePoint const& space_point,
    hdf5::Issues& issues)
{
    // A mobile space point is a same_shape::constant_shape::Value
    using Value = same_shape::constant_shape::Value;

    validate_value(
        object_tracker, dynamic_cast<Value const&>(space_point), issues);
}


static void validate(
    ObjectID const& object_id,
    ObjectTracker const& object_tracker,
    SpaceDomain& space_domain,
    hdf5::Issues& issues)
{
    auto const configuration = space_domain.configuration();
    auto const mobility = configuration.value<Mobility>();
    auto const item_type = configuration.value<SpaceDomainItemType>();

    switch(mobility) {
        case Mobility::mobile: {
            switch(item_type) {
                case SpaceDomainItemType::box: {
                    validate(
                        object_tracker,
                        space_domain.value<MobileSpaceBox>(), issues);
                    break;
                }
                case SpaceDomainItemType::point: {
                    validate(
                        object_tracker,
                        space_domain.value<MobileSpacePoint>(), issues);
                    break;
                }
            }

            break;
        }
        case Mobility::stationary: {
            switch(item_type) {
                case SpaceDomainItemType::box: {
                    validate(
                        object_id,
                        space_domain.value<StationarySpaceBox>(), issues);
                    break;
                }
                case SpaceDomainItemType::point: {
                    validate(
                        object_id,
                        space_domain.value<StationarySpacePoint>(), issues);
                    break;
                }
            }

            break;
        }
    }
}


template<
    typename DiscretizedProperty>
void validate_space_constant_regular_grid(
    DiscretizedProperty const& discretized_property,
    same_shape::Property const& discretization_property,
    hdf5::Issues& issues)
{
    auto const& discretized_property_value = discretized_property.value();
    auto const& discretization_property_value =
        discretization_property.value();

    // The discretization property must contain a value for each object
    // array in the discretized property
    if(discretization_property_value.nr_arrays() !=
            discretized_property_value.nr_objects()) {
        issues.add_error(
                discretization_property.id(),
                fmt::format(
            "Number of object arrays in discretization property "
            "must equal the number of object arrays discretized "
            "({} != {})",
            discretization_property_value.nr_arrays(),
            discretized_property_value.nr_objects()));
    }
}


template<>
void validate_space_constant_regular_grid<same_shape::constant_shape::Property>(
    same_shape::constant_shape::Property const& /* discretized_property */,
    same_shape::Property const& discretization_property,
    hdf5::Issues& issues)
{
    auto const& discretization_property_value =
        discretization_property.value();

    // The discretization property must contain a single value that is valid
    // for all object arrays. They all have the same shape which is
    // constant.
    if(discretization_property_value.nr_arrays() != 1) {
        issues.add_error(
                discretization_property.id(),
                fmt::format(
            "When discretizing a same_shape::constant_shape property "
            "the number of object arrays in the discretization property must "
            "equal 1 "
            "({} != 1)",
            discretization_property_value.nr_arrays()));
    }
}


template<
    typename DiscretizedProperty>
void validate_space_constant_regular_grid(
    DiscretizedProperty const& discretized_property,
    Properties& properties,
    std::string const& discretization_property_name,
    hdf5::Issues& issues)
{
    // For each dimension, the discretization property value contains
    // a count

    assert(
        properties.value_variability(discretization_property_name) ==
        ValueVariability::constant);

    if(properties.shape_per_object(discretization_property_name) !=
            ShapePerObject::same) {
        issues.add_error(discretized_property.id(),
            "Shape of object arrays of regular grid must be "
            "the same for each object");
    }
    else {
        auto const& property_value = discretized_property.value();
        auto& discretization_property =
            properties.collection<same_shape::Properties>()[
                discretization_property_name];
        auto const& discretization_value = discretization_property.value();

        // Validate the counts -------------------------------------------------

        // For each object array in the discretized property, the
        // discretization property must contain enough information
        // - In case discretization property is a collection property,
        //     it contains information for a single object (the collection
        //     as a whole). The information must be valid for all objects
        //     in the discretized property.
        // - In case the discretization property is not a collection property,
        //     it contains information for each object in the discretized
        //     property.

        if(is_collection_property(discretization_property)) {
            // The discretization property must contain a single value
            // that is valid for all object arrays in the discretized
            // property
            if(discretization_value.nr_arrays() != 1) {
                issues.add_error(
                        discretization_property.id(),
                        fmt::format(
                    "Number of object arrays in discretization property "
                    "must be 1 ({} != 1)",
                    discretization_value.nr_arrays()));
            }
        }
        else {
            validate_space_constant_regular_grid(
                discretized_property, discretization_property, issues);
        }

        // The discretization property's value must contain unsigned integers
        if(!hdf5::is_native_unsigned_integral(
                discretization_value.memory_datatype())) {
            issues.add_error(
                    discretization_property.id(),
                    fmt::format(
                "Discretization property must contain unsigned "
                "integral values "
                "({} is not unsigned integral)",
                hdf5::native_datatype_as_string(
                    discretization_value.memory_datatype())));
        }

        // The discretization property's value must contain object arrays
        // with a 1D rank and shape (rank_of_space)
        if(discretization_value.array_shape() !=
                hdf5::Shape{
                    static_cast<Count>(property_value.rank())}) {

            // FIXME What about properties discretized in both space
            //     and time? In that case there are multiple
            //     discretization properties.
            //     This error is thrown when storing a stack of rasters
            //     discretized both in time and space. False negative.
            // issues.add_error(
            //         discretization_property.id(),
            //     "For each spatial dimension, the discretization "
            //     "property must contain a count");
        }

        // TODO Compare the counts in the discretization property. These
        // must match the shapes of the values.
    }
}


static void validate_property(
    ObjectID const& object_id,
    same_shape::Property& property,
    hdf5::Issues& issues)
{
    validate_value(object_id, property.value(), issues);

    if(property.time_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through time");
    }

    if(property.space_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through space");
    }
}


static void validate_property(
    ObjectID const& object_id,
    different_shape::Property& property,
    hdf5::Issues& issues)
{
    auto const& property_value = property.value();

    validate_value(object_id, property_value, issues);

    if(property.time_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through time");
    }

    if(property.space_is_discretized()) {
        // Each property value is discretized through space
        auto discretization_property =
            property.space_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        auto discretization_property_set{
            property_set(discretization_property)};
        auto& properties = discretization_property_set.properties();

        if(properties.value_variability(discretization_property_name) !=
                ValueVariability::constant) {
            issues.add_error(discretization_property.id(),
                "Property values for discretization of constant "
                "property values must be constant themselves");
        }
        else {
            switch(property.space_discretization_type()) {
                case SpaceDiscretization::regular_grid: {
                    validate_space_constant_regular_grid(
                        property,
                        properties, discretization_property_name,
                        issues);

                    break;
                }
            }
        }
    }
}


static void validate_property(
    ObjectTracker const& object_tracker,
    same_shape::constant_shape::Property& property,
    hdf5::Issues& issues)
{
    validate_value(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through time");
    }

    if(property.space_is_discretized()) {
        // Each property value is discretized through space
        auto discretization_property =
            property.space_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        auto discretization_property_set{
            property_set(discretization_property)};
        auto& properties = discretization_property_set.properties();

        // Discretization property can be constant or variable
        auto const value_variability =
            properties.value_variability(discretization_property_name);

        if(value_variability == ValueVariability::constant) {
            switch(property.space_discretization_type()) {
                case SpaceDiscretization::regular_grid: {
                    validate_space_constant_regular_grid(
                        property,
                        properties, discretization_property_name,
                        issues);

                    break;
                }
            }
        }
        else {
            not_supported_yet(
                discretization_property.id(), issues,
                "validation of variable discretization through space");
        }
    }
}


static void validate_property(
    ObjectTracker const& object_tracker,
    different_shape::constant_shape::Property& property,
    hdf5::Issues& issues)
{
    validate_value(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through time");
    }

    if(property.space_is_discretized()) {
        auto discretization_property = property.space_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        auto discretization_property_set{
            property_set(discretization_property)};
        auto& properties = discretization_property_set.properties();

        if(properties.value_variability(discretization_property_name) !=
                ValueVariability::constant) {
            issues.add_error(discretization_property.id(),
                "Property values for discretization of constant "
                "shaped values must be constant themselves");
        }
        else {
            switch(property.space_discretization_type()) {
                case SpaceDiscretization::regular_grid: {
                    validate_space_constant_regular_grid(
                        property,
                        properties, discretization_property_name,
                        issues);

                    break;
                }
            }
        }
    }
}


static void validate_property(
    ObjectTracker const& object_tracker,
    same_shape::variable_shape::Property& property,
    hdf5::Issues& issues)
{
    auto& property_value = property.value();

    validate_value(object_tracker, property_value, issues);

    if(property.time_is_discretized()) {

        // Each property value is discretized through time
        auto discretization_property = property.time_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        auto discretization_property_set{
            property_set(discretization_property)};
        auto const& properties = discretization_property_set.properties();
        bool values_can_be_tested = false;


        if(properties.value_variability(discretization_property_name) !=
                ValueVariability::variable) {
            issues.add_error(discretization_property.id(),
                "Property values for discretization of variable "
                "property values must be variable themselves");
            values_can_be_tested = false;
        }

        switch(property.time_discretization_type()) {
            case TimeDiscretization::regular_grid: {
                // Discretization property must be a collection property
                // (nr objects == 1). For each location in time,
                // the discretization property contains a count
                // (same_shape::variable_shape::Value, unsigned integral).
                // The number of counts in the discretization property
                // must match the number of locations in time in the
                // discretized property.

                if(properties.shape_per_object(discretization_property_name) !=
                        ShapePerObject::same) {
                    issues.add_error(property.id(),
                        "Property values of regular grid must be "
                        "the same for each object");
                    values_can_be_tested = false;
                }

                if(properties.shape_variability(discretization_property_name) !=
                        ShapeVariability::constant) {
                    issues.add_error(property.id(),
                        "Shape of property values of regular grid must be "
                        "constant through time");
                    values_can_be_tested = false;
                }

                if(values_can_be_tested) {
                    auto const& discretization_value =
                        properties.collection<
                            same_shape::constant_shape::Properties>()[
                                discretization_property_name].value();

                    if(discretization_value.nr_arrays() !=
                            property_value.nr_locations_in_time()) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Number of counts in discretization property "
                            "must equal the number of locations in time "
                            "for which values are discretized "
                            "({} != {})",
                            discretization_value.nr_arrays(),
                            property_value.nr_locations_in_time()));
                    }

                    if(!hdf5::is_native_unsigned_integral(
                            discretization_value.memory_datatype())) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Discretization property must contain unsigned "
                            "integral values "
                            "({} is not unsigned integral)",
                            hdf5::native_datatype_as_string(
                                discretization_value.memory_datatype())));
                    }

                    if(discretization_value.array_shape().size() != 1) {
                        issues.add_error(
                            discretization_property.id(), fmt::format(
                                "The rank of the discretization property "
                                "values must be 1, but it currently is {}",
                                discretization_value.array_shape().size()));
                    }
                    else if(discretization_value.array_shape()[0] !=
                            property_value.nr_locations_in_time()) {
                        issues.add_error(
                            discretization_property.id(), fmt::format(
                                "The number of counts in the discretization "
                                "property must equal the number of locations "
                                "in time in the discretized property "
                                "({} != {})",
                                discretization_value.array_shape()[0],
                                property_value.nr_locations_in_time()));

                    }
                }

                break;
            }
        }
    }

    if(property.space_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through space");
    }
}


static void validate_property(
    ObjectTracker const& object_tracker,
    different_shape::variable_shape::Property& property,
    hdf5::Issues& issues)
{
    validate_value(object_tracker, property.value(), issues);

    // TODO
    if(property.time_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through time");
    }

    // TODO
    if(property.space_is_discretized()) {
        not_supported_yet(
            property.id(), issues,
            "validation of discretization through space");
    }
}


static void validate_properties(
    ObjectID const& object_id,
    ObjectTracker const& object_tracker,
    Properties& properties,
    hdf5::Issues& issues)
{
    for(auto& property:
            properties.collection<same_shape::Properties>()) {
        validate_property(object_id, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::Properties>()) {
        validate_property(object_id, property.second, issues);
    }

    for(auto& property: properties.collection<
            same_shape::constant_shape::Properties>()) {
        validate_property(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::constant_shape::Properties>()) {
        validate_property(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            same_shape::variable_shape::Properties>()) {
        validate_property(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::variable_shape::Properties>()) {
        validate_property(object_tracker, property.second, issues);
    }
}


static void validate_property_set(
    ObjectID const& object_id,
    PropertySet& property_set,
    hdf5::Issues& issues)
{
    auto const& object_tracker = property_set.object_tracker();
    auto nr_errors = issues.errors().size();
    validate_object_tracker(object_id, object_tracker, issues);
    bool const object_tracker_is_valid = issues.errors().size() == nr_errors;

    if(object_tracker_is_valid) {

        if(property_set.has_time_domain()) {
            validate(object_tracker, property_set.time_domain(), issues);
        }

        if(property_set.has_space_domain()) {
            auto& space_domain = property_set.space_domain();
            validate(object_id, object_tracker, space_domain, issues);

            if(space_domain.presence_is_discretized()) {
                auto const configuration = space_domain.configuration();
                auto const mobility = configuration.value<Mobility>();
                auto const item_type =
                    configuration.value<SpaceDomainItemType>();

                if(item_type != SpaceDomainItemType::box) {
                    issues.add_error(space_domain.id(), fmt::format(
                        "Discretization of presence in space is only supported "
                        "for space domain item type {}, but it is {}",
                        aspect_to_string(SpaceDomainItemType::box),
                        aspect_to_string(item_type)
                    ));
                }
                else {
                    auto presence_property =
                        space_domain.discretized_presence_property();
                    auto const& presence_property_name =
                        presence_property.name();
                    auto presence_property_set{
                        data_model::property_set(presence_property)};
                    auto const& properties =
                        presence_property_set.properties();

                    switch(mobility) {
                        case Mobility::mobile: {
                            not_supported_yet(
                                space_domain.id(), issues,
                                "validation of discretization of presence of "
                                "mobile space domain items");

                            break;
                        }
                        case Mobility::stationary: {
                            // Stationary space boxes with discretized presence
                            switch(properties.value_variability(
                                    presence_property_name)) {
                                case ValueVariability::constant: {
                                    not_supported_yet(
                                        space_domain.id(), issues,
                                        "validation of discretization of presence of "
                                        "stationary space domain items with constant "
                                        "presence");

                                    break;
                                }
                                case ValueVariability::variable: {
                                    // Presence property is
                                    // temporal. Verify its time domain
                                    // equals the time domain of the space
                                    // domain's property set
                                    if(
                                            presence_property_set.has_time_domain() &&
                                            property_set.has_time_domain() &&
                                            presence_property_set.time_domain().id() !=
                                            property_set.time_domain().id()) {

                                        issues.add_error(presence_property.id(), fmt::format(
                                            "Time domain of presence property "
                                            "must equal the time domain of the "
                                            "property-set containing the "
                                            "space domain ({} != {})",
                                            presence_property_set.time_domain().id().pathname(),
                                            property_set.time_domain().id().pathname()
                                        ));
                                    }

                                    // TODO The presence property's value must contain
                                    // unsigned integers (= convertable to boolean)

                                    break;
                                }
                            }

                            break;
                        }
                    }
                }
            }
        }

        validate_properties(
            object_id, object_tracker, property_set.properties(), issues);
    }
}


static void validate_phenomenon(
    Phenomenon& phenomenon,
    hdf5::Issues& issues)
{
    // Validate property sets
    auto& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        auto& property_set = property_sets[name];
        validate_property_set(phenomenon.object_id(), property_set, issues);
    }

    // Validate collection property sets
    auto& collection_property_sets = phenomenon.collection_property_sets();

    for(auto const& name: collection_property_sets.names()) {
        auto& property_set = collection_property_sets[name];

        // Collection property-sets don't need the object-id
        // collection. For non-temporal information there will be only one
        // ID stored anyway, representing the collection as a whole.
        // To be able to validate property sets in a uniform way, whether
        // or not they are collection property sets or regular ones,
        // we create an ad-hoc ObjectID collection here, containing a
        // single ID.

        auto tmp_file = hdf5::create_in_memory_file("tmp_file");
        auto tmp_object_id = create_object_id(tmp_file);
        tmp_object_id.expand(1);
        ID id{999};

        {
            // If the object tracker contains an ID, it must be used here.
            // Otherwise validation fails because of different IDs
            // representing the collection as a whole.
            auto const& object_tracker = property_set.object_tracker();
            auto const& active_object_id = object_tracker.active_object_id();

            if(active_object_id.nr_ids() > 0) {
                active_object_id.read(0, &id);
            }
        }

        tmp_object_id.write(&id);

        validate_property_set(tmp_object_id, property_set, issues);

        auto& properties = property_set.properties();

        // Assert size of collection in each collection property set is 1
        auto const& object_tracker = property_set.object_tracker();

        {
            std::vector<bool> check_ids;

            {
                using Properties = same_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_ids.push_back(!collection.empty());
            }

            {
                using Properties = different_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_ids.push_back(!collection.empty());
            }

            // TODO Somehow verify that the object tracker of the
            //   collection properties tracks only a single object (the
            //   collection as a whole). The properties must store a
            //   single value.

            // if(std::any_of(check_ids.begin(), check_ids.end(),
            //         [](bool const b) { return b; })) {

            //     auto const nr_objects = object_tracker.id().nr_objects();

            //     if(nr_objects != 1) {
            //         issues.add_error(object_tracker.id().id(), fmt::format(
            //             "Number of object IDs in object tracker "
            //             "of collection properties does not equal 1 "
            //             "({} != 1)",
            //             nr_objects
            //         ));

            //     }
            // }
        }

        {
            std::vector<bool> check_active_sets;
            bool check_active_object_indices = false;

            {
                using Properties = same_shape::constant_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
            }

            {
                using Properties = same_shape::variable_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
            }

            {
                using Properties = different_shape::constant_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
                check_active_object_indices = !collection.empty();
            }

            if(std::any_of(check_active_sets.begin(), check_active_sets.end(),
                    [](bool const b) { return b; })) {

                // In case the number of objects is one, as it should, than
                // the number of active sets equals the number of ids
                auto const nr_active_sets =
                    object_tracker.active_set_index().nr_arrays();
                auto const nr_objects1 =
                    object_tracker.active_object_id().nr_ids();

                if(nr_active_sets != nr_objects1) {
                    issues.add_error(property_set.id(), fmt::format(
                        "Size of each active set in object tracker "
                        "of collection properties does not equal "
                        "the number of active object IDs "
                        "({} != {})",
                        nr_active_sets, nr_objects1
                    ));
                }

                if(check_active_object_indices) {
                    auto const nr_objects2 =
                        object_tracker.active_object_id().nr_ids();

                    if(nr_active_sets != nr_objects2) {
                        issues.add_error(property_set.id(), fmt::format(
                            "Size of each active set in object tracker "
                            "of collection properties does not equal "
                            "the number of active object indices "
                            "({} != {})",
                            nr_active_sets, nr_objects2
                        ));
                    }
                }
            }
        }
    }
}


static void validate(
    Universe& universe,
    hdf5::Issues& issues)
{
    auto& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        validate_phenomenon(phenomena[name], issues);
    }
}


void validate(
    Dataset const& dataset,
    hdf5::Issues& issues)
{
    // FIXME Push const cast down
    auto& universes = const_cast<Dataset&>(dataset).universes();

    for(auto const& name: universes.names()) {
        validate(universes[name], issues);
    }


    // FIXME Push const cast down
    auto& phenomena = const_cast<Dataset&>(dataset).phenomena();

    for(auto const& name: phenomena.names()) {
        validate_phenomenon(phenomena[name], issues);
    }
}


/*!
    @brief      Validate whether @a file contains a LUE dataset and report
                any @a issues
*/
void validate(
    hdf5::File const& file,
    hdf5::Issues& issues)
{
    // TODO
    // - test version attribute (error)
    // - test history attribute (warning)

    // TODO
    //     history attribute on all levels, maybe with only information
    //     about the contained information?
    // std::vector<std::string> expected_attribute_names{
    //     history_tag,
    //     version_tag,
    // };
    // assert_attributes_exist(file, expected_attribute_names, issues);


    // std::vector<std::string> expected_group_names{
    //     phenomena_tag,
    //     universes_tag,
    // };
    // auto const groups_exist = hdf5::assert_groups_exist(
    //     file, expected_group_names, issues);

    // if(groups_exist) {
        // Catching exceptions, because we start using the lue
        // API. Exceptions may be thrown when expected HDF5 objects are
        // not found.
        try {
            Dataset dataset{file.pathname()};
            validate(dataset, issues);
        }
        catch(std::exception const& exception) {
            issues.add_error(file.id(), exception.what());
        }

    // }



    // std::vector<std::string> expected_object_names = expected_group_names;
    // hdf5::assert_no_superfluous_objects_exist(
    //     file, expected_object_names, issues);






    // // if(file.contains_group("lue_universes")) {
    // //     validate_universes(hdf5::open_group(file, "lue_universes"), issues);
    // // }


    // // if(file.contains_group("lue_phenomena")) {
    // //     validate_phenomena(hdf5::open_group(file, "lue_phenomena"), issues);
    // // }




    // // std::vector<std::string> expected_object_names = expected_group_names;
    // // assert_no_superfluous_objects_exist(file, expected_object_names, issues);


    // // auto const& universes = dataset.universes();

    // // for(auto const& name: universes.names()) {
    // //     validate(universes[name], issues);
    // // }

    // // auto const& phenomena = dataset.phenomena();

    // // for(auto const& name: phenomena.names()) {
    // //     validate(phenomena[name], issues);
    // // }
}


/*!
    @brief      Validate file named @a pathname
    @param      issues Collection of issues found
*/
void validate(
    std::string const& pathname,
    hdf5::Issues& issues)
{
    validate(hdf5::File{pathname}, issues);
}


void assert_is_valid(
    Dataset const& dataset,
    bool const fail_on_warning)
{
    hdf5::Issues issues;

    validate(dataset, issues);

    if(issues.errors_found() || (fail_on_warning && issues.warnings_found())) {
        throw std::runtime_error(fmt::format(
            "{}"
            "LUE is work in progress -- if you encounter "
            "false-negatives, then please open an issue here: "
            "https://github.com/pcraster/lue/issues",
            message(issues)
        ));
    }
}


/*!
    @brief      Assert that the dataset in @a file is valid
    @exception  std::runtime_error In case the dataset is not valid
*/
void assert_is_valid(
    hdf5::File const& file,
    bool const fail_on_warning)
{
    hdf5::Issues issues;

    validate(file, issues);

    if(issues.errors_found() || (fail_on_warning && issues.warnings_found())) {
        throw std::runtime_error(fmt::format(
            "{}"
            "LUE is work in progress -- if you encounter "
            "false-negatives, then please open an issue here: "
            "https://github.com/pcraster/lue/issues",
            message(issues)
        ));
    }
}


/*!
    @brief      Assert that the dataset in @a dataset_name is valid
    @exception  std::runtime_error In case the dataset is not valid
*/
void assert_is_valid(
    std::string const& dataset_name,
    bool const fail_on_warning)
{
    assert_is_valid(hdf5::File(dataset_name), fail_on_warning);
}

}  // namespace data_model
}  // namespace lue
