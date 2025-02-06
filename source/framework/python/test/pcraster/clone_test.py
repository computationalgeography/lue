import lue.framework as lfr
import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class CloneTest(OperationTest):
    @lue_test.framework_test_case
    def test_setclone(self):
        for expression_type in self.numeric_types:
            spatial = self.spatial[expression_type]
            raster_name = "test_setclone.tif"

            # Write a raster
            lfr.to_gdal(spatial, raster_name).get()

            # Use the written raster as clone
            lpr.setclone(raster_name)

            # Test clone properties
            self.assertEqual(lpr.configuration.array_shape, spatial.shape)
            self.assertEqual(lpr.configuration.cell_size, 1)
            self.assertEqual(
                lpr.configuration.bounding_box,
                lpr.BoundingBox(
                    north=spatial.shape[0], west=0, south=0, east=spatial.shape[1]
                ),
            )

            nr_rows, nr_cols = spatial.shape

            self.assertEqual(lpr.clone().cellSize(), 1)
            self.assertEqual(lpr.clone().nrRows(), nr_rows)
            self.assertEqual(lpr.clone().nrCols(), nr_cols)

    @lue_test.framework_test_case
    def test_non_spatial_to_spatial(self):
        for expression_type in self.numeric_types:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )
            raster_name = "test_non_spatial_to_spatial.tif"

            lfr.to_gdal(spatial, raster_name).get()
            lpr.setclone(raster_name)

            spatial_out = lpr.spatial(non_spatial)

            self.assertEqual(spatial_out.shape, spatial.shape)
