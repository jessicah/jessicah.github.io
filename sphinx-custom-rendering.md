# Custom HTML Rendering for Sphinx Documentation

To override the rendering of HTML output with the Sphinx documentation generator, it can be achieved
by creating your own builder and translator classes.

The gist looks as follows:

  def setup(app):
    from docutils import nodes
    from docutils.nodes import Element
    
    from sphinx.application import Sphinx
    from sphinx.environment import BuildEnvironment
    from sphinx.builders import Builder
    from sphinx.builders.html import StandaloneHTMLBuilder
    from sphinx.writers.html5 import HTML5Translator
    
    class CustomBuilder(StandaloneHTMLBuilder):
      def __init__(self, app: Sphinx, env: BuildEnvironment = None) -> None:
        super().__init__(app, env)
        
      @property
      def default_translator_class(self):
        return CustomHTMLTranslator
      
    class CustomHTMLTranslator(HTML5Translator):
      def __init__(self, document: nodes.document, builder: Builder) -> None:
        super().__init__(document, builder)
      
      # then overwrite the visit/depart functions as required

Majority of the inspiration came from http://www.arnebrodowski.de/blog/write-your-own-restructuredtext-writer.html,
with their excellent write up of the basics.

Unfortunately, at first I thought I could just use `app.set_translator`, but this didn't seem to do as I wanted.
Perhaps I needed to use a different name, e.g. `app.set_translator('html', CustomHTMLTranslator, True)`. I shall
try that later, but for now, the above works even directly in the `conf.py` file.
