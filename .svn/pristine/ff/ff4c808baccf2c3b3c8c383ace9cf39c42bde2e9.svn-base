import datetime
from base_doc_output import BaseEXODocOutput

class EXOBasicHTMLOutput(BaseEXODocOutput):

    list_type = """<ul style="list-style-type:none;">"""
    base_url = "http://exo-data.slac.stanford.edu/exodoc/"
    def output_initial_header(self):
        print """
    <html>
    <body>
    <p>EXO ROOT file tree-structure, auto-generated on (UTC) %s""" % (str(datetime.datetime.utcnow()))

    def output_tree_header(self, tree_name, branch_name):
        print """
    <p>
    \"%s\" tree, containing branch \"%s\"
    %s""" % (tree_name, branch_name, self.list_type)

    def output_tree_footer(self):
        print """</ul></p>"""
     
    def output_header(self, title):
        print """<li><a href="%s%s.html">%s</a></li>
%s""" % (self.base_url, title, title, self.list_type)

    def output_final_footer(self):
        print """
    </body>
    </html>
        """

    def output_footer(self):
        print "</ul>"
        return
 

