class Cluster(object):

    def __init__(self, json):

        self.name = json["name"]


class SlurmCluster(Cluster):

    def __init__(self, json):

        Cluster.__init__(self, json)
        self.partition_name = json["partition_name"]


class ShellCluster(Cluster):

    def __init__(self, json):

        Cluster.__init__(self, json)
