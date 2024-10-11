import numpy as np

import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class LocalOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_add(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial + spatial
            _ = spatial + non_spatial
            _ = non_spatial + spatial
            _ = non_spatial + non_spatial

    @lue_test.framework_test_case
    def test_max(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.max(spatial, spatial)
            _ = lpr.max(spatial, non_spatial)
            _ = lpr.max(non_spatial, spatial)
            _ = lpr.max(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_min(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.min(spatial, spatial)
            _ = lpr.min(spatial, non_spatial)
            _ = lpr.min(non_spatial, spatial)
            _ = lpr.min(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_subtract(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial - spatial
            _ = spatial - non_spatial
            _ = non_spatial - spatial
            # _ = non_spatial - non_spatial

    @lue_test.framework_test_case
    def test_divide(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial / spatial
            _ = spatial / non_spatial
            _ = non_spatial / spatial
            # _ = non_spatial / non_spatial

            _ = lpr.div(spatial, spatial)
            _ = lpr.div(spatial, non_spatial)
            _ = lpr.div(non_spatial, spatial)
            _ = lpr.div(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_multiply(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial * spatial
            _ = spatial * non_spatial
            _ = non_spatial * spatial
            # _ = non_spatial * non_spatial

    @lue_test.framework_test_case
    def test_power(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial**spatial
            _ = spatial**non_spatial
            _ = non_spatial**spatial
            # _ = non_spatial**non_spatial

    @lue_test.framework_test_case
    def test_abs(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.abs(spatial)
            _ = lpr.abs(non_spatial)

    @lue_test.framework_test_case
    def test_acos(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.acos(spatial)
            _ = lpr.acos(non_spatial)

    @lue_test.framework_test_case
    def test_and(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.pcrand(spatial, spatial)
            _ = lpr.pcrand(spatial, spatial)
            _ = lpr.pcrand(non_spatial, non_spatial)
            _ = lpr.pcrand(non_spatial, non_spatial)

            _ = spatial & spatial
            _ = spatial & spatial
            _ = non_spatial & non_spatial
            # _ = non_spatial & non_spatial

    @lue_test.framework_test_case
    def test_asin(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.asin(spatial)
            _ = lpr.asin(non_spatial)

    # TODO Results in an int32 array, not uint8 array
    # @lue_test.framework_test_case
    # def test_boolean(self):
    #     for expression_type in [np.uint8, np.int32, np.float32]:
    #         spatial, non_spatial = self.spatial[expression_type], self.non_spatial[expression_type]

    #         _ = lpr.boolean(spatial)
    #         _ = lpr.boolean(non_spatial)

    @lue_test.framework_test_case
    def test_cos(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.cos(spatial)
            _ = lpr.cos(non_spatial)

    @lue_test.framework_test_case
    def test_cover(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.cover(spatial, spatial)
            _ = lpr.cover(spatial, non_spatial)
            _ = lpr.cover(non_spatial, non_spatial)
            _ = lpr.cover(non_spatial, spatial)

    @lue_test.framework_test_case
    def test_defined(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.defined(spatial)
            _ = lpr.defined(non_spatial)

    @lue_test.framework_test_case
    def test_eq(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial == spatial
            _ = spatial == non_spatial
            _ = non_spatial == spatial
            # _ = non_spatial == non_spatial

            _ = lpr.eq(spatial, spatial)
            _ = lpr.eq(spatial, non_spatial)
            _ = lpr.eq(non_spatial, spatial)
            _ = lpr.eq(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_exp(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.exp(spatial)
            _ = lpr.exp(non_spatial)

    @lue_test.framework_test_case
    def test_ge(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial >= spatial
            _ = spatial >= non_spatial
            _ = non_spatial >= spatial
            # _ = non_spatial >= non_spatial

            _ = lpr.ge(spatial, spatial)
            _ = lpr.ge(spatial, non_spatial)
            _ = lpr.ge(non_spatial, spatial)
            _ = lpr.ge(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_gt(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial > spatial
            _ = spatial > non_spatial
            _ = non_spatial > spatial
            # _ = non_spatial > non_spatial

            _ = lpr.gt(spatial, spatial)
            _ = lpr.gt(spatial, non_spatial)
            _ = lpr.gt(non_spatial, spatial)
            _ = lpr.gt(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_ifthen(self):
        spatial_condition, non_spatial_condition = (
            self.spatial[np.uint8],
            self.non_spatial[np.uint8],
        )

        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial_expression, non_spatial_expression = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.ifthen(spatial_condition, spatial_expression)
            _ = lpr.ifthen(spatial_condition, non_spatial_expression)
            _ = lpr.ifthen(non_spatial_condition, spatial_expression)
            _ = lpr.ifthen(non_spatial_condition, non_spatial_expression)

    @lue_test.framework_test_case
    def test_ifthenelse(self):
        spatial_condition, non_spatial_condition = (
            self.spatial[np.uint8],
            self.non_spatial[np.uint8],
        )

        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial_expression, non_spatial_expression = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.ifthenelse(
                spatial_condition, spatial_expression, spatial_expression
            )
            _ = lpr.ifthenelse(
                spatial_condition, spatial_expression, non_spatial_expression
            )
            _ = lpr.ifthenelse(
                spatial_condition, non_spatial_expression, spatial_expression
            )
            _ = lpr.ifthenelse(
                spatial_condition, non_spatial_expression, non_spatial_expression
            )
            _ = lpr.ifthenelse(
                non_spatial_condition, spatial_expression, spatial_expression
            )
            _ = lpr.ifthenelse(
                non_spatial_condition, spatial_expression, non_spatial_expression
            )
            _ = lpr.ifthenelse(
                non_spatial_condition, non_spatial_expression, spatial_expression
            )
            _ = lpr.ifthenelse(
                non_spatial_condition, non_spatial_expression, non_spatial_expression
            )

    @lue_test.framework_test_case
    def test_le(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial <= spatial
            _ = spatial <= non_spatial
            _ = non_spatial <= spatial
            # _ = non_spatial <= non_spatial

            _ = lpr.le(spatial, spatial)
            _ = lpr.le(spatial, non_spatial)
            _ = lpr.le(non_spatial, spatial)
            _ = lpr.le(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_ln(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.ln(spatial)
            _ = lpr.ln(non_spatial)

    @lue_test.framework_test_case
    def test_lt(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial < spatial
            _ = spatial < non_spatial
            _ = non_spatial < spatial
            # _ = non_spatial < non_spatial

            _ = lpr.lt(spatial, spatial)
            _ = lpr.lt(spatial, non_spatial)
            _ = lpr.lt(non_spatial, spatial)
            _ = lpr.lt(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_ne(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial != spatial
            _ = spatial != non_spatial
            _ = non_spatial != spatial
            # _ = non_spatial != non_spatial

            _ = lpr.ne(spatial, spatial)
            _ = lpr.ne(spatial, non_spatial)
            _ = lpr.ne(non_spatial, spatial)
            _ = lpr.ne(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_normal(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.normal(spatial)
            _ = lpr.normal(non_spatial)

    @lue_test.framework_test_case
    def test_not(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.pcrnot(spatial)
            _ = lpr.pcrnot(non_spatial)

            _ = ~spatial
            # _ = ~non_spatial

    @lue_test.framework_test_case
    def test_or(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.pcror(spatial, spatial)
            _ = lpr.pcror(spatial, spatial)
            _ = lpr.pcror(non_spatial, non_spatial)
            _ = lpr.pcror(non_spatial, non_spatial)

            _ = spatial | spatial
            _ = spatial | spatial
            # _ = non_spatial | non_spatial
            # _ = non_spatial | non_spatial

    @lue_test.framework_test_case
    def test_rounddown(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.rounddown(spatial)
            _ = lpr.rounddown(non_spatial)

    @lue_test.framework_test_case
    def test_roundoff(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.roundoff(spatial)
            _ = lpr.roundoff(non_spatial)

    @lue_test.framework_test_case
    def test_roundup(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.roundup(spatial)
            _ = lpr.roundup(non_spatial)

    @lue_test.framework_test_case
    def test_sin(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.sin(spatial)
            _ = lpr.sin(non_spatial)

    @lue_test.framework_test_case
    def test_spatial(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.spatial(spatial)
            _ = lpr.spatial(non_spatial)

    @lue_test.framework_test_case
    def test_sqr(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.sqr(spatial)
            _ = lpr.sqr(non_spatial)

    @lue_test.framework_test_case
    def test_sqrt(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.sqrt(spatial)
            _ = lpr.sqrt(non_spatial)

    @lue_test.framework_test_case
    def test_tan(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.tan(spatial)
            _ = lpr.tan(non_spatial)

    @lue_test.framework_test_case
    def test_uniform(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.uniform(spatial)
            _ = lpr.uniform(non_spatial)

    @lue_test.framework_test_case
    def test_uniqueid(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.uniqueid(spatial)
            _ = lpr.uniqueid(non_spatial)

    @lue_test.framework_test_case
    def test_xor(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.pcrxor(spatial, spatial)
            _ = lpr.pcrxor(spatial, spatial)
            _ = lpr.pcrxor(non_spatial, non_spatial)
            _ = lpr.pcrxor(non_spatial, non_spatial)

            _ = spatial | spatial
            _ = spatial | spatial
            # _ = non_spatial | non_spatial
            # _ = non_spatial | non_spatial
