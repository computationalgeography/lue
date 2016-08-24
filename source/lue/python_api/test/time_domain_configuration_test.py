import lue
import lue_test


class TimeDomainTest(lue_test.TestCase):

    def test_default_construct(self):

        configuration = lue.TimeDomainConfiguration()

        self.assertEqual(configuration.type, lue.time_domain.omnipresent)


    def test_construct(self):

        configuration = lue.TimeDomainConfiguration(
            lue.time_domain.omnipresent)

        self.assertEqual(configuration.type, lue.time_domain.omnipresent)


        configuration = lue.TimeDomainConfiguration(
            lue.time_domain.shared_constant_collection)

        self.assertEqual(configuration.type,
            lue.time_domain.shared_constant_collection)
