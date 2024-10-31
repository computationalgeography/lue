import lue
import lue.data_model as ldm
import lue.framework as lfr
import lue.pcraster as lpr
import lue.qa.scalability.instrument as lqi


print("lue version: {}".format(lue.lue_version))
print("git hash:    {}".format(lue.git_short_sha1))


assert ldm.SpaceDomain
assert lfr.log
assert lqi.ArrayExperiment
