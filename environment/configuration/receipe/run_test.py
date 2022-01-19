import lue
import lue.data_model as ldm
import lue.framework as lfr

print('lue version: {}'.format(lue.lue_version))
print('git hash:    {}'.format(lue.git_short_sha1))

assert ldm.SpaceDomain

assert lfr.log
