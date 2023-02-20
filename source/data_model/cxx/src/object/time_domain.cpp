#include "lue/object/time_domain.hpp"


namespace lue {
    namespace data_model {
        namespace {

            void write_epoch(time::Epoch const& epoch, hdf5::Attributes& attributes)
            {
                Aspect<time::Epoch::Kind>(epoch.kind()).save(attributes);

                if (epoch.origin())
                {
                    attributes.write(epoch_origin_tag, *epoch.origin());

                    if (epoch.calendar())
                    {
                        Aspect<time::Calendar>(*epoch.calendar()).save(attributes);
                    }
                }
            }


            time::Epoch read_epoch(hdf5::Attributes const& attributes)
            {
                time::Epoch epoch{};
                auto const kind =
                    string_to_aspect<time::Epoch::Kind>(attributes.read<std::string>(epoch_kind_tag));

                if (!attributes.exists(epoch_origin_tag))
                {
                    epoch = time::Epoch{kind};
                }
                else
                {
                    std::string const origin{attributes.read<std::string>(epoch_origin_tag)};

                    if (!attributes.exists(epoch_calendar_tag))
                    {
                        epoch = time::Epoch{kind, origin};
                    }
                    else
                    {
                        time::Calendar const calendar = Aspect<time::Calendar>(attributes).value();
                        epoch = time::Epoch{kind, origin, calendar};
                    }
                }

                return epoch;
            }


            void write_clock(data_model::Clock const& clock, hdf5::Attributes& attributes)
            {
                write_epoch(clock.epoch(), attributes);

                Aspect<time::Unit>(clock.unit()).save(attributes);
                attributes.write(nr_time_units_tag, clock.nr_units());
            }


            Clock read_clock(hdf5::Attributes const& attributes)
            {
                time::Epoch const epoch = read_epoch(attributes);
                auto const unit = Aspect<time::Unit>(attributes).value();
                auto const nr_units = attributes.read<time::TickPeriodCount>(nr_time_units_tag);

                return {epoch, unit, nr_units};
            }

        }  // Anonymous namespace


        TimeDomain::TimeDomain(hdf5::Group const& parent):

            hdf5::Group{parent, time_domain_tag},
            _configuration{attributes()},
            _clock{read_clock(attributes())}

        {
        }


        TimeDomain::TimeDomain(hdf5::Group&& group):

            hdf5::Group{std::move(group)},
            _configuration{attributes()},
            _clock{read_clock(attributes())}

        {
        }


        TimeConfiguration const& TimeDomain::configuration() const
        {
            return _configuration;
        }


        Clock const& TimeDomain::clock() const
        {
            return _clock;
        }


        TimeDomain create_time_domain(
            hdf5::Group& parent, TimeConfiguration const& configuration, data_model::Clock const& clock)
        {
            hdf5::Group group{hdf5::create_group(parent, time_domain_tag)};

            switch (configuration.value<TimeDomainItemType>())
            {
                case TimeDomainItemType::box:
                {
                    create_time_box(group);
                    break;
                }
                case TimeDomainItemType::cell:
                {
                    create_time_cell(group);
                    break;
                }
                case TimeDomainItemType::point:
                {
                    create_time_point(group);
                    break;
                }
            }

            write_clock(clock, group.attributes());

            configuration.save(group.attributes());

            return TimeDomain{std::move(group)};
        }


        void link_time_domain(hdf5::Group& parent, TimeDomain& domain)
        {
            parent.create_soft_link(domain.id(), time_domain_tag);
        }


        bool has_linked_time_domain(hdf5::Group const& parent)
        {
            return parent.contains_soft_link(time_domain_tag);
        }


        bool time_domain_exists(hdf5::Group const& parent)
        {
            return parent.contains_group(time_domain_tag);
        }

    }  // namespace data_model
}  // namespace lue
