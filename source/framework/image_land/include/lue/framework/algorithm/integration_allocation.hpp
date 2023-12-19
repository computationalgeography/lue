/*
def integration_allocation(sdp_facs_flat, yield_facs_flat, old_fracs_flat, regs_flat, suit_map,
                           reg_prod, reg_demands, dems_flat, is_cropland, shape, expansion=False,
                           save=False):
    """
    Allocates new land (or remaining cropland fracs) based on summed yield

    Parameters
    ----------
    fracs_remaining : np.ndarray, default = None
                      2D array containing the remaining empty fractions
                      in each cell following the correction to the initial
                      cropland reallocation. Value defaults to None in the
                      case that the function is called to expand cropland

    """

    # map of cells that can be changed (if expansion, then non-cropland; if not, then cropland)
    cells_relevant = np.logical_xor(is_cropland, expansion)

    # shape = regs.shape
    nc = yield_facs_flat.shape[0]

    # indices, in order of declining suitability
    sorted_inds = find_sorted_indices(suit_map)

    # compute demands met boolean (one value for each region, for each crop)
    rdm = reg_prod > reg_demands[1, : , :]

    # loop through cells, from low suitability to high
    sdp = np.zeros_like(regs_flat)
    extra_fracs = np.zeros_like(old_fracs_flat)
    yields = np.zeros_like(old_fracs_flat)
    for ind in sorted_inds:
        reg = regs_flat[ind]
        # if in valid region and cell belongs to the appropriate category (cropland or not)
        if reg!=np.nan:
            reg = int(reg)
            if 0<reg<27 and cells_relevant[ind]:
                # find crops with demand already met - 'old' fraction must be 0
                c_dm = np.where(rdm[reg-1, :])[0]
                old_fracs_flat[c_dm, ind] = 0.0

                # store temporary updated regional production for current region
                reg_prod_temp = old_fracs_flat[:, ind] * yield_facs_flat[:, ind]
                # reg_prod[reg-1, :] += old_fracs_flat[:, ind] * yield_fac_flat[:, ind]

                # store temporary regional demand met boolean array for current region and c_dm
                rdm_temp = reg_prod_temp > reg_demands[1, reg-1 , :]
                c_dm = np.where(np.logical_and(rdm_temp, old_fracs_flat[:, ind]>0))[0]
                # print(c_dm)

                # correct fractions which have just made regional production exceed demand
                if len(c_dm):
                    old_fracs_flat[c_dm, ind] = ((reg_demands[1, reg-1, c_dm] - reg_prod[reg-1, c_dm])
                                                    / yield_facs_flat[c_dm, ind])
                    rdm[reg-1, c_dm] = True

                # compute potential yield, regional prod boolean array and sum of fractions
                yields[:, ind] = old_fracs_flat[:, ind] * yield_facs_flat[:, ind]
                reg_prod += yields[:, ind]
                f_sum = old_fracs_flat[:, ind].sum()
                # print(f_sum)

                # remaining regional demand for crop
                dem_remain = dems_flat[:, ind]-reg_prod[reg-1, :]
                sdp[ind] = (dem_remain * sdp_facs_flat[:, ind]).sum()
                extra_fracs[:, ind] = (old_fracs_flat[:, ind]
                                        + sdp_facs_flat[:, ind] / sdp[ind] * dem_remain)

    new_fracs_flat = old_fracs_flat + extra_fracs

    # reshape into raster-like shape
    new_fracs = np.zeros((nc, shape[0], shape[1]))
    for crop in range(nc):
        new_fracs[crop] = np.reshape(new_fracs_flat[crop], shape)

    # If some non-cropland has higher suitability than cropland, obvs don't want to expand
    # cropland when demand could potentially be met by existing cropland. Hence, need to call this
    # function twice - and the first time, to rejig existing computed fractions, will need the
    # is_cropland boolean as an input.

    nr = min(reg, 26)

    # save output arrays
    if save:
        np.save(f"test_IO\\new_fraction_maps_{nr}_{nc-1}_{shape}", new_fracs)

    return new_fracs, reg_prod
*/
