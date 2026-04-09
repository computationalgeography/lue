#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/future.hpp>


namespace lue {

    /*!
        @brief      Class for scalar values, representing an omnipresent value

        The actual value is "pointed to" by a shared future. This implies that it can be the
        result of an asynchronous computation. Continuations can be attached to the value,
        which will be triggered after the future has become ready.

        Copieѕ of shared futures, share their state (the actual value). This implies that
        copies of Scalar instances, through their copied layered shared future also share the
        actual value.
    */
    template<typename Element>
    class Scalar
    {
        public:

            /*!
                @brief      Default-construct an instance

                The layered future will be ready immediately, containing a default constructed
                value.
            */
            Scalar():

                _value_f{hpx::make_ready_future<Element>()}

            {
            }


            /*!
                @brief      Copy-construct an instance

                The layered future will be copied from the instance passed in. Not that these
                copies share state (the actual value).
            */
            Scalar(Scalar const& other) = default;


            /*!
                @brief      Move-construct an instance
            */
            Scalar(Scalar&& other) noexcept:

                _value_f{std::move(other._value_f)}

            {
            }


            /*!
                @brief      Convert-construct an instance

                The layered future will be ready immediately, containing a copy of the value
                passed in.
            */
            Scalar(Element const& value):

                _value_f{hpx::make_ready_future<Element>(value)}

            {
            }


            /*!
                @brief      Convert-construct an instance

                The layered future will be ready immediately, containing the value passed in.
            */
            explicit Scalar(Element&& value):

                _value_f{hpx::make_ready_future<Element>(std::move(value))}

            {
            }


            /*!
                @brief      Convert-construct an instance

                The layered future will use the state of the future passed in.
            */
            Scalar(hpx::future<Element>&& element_f):

                _value_f{std::move(element_f)}

            {
            }


            /*!
                @brief      Convert-construct an instance

                The layered future will use the state of the future passed in.
            */
            Scalar(hpx::shared_future<Element> const& element_f):

                _value_f{element_f}

            {
            }


            /*!
                @brief      Convert-construct an instance

                The layered future will use the state of the future passed in.
            */
            Scalar(hpx::shared_future<Element>&& element_f):

                _value_f{std::move(element_f)}

            {
            }


            ~Scalar() = default;


            /*!
                @brief      Copy-assign an instance to this instance

                The state of the layered future of the instance passed in will be shared by
                the layered future in this instance.
            */
            auto operator=(Scalar const& other) -> Scalar&
            {
                if (&other != this)
                {
                    _value_f = other._value_f;
                }

                return *this;
            }


            /*!
                @brief      Move-assign an instance to this instance

                The state of the layered future of the instance passed in will be moved to the
                layered future in this instance.
            */
            auto operator=(Scalar&& other) noexcept -> Scalar&
            {
                if (&other != this)
                {
                    _value_f = std::move(other._value_f);
                }

                return *this;
            }


            /*!
                @brief      Return the layered future

                The result can be used to attach continuations to.
            */
            auto future() const -> hpx::shared_future<Element> const&
            {
                return _value_f;
            }


        private:

            //! A future to state that can be created asynchronously
            hpx::shared_future<Element> _value_f;


            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                // clang-format off
                archive & _value_f;
                // clang-format on
            }
    };


    namespace detail {

        // Specialization for PartitionedArray
        template<typename E>
        class ArrayTraits<Scalar<E>>
        {

            public:

                using Element = E;
        };

    }  // namespace detail

}  // namespace lue
