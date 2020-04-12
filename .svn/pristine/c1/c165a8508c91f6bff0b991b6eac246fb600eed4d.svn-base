import ROOT
import datetime
import sys
ROOT.gROOT.SetBatch()

class EXOSimHTMLOutput:
    list_type = """<ul style="list-style-type:none;">"""
    def __init__(self, output):
        self.out = output

    def output_initial_header(self, alist):
        self.out.write("""
    <html>
    <body>
    <p>EXOSim command reference, auto-generated on (UTC) %s</p>
    <a name="top"></a>
    %s
        """ % (str(datetime.datetime.utcnow()), self.list_type))
        self.output_tree_header(alist)
        self.out.write( """
    </ul>
    <table border="1" cellpadding="5" cellpadding="0">
    <caption>EXOSim commands</caption> 
    <tr><th>Command</th><th>Description</th><th>Default value</th></tr>
""" )
    def output_tree_header(self, adict):
       for key in sorted(adict.keys(), key=lambda x: hasattr(x,"GetPathName") and x.GetPathName() or x): 
           if str(key) == 'cmds': continue
           path_name = key.GetPathName()
           title = key.GetTitle()
           self.out.write("""
    <li><a name="%s_top"></a><a href="#%s">%s</a>: %s</li>
    """ % (self.norm_string(path_name), 
           self.norm_string(path_name), path_name, title))
           if len(adict[key].keys()) > 1:
               self.out.write(self.list_type)
               self.output_tree_header(adict[key])
               self.out.write("""
     </ul>
""")

    def norm_string(self, string):
        return str(string).replace("/","_")

    def output_all(self, adict):
       for key in sorted(adict.keys(), key=lambda x: hasattr(x,"GetPathName") and x.GetPathName() or x): 
           if str(key) != 'cmds':
               self.output_header(key.GetPathName())
               self.output_all(adict[key])
               continue
           for cmd in adict[key]:
               guide = ""
               for i in range(cmd.GetGuidanceEntries()):
                   guide += str(cmd.GetGuidanceLine(i)) + " " 
               self.output_data((cmd.GetCommandPath(), guide, cmd.GetCurrentValue())) 
        
    def output_header(self, title):
        self.out.write( """<tr><th colspan="3" align="left">  %s <a name="%s"></a><a href="#%s_top">top</a></th></tr>
   """ % (title, self.norm_string(title), self.norm_string(title)) )

    def output_footer(self):
        return

    def output_data(self, values):
        self.out.write( """<tr><td>%s  </td><td>%s </td><td> %s </td></tr>""" % values )

    def output_final_footer(self):
        self.out.write( """
    </table>
    </body>
    </html>
        """)


def add_tree_to_cmd_dict(atree):
    adict = {}
    for i in range(atree.GetTreeEntry()): 
        the_tree = atree.GetTree(i+1)
        adict[the_tree] = add_tree_to_cmd_dict(the_tree)
    adict['cmds'] = []
    for cmd in range(atree.GetCommandEntry()): adict['cmds'].append(atree.GetCommand(cmd+1))
    return adict

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print "Usage: [output.html] [libEXOROOT] [EXOGeant4Module plugin]"
        sys.exit(1)
    ROOT.gErrorIgnoreLevel = ROOT.kError
    ROOT.gSystem.Load(sys.argv[2])
    ROOT.EXOErrorLogger.GetLogger().SetOutputThreshold("panic")
    sim_class_name = ROOT.EXOROOT.LoadPlugin(sys.argv[3])
    if sim_class_name == "":
        print "Problem loading sim module"
        sys.exit(1)
    sim = getattr(ROOT, sim_class_name)()
    sim.Initialize()
    exo_cmd_list = add_tree_to_cmd_dict(ROOT.G4UImanager.GetUIpointer().GetTree())

    output = EXOSimHTMLOutput(open(sys.argv[1], 'w'))
    output.output_initial_header(exo_cmd_list)
    output.output_all(exo_cmd_list)
    output.output_final_footer()
