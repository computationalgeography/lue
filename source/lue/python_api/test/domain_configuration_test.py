import lue
import lue_test


class DomainTest(lue_test.TestCase):

    def test_default_construct(self):

        configuration = lue.DomainConfiguration()

        self.assertEqual(configuration.time.type, lue.time_domain.omnipresent)
        self.assertEqual(configuration.space.type, lue.space_domain.omnipresent)
        self.assertEqual(configuration.space.item_type,
            lue.space_domain_item.none)


    def test_construct1(self):

        time_configuration = lue.TimeDomainConfiguration(
            lue.time_domain.shared_constant_collection,
            lue.time_domain_item.point)
        configuration = lue.DomainConfiguration(time_configuration)

        self.assertEqual(configuration.time.type,
            lue.time_domain.shared_constant_collection)
        self.assertEqual(configuration.time.item_type,
            lue.time_domain_item.point)
        self.assertEqual(configuration.space.type, lue.space_domain.omnipresent)
        self.assertEqual(configuration.space.item_type,
            lue.space_domain_item.none)


    def test_construct2(self):

        space_configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.located, lue.space_domain_item.box)
        configuration = lue.DomainConfiguration(space_configuration)

        self.assertEqual(configuration.time.type, lue.time_domain.omnipresent)
        self.assertEqual(configuration.space.type, lue.space_domain.located)
        self.assertEqual(configuration.space.item_type,
            lue.space_domain_item.box)


    def test_construct3(self):

        time_configuration = lue.TimeDomainConfiguration(
            lue.time_domain.shared_constant_collection,
            lue.time_domain_item.point)
        space_configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.located, lue.space_domain_item.box)

        configuration = lue.DomainConfiguration(time_configuration,
            space_configuration)

        self.assertEqual(configuration.time.type,
            lue.time_domain.shared_constant_collection)
        self.assertEqual(configuration.time.item_type,
            lue.time_domain_item.point)
        self.assertEqual(configuration.space.type, lue.space_domain.located)
        self.assertEqual(configuration.space.item_type,
            lue.space_domain_item.box)
