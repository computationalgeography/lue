class WorkerRange(object):

    def __init__(self,
            data):

        self._from_json(data)

        self.max_size = self.permutation_size(self.nr_permutations - 1)

    def _from_json(self,
            data):

        self.min_size = data["min_size"]
        self.max_size = data["max_size"]

        assert self.min_size > 0
        assert self.min_size <= self.max_size

    def to_json(self):

        return {
                "min_size": self.min_size,
                "max_size": self.max_size,
            }


class MultipliedWorkerRange(WorkerRange):

    def __init__(self,
            data):

        self.from_json(data)
        super(MultipliedWorkerRange, self).__init__(data)

    def __str__(self):

        return "MultipliedWorkerRange(min_size={}, max_size={}, multiplier={})" \
            .format(
                self.min_size,
                self.max_size,
                self.multiplier,
            )

    def from_json(self,
            data):

        self.multiplier = data["multiplier"]
        assert self.multiplier > 1

    def to_json(self):

        result = super(MultipliedWorkerRange, self).to_json()
        result["multiplier"] = self.multiplier

        return result

    @property
    def nr_permutations(self):

        size = self.min_size
        nr = 1

        while self.multiplier * size <= self.max_size:
            size *= self.multiplier
            nr += 1

        return nr

    def permutation_size(self,
            idx):

        assert idx in range(self.nr_permutations), idx

        size = self.min_size
        idx -= 1

        while idx >= 0:
            size *= self.multiplier
            idx -= 1

        return size


class IncrementedWorkerRange(WorkerRange):

    def __init__(self,
            data):

        self.from_json(data)
        super(IncrementedWorkerRange, self).__init__(data)

    def __str__(self):

        return "IncrementedWorkerRange(min_size={}, max_size={}, incrementor={})" \
            .format(
                self.min_size,
                self.max_size,
                self.incrementor,
            )

    def from_json(self,
            data):

        self.incrementor = data["incrementor"]
        assert self.incrementor >= 1

    def to_json(self):

        result = super(IncrementedWorkerRange, self).to_json()
        result["incrementor"] = self.incrementor

        return result

    @property
    def nr_permutations(self):

        size = self.min_size
        nr = 1

        size += self.incrementor

        while size <= self.max_size:
            size += self.incrementor
            nr += 1

        return nr

    def permutation_size(self,
            idx):

        assert idx in range(self.nr_permutations), idx

        size = self.min_size
        idx -= 1

        while idx >= 0:
            size += self.incrementor
            idx -= 1

        return size


class EmptyWorkerRange(WorkerRange):
    """
    Worker range representing a single collection of workers of a
    certain size
    """

    def __init__(self,
            data):

        self.from_json(data)

        super(EmptyWorkerRange, self).__init__(
            {"min_size": self.size, "max_size": self.size})
        assert self.min_size == self.max_size == self.size

    def __str__(self):
        return "EmptyWorkerRange(size={})" \
            .format(
                self.size)

    def from_json(self,
            data):

        self.size = data["size"]
        assert self.size >= 1

    def to_json(self):

        return {
                "size": self.size,
            }

    @property
    def nr_permutations(self):

        return 1

    def permutation_size(self,
            idx):

        assert idx == 0
        return self.size


class Pool(object):

    def __init__(self,
            data):

        if "multiplier" in data:
            self.range = MultipliedWorkerRange(data)
        elif "incrementor" in data:
            self.range = IncrementedWorkerRange(data)
        else:
            self.range = EmptyWorkerRange(data)

    def to_json(self):

        return self.range.to_json()

    @property
    def min_size(self):

        return self.range.min_size

    @property
    def max_size(self):

        return self.range.max_size

    @property
    def nr_permutations(self):

        return self.range.nr_permutations

    def permutation_size(self,
            idx):

        return self.range.permutation_size(idx)
