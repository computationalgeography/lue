class Core(object):

    def __init__(self, json):
        self.nr_threads = json["nr_threads"]

    def __str__(self):
        return "Core(nr_threads={})" \
            .format(
                self.nr_threads,
            )


class NUMADomain(object):

    def __init__(self, json):
        self.nr_cores = json["nr_cores"]
        self.core = Core(json["core"])

    def __str__(self):
        return "NUMADomain(nr_cores={}, core={})" \
            .format(
                self.nr_cores,
                self.core,
            )

    def nr_threads(self):
        return self.nr_cores * self.core.nr_threads


class Package(object):

    def __init__(self, json):
        self.nr_numa_domains = json["nr_numa_domains"]
        self.numa_domain = NUMADomain(json["numa_domain"])

    def __str__(self):
        return "Package(nr_numa_domains={}, numa_domain={})" \
            .format(
                self.nr_numa_domains,
                self.numa_domain,
            )

    def nr_threads(self):
        return self.nr_numa_domains * self.numa_domain.nr_threads()


class Node(object):

    def __init__(self, json):
        self.nr_packages = json["nr_packages"]
        self.package = Package(json["package"])

    def __str__(self):
        return "Node(nr_packages={}, package={})" \
            .format(
                self.nr_packages,
                self.package,
            )

    def nr_threads(self):
        return self.nr_packages * self.package.nr_threads()


class Scheduler(object):

    def __init__(self, json):

        self.kind = json["kind"]


class ShellScheduler(Scheduler):

    def __init__(self, json):

        Scheduler.__init__(self, json)


class SlurmSettings(object):

    def __init__(self, json):

        self.partition_name = json["partition"]
        self.sbatch_options = \
            json["sbatch_options"] if "sbatch_options" in json else ""
        self.srun_options = \
            json["srun_options"] if "srun_options" in json else ""


class SlurmScheduler(Scheduler):

    def __init__(self, json):

        Scheduler.__init__(self, json)
        self.settings = SlurmSettings(json["settings"])


class Cluster(object):

    def __init__(self, json):

        self.name = json["name"]
        scheduler_json = json["scheduler"]
        scheduler_kind = scheduler_json["kind"]

        if scheduler_kind == "shell":
            self.scheduler = ShellScheduler(scheduler_json)
        elif scheduler_kind == "slurm":
            self.scheduler = SlurmScheduler(scheduler_json)
            self.node = Node(json["node"])
