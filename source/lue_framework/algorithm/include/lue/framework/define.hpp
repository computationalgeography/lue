#pragma once


// Given a function template
// How to turn a function template instantiation into an action type?
// - Do this once for a given function template:
//     - Define function template (duh!)
//     - Define an empty primary class template with the same template
//         parameter types as the function template
// - Do this for every combination of parameter types:
//     - Define a function named after the template parameter types,
//         instantiating the function template for the collection of types
//     - Define an action type, based on the function defined in the
//         previous step
//     - Specialize the primary class template created above for the function
//         template parameter types. Add an Action member with the correct
//         HPX action type. Clients can access this type using the types to
//         pass to the function template.


/*!
    @brief      Define a primary class template for action types
    @param      Name Name of class template
    @param      nr_types Number of template parameters
*/
#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE(           \
    Class,                                           \
    nr_types)                                        \
LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_##nr_types(Class)


#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_1(  \
    Class)                                    \
template<                                     \
    typename T1>                              \
struct Class                                  \
{                                             \
};


#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_2(  \
    Class)                                    \
template<                                     \
    typename T1,                              \
    typename T2>                              \
struct Class                                  \
{                                             \
};


#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_3(  \
    Class)                                    \
template<                                     \
    typename T1,                              \
    typename T2,                              \
    typename T3>                              \
struct Class                                  \
{                                             \
};


#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_4(  \
    Class)                                    \
template<                                     \
    typename T1,                              \
    typename T2,                              \
    typename T3,                              \
    typename T4>                              \
struct Class                                  \
{                                             \
};


#define LUE_PLAIN_ACTION_PRIMARY_TEMPLATE_5(  \
    Class)                                    \
template<                                     \
    typename T1,                              \
    typename T2,                              \
    typename T3,                              \
    typename T4,                              \
    typename T5>                              \
struct Class                                  \
{                                             \
};


/*!
    @brief      Given template parameters, turn a function template into
                an HPX plain action and specialize a class template to
                access the resulting action type.
    @param      function Name of function template
    @param      Class Name of class template to specialize
    @param      ResultType Name of result type
    @param      ArgumentType1 Name of argument type
*/
#define LUE_PLAIN_ACTION_SPECIALIZATION(                    \
    namespace_,                                             \
    function,                                               \
    Class,                                                  \
    ResultType,                                             \
    ArgumentType1)                                          \
                                                            \
namespace namespace_ {                                      \
                                                            \
ResultType function##_##ResultType##_##ArgumentType1(       \
    ArgumentType1 value1)                                   \
{                                                           \
    return function<ResultType, ArgumentType1>(value1);     \
}                                                           \
                                                            \
}                                                           \
                                                            \
                                                            \
HPX_PLAIN_ACTION(                                           \
    namespace_::function##_##ResultType##_##ArgumentType1,  \
    Class##_##ResultType##_##ArgumentType1)                 \
                                                            \
                                                            \
namespace namespace_ {                                      \
                                                            \
template<>                                                  \
struct Class<ResultType, ArgumentType1>                     \
{                                                           \
    using Type = Class##_##ResultType##_##ArgumentType1;    \
};                                                          \
                                                            \
}  // namespace namespace_
