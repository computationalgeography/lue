import os.path
from docutils import nodes
from docutils.parsers.rst import directives
from docutils.parsers.rst import Directive

# http://docutils.sourceforge.net/docs/ref/rst/directives.html#image


def setup(app):
    app.add_config_value("generated_image_root", None, "html")

    app.add_node(
        GeneratedImage, html=(visit_generated_image_node, depart_generated_image_node)
    )

    app.add_directive("generated_image", GeneratedImageDirective)

    # app.connect("doctree-resolved", process_generated_image_nodes)


class GeneratedImage(nodes.Body, nodes.Element):
    pass


def visit_generated_image_node(self, node):
    self.visit_image(node)


def depart_generated_image_node(self, node):
    self.depart_image(node)


class GeneratedImageDirective(Directive):

    required_arguments = 1
    has_content = False

    def run(self):
        env = self.state.document.settings.env
        image_root = env.config.generated_image_root
        uri = directives.uri(self.arguments[0])

        if not image_root is None:
            uri = os.path.join(image_root, uri)

        self.options["uri"] = uri

        image_node = GeneratedImage(rawsource=self.block_text, **self.options)

        return [image_node]


# def process_generated_image_nodes(
#         app,
#         doctree,
#         fromdocname):
#
#     image_root = app.config.generated_image_root
#
#     if not image_root is None:
#         for node in doctree.traverse(GeneratedImage):
#             # print(node)
#             # print(type(node))
#             print(dir(node))
#             print(node.attributes)
#             # node.uri = os.path.join("meh", node.uri)
