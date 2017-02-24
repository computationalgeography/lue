#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif

/*!
    @ingroup    lue_c_api_group
    @brief      Type for storing an item-id
*/
typedef hsize_t lue_item_t;

/*!
    @ingroup    lue_c_api_group
    @brief      Type-id of in-file representation of an item-id
*/
#define LUE_STD_ITEM H5T_STD_U64LE

/*!
    @ingroup    lue_c_api_group
    @brief      Type-id of in-memory representation of an item-id
*/
#define LUE_NATIVE_ITEM H5T_NATIVE_HSIZE


/// typedef enum
/// {
/// 
///     omnipresent_in_time,
/// 
///     /*!
///         @brief      For each item a set of time points.
/// 
///         Each point represents a location in time where
///         - a space domain item is available,
///         - for each property in the set a property value is available.
/// 
///         Values are not available at time locations between the points.
/// 
///         This type is for time discrete events happening at specific
///         locations in time (time point objects).
/// 
///         Examples:
///         - For each traffic light in a city, the time location of light
///           color changes.
///         - For various space locations along the world's coasts, time
///           locations of high tides.
///     */
///     time_point,
/// 
///     /*!
///         @brief      For each item a set of time regions (periods).
/// 
///         Each period represents a region in time where
///         - a space domain item is available,
///         - for each property in the set a property value is available.
/// 
///         Values are not available at time locations between the regions.
/// 
///         This type is for time discrete events happening at specific
///         moments in time but having a time extent (duration, time
///         region objects).
/// 
///         Examples:
///         - For each traffic light in a city, the time regions that the color
///           is green.
///         - For various space locations along the world's coasts, time
///           regions of the top half of the tidal range.
///     */
///     time_period,
/// 
///     /*!
///         @brief      For each item a time discretization (grid).
/// 
///         Each time cell containing a true value represents a region in
///         time where
///         - a space domain item is available,
///         - for each property in the set a property value is available.
/// 
///         This type is for space domains and/or property values that vary
///         continuously through time (temporal fields).
///     */
///     time_cell
/// 
/// } TimeDomainTypeId;
/// 
/// 
/// typedef enum
/// {
/// 
///     omnipresent_in_space,
/// 
///     /*!
///         @brief      For each item a set of spatial points.
/// 
///         Each point represents a location in space where for each property
///         in the set a property value is available.
/// 
///         Values are not available at space locations between the points.
/// 
///         This type is for spatial discrete events happening at specific
///         locations in space (spatial point objects).
/// 
///         Examples:
///         - For each traffic light in a city, the space location.
///         - For various space locations along the world's coasts,
///           the location.
///     */
///     space_point,
/// 
///     space_box,
/// 
///     /*!
///         @brief      For each item a set of spatial regions (areas).
/// 
///         Each area represents a region in space where
///         - for each property in the set a property value is available.
/// 
///         Values are not available at space locations between the regions.
/// 
///         This type is for spatial discrete events happening at specific
///         points in space but having a space extent (area, spatial region
///         objects).
/// 
///         Examples:
///         - For each country, the area it occupies.
///         - For each house in a city, the area it occupies.
///     */
///     space_region,
/// 
///     /*!
///         @brief      For each item a space discretization (grid).
/// 
///         Each space cell containing a true value represents a region in
///         space where
///         - for each property in the set a property value is available.
/// 
///         This type is for property values that vary continuously through
///         space (spatial fields).
/// 
///         The grid may be a boolean grid containing a true value in those cells
///         that belong to the space domain of the item. In this case, the grid
///         is a clone grid and values are only stored for those cells that are
///         included in the domain.
/// 
///         The grid may also be specified such that for each cell in the grid
///         a value is available, but this value might be no-data to signal
///         that at certain locations in the grid a valid value is missing.
/// 
///         Notes:
///         - A 2D flat grid is modeled in the domain using a single bounding
///           box and a discretized value. The outlines of the grid are stored
///           in the domain, and the number of cells and the (constant!) cell
///           size in the value.
/// 
///         Examples:
///         - For each country, the area it occupies.
///         - For each country, its height, which varies spatially within the
///           field.
///         - For each house in a city, the area it occupies.
///     */
///     space_cell
/// 
/// } SpaceDomainTypeId;
/// 
/// 
/// typedef enum
/// {
/// 
///     not_discretized_in_time
/// 
/// } TimeDiscretizationTypeId;
/// 
/// 
/// typedef enum
/// {
/// 
///     not_discretized_in_space,
/// 
///     /*!
///         @brief  {1,2,3}D Euclidian space.
/// 
///         Cells have equal sizes in each dimension.
///     */
///     cartesian_grid,
/// 
///     // /*!
///     //     @brief  {1,2,3}D Euclidian space.
/// 
///     //     Cells have different sizes in each dimension.
///     // */
///     // rectilinear_grid,
/// 
///     // curvilinear_grid,
/// 
/// } SpaceDiscretizationTypeId;
/// 
/// 
/// // typedef enum
/// // {
/// // 
/// //     universe
/// // 
/// // } DatasetTypeId;
/// 
/// 
/// enum { rank_1 = 1 };
/// 
/// 
/// typedef enum {
/// 
///     shared=0,  // See TimeDomainConfiguration...
/// 
///     not_shared
/// 
/// } DomainSharingType;
/// 
/// 
/// typedef struct {
/// 
///     TimeDomainTypeId type;
/// 
///     // Keep this sharing field as last. If not initialized explicitly,
///     // it will be default initialized to zero (shared), which is
///     // what we want in case type is omnipresent.
///     DomainSharingType sharing;
/// 
/// } TimeDomainConfiguration;
/// 
/// 
/// typedef struct {
/// 
///     SpaceDomainTypeId type;
/// 
///     // hid_t memory_coordinate_type;
/// 
///     hid_t /* file_ */ coordinate_datatype;
/// 
///     uint32_t rank;
/// 
/// } SpaceDomainConfiguration;
/// 
/// 
/// typedef struct {
/// 
///     TimeDomainConfiguration time;
/// 
///     SpaceDomainConfiguration space;
/// 
/// } DomainConfiguration;
/// 
/// 
/// // Just something we don't use.
/// #define LUE_UNUSED_DATATYPE H5T_STD_REF_DSETREG
/// #define LUE_UNUSED_RANK 99999
/// 
/// 
/// typedef struct {
/// 
///     TimeDiscretizationTypeId type;
/// 
/// } TimeDiscretizationConfiguration;
/// 
/// 
/// typedef struct {
/// 
///     SpaceDiscretizationTypeId type;
/// 
///     uint32_t rank;
/// 
/// } SpaceDiscretizationConfiguration;
/// 
/// 
/// typedef struct {
/// 
///     TimeDiscretizationConfiguration time;
/// 
///     SpaceDiscretizationConfiguration space;
/// 
/// } DiscretizationConfiguration;


#ifdef __cplusplus
}  // extern "C"
#endif
