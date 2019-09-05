class Pool(object):

    def __init__(self, json):

        self.min_size = json["min_size"]
        self.max_size = json["max_size"]
        self.multiplier = json["multiplier"] if "multiplier" in json else 1

        assert self.min_size > 0
        assert self.min_size <= self.max_size
        assert self.multiplier > 1

        self.max_size = self.permutation_size(self.nr_permutations() - 1)


    def __str__(self):
        return "Pool(min_size={}, max_size={}, multiplier={})" \
            .format(
                self.min_size,
                self.max_size,
                self.multiplier,
            )


    def nr_permutations(self):
        size = self.min_size
        nr = 1

        while self.multiplier * size <= self.max_size:
            size *= self.multiplier
            nr += 1

        return nr


    def permutation_size(self,
            idx):

        assert idx in range(self.nr_permutations()), idx

        size = self.min_size
        idx -= 1

        while idx >= 0:
            size *= self.multiplier
            idx -= 1

        return size
