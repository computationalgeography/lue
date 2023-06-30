
# Visualization loop

We must keep the application interactive / responsive. This requires polling for events
regularly. This requires that drawing code should take less then t amount of time.

The time t depends on:
- Speed with which data can be read from the data set (fixed). Larger is faster.
- Size of image in pixels (fixed until resized). Larger is slower.
- Skipfactor when reading rasters (flexible). Larger is faster.
- Scale. Raster: relation between shape of subset of the image to draw cells in and the shape
  of the subset of the cells. No need to draw more cells than pixels. The more distant the

We need an optimization routine that finds the optimal settings to use for visualizing fast,
incrementally.
