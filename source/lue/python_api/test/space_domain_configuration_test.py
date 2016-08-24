import lue
import lue_test


class SpaceDomainTest(lue_test.TestCase):

    def test_default_construct(self):

        configuration = lue.SpaceDomainConfiguration()

        self.assertEqual(configuration.type, lue.space_domain.omnipresent)
        self.assertEqual(configuration.item_type, lue.space_domain_item.none)


    def test_construct1(self):

        configuration = lue.SpaceDomainConfiguration(
            lue.space_domain_item.point)

        self.assertEqual(configuration.type, lue.space_domain.located)
        self.assertEqual(configuration.item_type, lue.space_domain_item.point)


    def test_construct1_error(self):

        # It is not OK to not specify a domain item type. The constructor
        # assumes located space.
        self.assertRaises(RuntimeError, lue.SpaceDomainConfiguration,
            lue.space_domain_item.none)


    def test_construct2(self):

        configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.omnipresent, lue.space_domain_item.none)

        self.assertEqual(configuration.type, lue.space_domain.omnipresent)
        self.assertEqual(configuration.item_type, lue.space_domain_item.none)


        configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.located, lue.space_domain_item.point)

        self.assertEqual(configuration.type, lue.space_domain.located)
        self.assertEqual(configuration.item_type, lue.space_domain_item.point)


        # It is OK to pass a domain item type, but given that the domain type
        # is omnipresent, it will be reset to none.
        configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.omnipresent, lue.space_domain_item.point)

        self.assertEqual(configuration.type, lue.space_domain.omnipresent)
        self.assertEqual(configuration.item_type, lue.space_domain_item.none)


    def test_construct2_error(self):

        # It is not OK to pass a located domain type, but not to specify a
        # domain item type.
        self.assertRaises(RuntimeError, lue.SpaceDomainConfiguration,
            lue.space_domain.located, lue.space_domain_item.none)
