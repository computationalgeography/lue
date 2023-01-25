import matplotlib.scale as mscale
import matplotlib.pyplot as plt
import matplotlib.transforms as mtransforms
import matplotlib.ticker as ticker
import numpy as np


# https://www.nordtheme.com/

# Colors for scalability plots
actual_color = "#5e81ac"  # Blue
serial_color = "#bf616a"  # Red
linear_color = "#a3be8c"  # Green

default_linewidth = 2


class SquareRootScale(mscale.ScaleBase):
    """
    ScaleBase class for generating square root scale.
    """

    name = "square_root"

    def __init__(self, axis, **kwargs):

        mscale.ScaleBase.__init__(self)

    def set_default_locators_and_formatters(self, axis):
        axis.set_major_locator(ticker.AutoLocator())
        axis.set_major_formatter(ticker.ScalarFormatter())
        axis.set_minor_locator(ticker.NullLocator())
        axis.set_minor_formatter(ticker.NullFormatter())

    def limit_range_for_scale(self, vmin, vmax, minpos):
        return max(0.0, vmin), vmax

    class SquareRootTransform(mtransforms.Transform):
        input_dims = 1
        output_dims = 1
        is_separable = True

        def transform_non_affine(self, a):
            return np.array(a) ** 0.5

        def inverted(self):
            return SquareRootScale.InvertedSquareRootTransform()

    class InvertedSquareRootTransform(mtransforms.Transform):
        input_dims = 1
        output_dims = 1
        is_separable = True

        def transform(self, a):
            return np.array(a) ** 2

        def inverted(self):
            return SquareRootScale.SquareRootTransform()

    def get_transform(self):
        return self.SquareRootTransform()


### mscale.register_scale(SquareRootScale)

### fig, ax = plt.subplots(1)
###
### ax.plot(np.arange(0, 9)**2, label='$y=x^2$')
### ax.legend()
###
### ax.set_yscale('squareroot')
### ax.set_yticks(np.arange(0,9,2)**2)
### ax.set_yticks(np.arange(0,8.5,0.5)**2, minor=True)
###
### plt.show()
