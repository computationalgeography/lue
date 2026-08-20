class Core:
    def __init__(self, data):
        self.from_json(data)

    def __str__(self):
        return f"Core(nr_threads={self.nr_threads})"

    def from_json(self, data):
        self.nr_threads = data["nr_threads"]

    def to_json(self):
        return {
            "nr_threads": self.nr_threads,
        }


class NUMANode:
    def __init__(self, data):
        self.from_json(data)

    def __str__(self):
        return f"NUMANode(memory={self.memory}, nr_cores={self.nr_cores}, core={self.core})"

    @property
    def nr_threads(self):
        return self.nr_cores * self.core.nr_threads

    def from_json(self, data):
        self.memory = data["memory"]
        self.nr_cores = data["nr_cores"]
        self.core = Core(data["core"])

    def to_json(self):
        return {
            "memory": self.memory,
            "nr_cores": self.nr_cores,
            "core": self.core.to_json(),
        }


class Package:
    def __init__(self, data):
        self.from_json(data)

    def __str__(self):
        return f"Package(nr_numa_nodes={self._nr_numa_nodes}, numa_node={self.numa_node.to_json()})"

    @property
    def nr_numa_nodes(self):
        return self._nr_numa_nodes

    @property
    def memory(self):
        return self._nr_numa_nodes * self.numa_node.memory

    @property
    def nr_cores(self):
        return self._nr_numa_nodes * self.numa_node.nr_cores

    @property
    def nr_threads(self):
        return self._nr_numa_nodes * self.numa_node.nr_threads

    def from_json(self, data):
        self._nr_numa_nodes = data["nr_numa_nodes"]
        self.numa_node = NUMANode(data["numa_node"])

    def to_json(self):
        return {
            "nr_numa_nodes": self.nr_numa_nodes,
            "numa_node": self.numa_node.to_json(),
        }


class ClusterNode:
    def __init__(self, data):
        self.from_json(data)

    def __str__(self):
        return f"ClusterNode(nr_packages={self.nr_packages}, package={self.package})"

    @property
    def nr_numa_nodes(self):
        return self.nr_packages * self.package.nr_numa_nodes

    @property
    def memory(self):
        return self.nr_packages * self.package.memory

    @property
    def nr_cores(self):
        return self.nr_packages * self.package.nr_cores

    @property
    def nr_threads(self):
        return self.nr_packages * self.package.nr_threads

    def from_json(self, data):
        self.nr_packages = data["nr_packages"]
        self.package = Package(data["package"])

    def to_json(self):
        return {
            "nr_packages": self.nr_packages,
            "package": self.package.to_json(),
        }


class Scheduler:
    def __init__(self, data):
        self._from_json(data)

    def _from_json(self, data):
        self.kind = data["kind"]

    def to_json(self):
        return {"kind": self.kind}


class ShellScheduler(Scheduler):
    def __init__(self, data):
        super().__init__(data)

    def to_json(self):
        return super().to_json()


class SlurmSettings:
    def __init__(self, data):
        self.from_json(data)

    def from_json(self, data):
        self.partition_name = data["partition"]
        self.sbatch_options = data.get("sbatch_options", "")
        self.mpirun_options = data.get("mpirun_options", "")
        self.srun_options = data.get("srun_options", "")

    def to_json(self):
        result = {
            "partition": self.partition_name,
        }

        if self.sbatch_options:
            result["sbatch_options"] = self.sbatch_options

        if self.mpirun_options:
            result["mpirun_options"] = self.mpirun_options

        if self.srun_options:
            result["srun_options"] = self.srun_options

        return result


class SlurmScheduler(Scheduler):
    def __init__(self, data):
        super().__init__(data)
        self.from_json(data)

    def from_json(self, data):
        self.settings = SlurmSettings(data["settings"])

    def to_json(self):
        result = super().to_json()

        result["settings"] = self.settings.to_json()

        return result


class SoftwareEnvironment:
    def __init__(self, data):
        self.from_json(data)

    def from_json(self, data):
        self.module_names = data["modules"]

        if "venv" in data:
            self.venv = data["venv"]
        else:
            self.venv = None

    def to_json(self):
        result = {
            "modules": self.module_names,
        }

        if self.venv:
            result["venv"] = self.venv

        return result

    @property
    def configuration(self):
        commands = []

        if self.module_names:
            commands = ["module purge"]

        commands += [f"module load {name}" for name in self.module_names]

        if self.venv:
            environment = self.venv["environment"]
            commands.append(f"source {environment}/bin/activate")

        return "\n".join(commands)


class Cluster:
    def __init__(self, data):
        self.from_json(data)

    def from_json(self, data):
        self.name = data["name"]
        scheduler_json = data["scheduler"]
        scheduler_kind = scheduler_json["kind"]

        if scheduler_kind == "shell":
            self.scheduler = ShellScheduler(scheduler_json)
        elif scheduler_kind == "slurm":
            self.scheduler = SlurmScheduler(scheduler_json)

        self.nr_cluster_nodes = data["nr_cluster_nodes"]
        self.cluster_node = ClusterNode(data["cluster_node"])

        self.software_environment = (
            SoftwareEnvironment(data["software_environment"])
            if "software_environment" in data
            else None
        )

    def to_json(self):
        result = {
            "name": self.name,
            "scheduler": self.scheduler.to_json(),
            "nr_cluster_nodes": self.nr_cluster_nodes,
            "cluster_node": self.cluster_node.to_json(),
        }

        if self.software_environment:
            result["software_environment"] = self.software_environment.to_json()

        return result

    def nr_localities_to_reserve(self, worker, locality_per) -> int:
        """
        When scheduling jobs, the number of localities we need to ask for from the scheduler

        This number might be larger than the actual number of localities used by tasks. We want to prevent to
        compete with other jobs while performing the benchmarks.
        """
        result = -1

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
