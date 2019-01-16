#include "lue/framework/core/partitioned_array.hpp"
// #include "raster_partition.hpp"


namespace lue {

// template<
//     typename T>
// PartitionedArray<T>::PartitionedArray(
//     Definition const& definition,
//     Envelope<double, 2> const& envelope)
// 
//     : _definition{definition},
//       _envelope{envelope},
//       _index{}
// 
// {
// }
// 
// 
// template<
//     typename T>
// typename PartitionedArray<T>::const_iterator PartitionedArray<T>::begin() const
// {
//     return _index.begin();
// }
// 
// 
// template<
//     typename T>
// typename PartitionedArray<T>::const_iterator PartitionedArray<T>::end() const
// {
//     return _index.end();
// }
// 
// 
// template<
//     typename T>
// void PartitionedArray<T>::insert(
//     Value const& value)
// {
//     _index.insert(value);
// }
// 
// 
// template<
//     typename T>
// bool PartitionedArray<T>::empty() const
// {
//     return _index.empty();
// }
// 
// 
// template<
//     typename T>
// std::size_t PartitionedArray<T>::size() const
// {
//     return _index.size();
// }
// 
// 
// template<
//     typename T>
// typename PartitionedArray<T>::Definition const&
//     PartitionedArray<T>::definition() const
// {
//     return _definition;
// }
// 
// 
// template<
//     typename T>
// Envelope<double, 2> const& PartitionedArray<T>::envelope() const
// {
//     return _envelope;
// }
// 
// 
// // template<
// //     typename T>
// // void PartitionedArray<T>::wait()
// // {
// //     wait_all(_index);
// // }
// 
// 
// template<
//     typename T>
// bool PartitionedArray<T>::valid() const
// {
//     return all_valid(_index);
// }


template class PartitionedArray<bool>;
template class PartitionedArray<double>;

}  // namespace lue
