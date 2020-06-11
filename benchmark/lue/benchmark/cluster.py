class Core(object):

    def __init__(self, json):
        self.from_json(json)

    def __str__(self):
        return "Core(nr_threads={})" \
            .format(
                self.nr_threads,
            )

    def from_json(self, json):
        self.nr_threads = json["nr_threads"]

    def to_json(self):
        return {
                "nr_threads": self.nr_threads,
            }


class NUMANode(object):

    def __init__(self, json):
        self.from_json(json)

    def __str__(self):
        return "NUMANode(nr_cores={}, core={})" \
            .format(
                self.nr_cores,
                self.core,
            )

    @property
    def nr_threads(self):
        return self.nr_cores * self.core.nr_threads

    def from_json(self, json):
        self.nr_cores = json["nr_cores"]
        self.core = Core(json["core"])

    def to_json(self):
        return {
                "nr_cores": self.nr_cores,
                "core": self.core.to_json(),
            }


class Package(object):

    def __init__(self, json):
        self.from_json(json)

    def __str__(self):
        return "Package(nr_numa_nodes={}, numa_node={})" \
            .format(
                self._nr_numa_nodes,
                self.numa_node.to_json(),
            )

    @property
    def nr_numa_nodes(self):
        return self._nr_numa_nodes

    @property
    def nr_cores(self):
        return self._nr_numa_nodes * self.numa_node.nr_cores

    @property
    def nr_threads(self):
        return self._nr_numa_nodes * self.numa_node.nr_threads

    def from_json(self, json):
        self._nr_numa_nodes = json["nr_numa_nodes"]
        self.numa_node = NUMANode(json["numa_node"])

    def to_json(self):
        return {
                "nr_numa_nodes": self.nr_numa_nodes,
                "numa_node": self.numa_node.to_json(),
            }


class ClusterNode(object):

    def __init__(self, json):
        self.from_json(json)

    def __str__(self):
        return "ClusterNode(nr_packages={}, package={})" \
            .format(
                self.nr_packages,
                self.package,
            )

    @property
    def nr_numa_nodes(self):
        return self.nr_packages * self.package.nr_numa_nodes

    @property
    def nr_cores(self):
        return self.nr_packages * self.package.nr_cores

    @property
    def nr_threads(self):
        return self.nr_packages * self.package.nr_threads

    def from_json(self, json):
        self.nr_packages = json["nr_packages"]
        self.package = Package(json["package"])

    def to_json(self):
        return {
                "nr_packages": self.nr_packages,
                "package": self.package.to_json(),
            }


class Scheduler(object):

    def __init__(self, json):
        self._from_json(json)

    def _from_json(self, json):
        self.kind = json["kind"]

    def to_json(self):
        return {
                "kind": self.kind
            }


class ShellScheduler(Scheduler):

    def __init__(self, json):
        super(ShellScheduler, self).__init__(json)

    def to_json(self):
        return super(ShellScheduler, self).to_json()


class SlurmSettings(object):

    def __init__(self, json):
        self.from_json(json)

    def from_json(self, json):
        self.partition_name = json["partition"]
        self.sbatch_options = \
            json["sbatch_options"] if "sbatch_options" in json else ""
        self.srun_options = \
            json["srun_options"] if "srun_options" in json else ""

    def to_json(self):
        result = {
                "partition": self.partition_name,
            }

        if self.sbatch_options:
            result["sbatch_options"] = self.sbatch_options

        if self.srun_options:
            result["srun_options"] = self.srun_options

        return result


class SlurmScheduler(Scheduler):

    def __init__(self, json):
        super(SlurmScheduler, self).__init__(json)
        self.from_json(json)

    def from_json(self, json):
        self.settings = SlurmSettings(json["settings"])

    def to_json(self):
        result = super(SlurmScheduler, self).to_json()

        result["settings"] = self.settings.to_json()

        return result


class Cluster(object):

    def __init__(self, json):

        self.from_json(json)

    def from_json(self, json):
        self.name = json["name"]
        scheduler_json = json["scheduler"]
        scheduler_kind = scheduler_json["kind"]

        if scheduler_kind == "shell":
            self.scheduler = ShellScheduler(scheduler_json)
        elif scheduler_kind == "slurm":
            self.scheduler = SlurmScheduler(scheduler_json)

        self.nr_cluster_nodes = json["nr_cluster_nodes"]
        self.cluster_node = ClusterNode(json["cluster_node"])

    def to_json(self):
        return {
                "name": self.name,
                "scheduler": self.scheduler.to_json(),
                "nr_cluster_nodes": self.nr_cluster_nodes,
                "cluster_node": self.cluster_node.to_json(),
            }

    def nr_localities_to_reserve(self,
            worker,
            locality_per):
        """
        When scheduling jobs, the number of localities we need to ask
        for from the scheduler

        This number might be larger than the actual number of localities
        used by tasks. We want to prevent to compete with other jobs
        while performing the benchmarks.
        """
        if worker.type == "thread":
            # Claim a whole cluster node
            if locality_per == "cluster_node":
                result = 1
            elif locality_per == "numa_node":
                result = self.cluster_node.nr_numa_nodes
        elif worker.type == "numa_node":
            # Claim a whole cluster node
            assert locality_per == "numa_node"
            result = self.cluster_node.nr_numa_nodes
        elif worker.type == "cluster_node":
            # Claim all cluster nodes in the partition
            if locality_per == "cluster_node":
                result = worker.nr_cluster_nodes
            elif locality_per == "numa_node":
                result = worker.nr_cluster_nodes * self.cluster_node.nr_numa_nodes

        return result
