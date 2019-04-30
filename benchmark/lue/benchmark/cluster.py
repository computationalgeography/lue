class Cluster(object):

    def __init__(self, json):

        self.name = json["name"]


class ShellCluster(Cluster):

    def __init__(self, json):

        Cluster.__init__(self, json)


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


class SlurmCluster(Cluster):

    def __init__(self, json):

        Cluster.__init__(self, json)
        self.partition_name = json["partition_name"]
        self.node = Node(json["node"])

    def __str__(self):
        return "SlurmCluster(partition_name={}, node={})" \
            .format(
                self.partition_name,
                self.node,
            )
