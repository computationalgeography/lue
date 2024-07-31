import os


def pcraster_by_lue():
    import lue.framework as lfr
    import lue.pcraster as pcr
    import lue.pcraster.framework as pcrfw

    pcr.runtime_scope = lfr.runtime_scope

    return pcr, pcrfw


def pcraster_by_pcraster():
    import pcraster as pcr
    import pcraster.framework as pcrfw

    def runtime_scope(user_main):
        def decorated_function(*args, **kwargs):
            user_main(*args, **kwargs)

        return decorated_function

    pcr.runtime_scope = runtime_scope

    return pcr, pcrfw


def pcraster_by_provider():
    pcraster_by_provider_name = {
        "lue": pcraster_by_lue,
        "pcraster": pcraster_by_pcraster,
    }

    provider_name = os.getenv("LUE_PCRASTER_PROVIDER_NAME", default="unknown")

    if provider_name not in pcraster_by_provider_name:
        raise RuntimeError(
            f'Select a PCRaster provider by setting the LUE_PCRASTER_PROVIDER_NAME environment variable to one of {", ".join(pcraster_by_provider_name)}'
        )

    pcr, pcrfw = pcraster_by_provider_name[provider_name]()
    pcr.provider_name = provider_name

    return pcr, pcrfw


pcr, pcrfw = pcraster_by_provider()
