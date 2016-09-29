import lue
import lue_test


class TimeDomainTest(lue_test.TestCase):

    def test_default_construct(self):

        configuration = lue.TimeDomainConfiguration()

        self.assertEqual(configuration.type, lue.time_domain.omnipresent)
        self.assertEqual(configuration.item_type, lue.time_domain_item.none)


    def test_construct(self):

        configuration = lue.TimeDomainConfiguration(
            lue.time_domain.omnipresent,
            lue.time_domain_item.none)

        self.assertEqual(configuration.type, lue.time_domain.omnipresent)
        self.assertEqual(configuration.item_type, lue.time_domain_item.none)


        configuration = lue.TimeDomainConfiguration(
            lue.time_domain.shared_constant_collection,
            lue.time_domain_item.point)

        self.assertEqual(configuration.type,
            lue.time_domain.shared_constant_collection)
        self.assertEqual(configuration.item_type, lue.time_domain_item.point)
