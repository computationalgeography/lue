import lue
import lue.data_model as ldm
import lue.framework as lfr
import lue.qa.scalability as lqs


print("lue version: {}".format(lue.lue_version))
print("git hash:    {}".format(lue.git_short_sha1))


assert ldm.SpaceDomain
assert lfr.log
assert lqs.perform_experiment_task
