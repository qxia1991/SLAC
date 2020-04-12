import ROOT
import re
import datetime
import sys
import os.path
ROOT.gROOT.SetBatch()

class EXOTalkToHTMLOutput:
    list_type = """<ul style="list-style-type:none;">"""
    def __init__(self, output):
        self.out = output
    def output_initial_header(self, alist):
        self.out.write("""
    <html>
    <body>
    <p>Talk-to command reference, auto-generated on (UTC) %s</p>
    <a name="top"></a>
    %s
        """ % (str(datetime.datetime.utcnow()), self.list_type))
        for val in alist:
            self.out.write( """
    <li><a href="#%s">%s</a></li>""" % (val, val) )
        self.out.write( """
    </ul>
    <table border="1" cellpadding="5" cellpadding="0">
    <caption>Talk-to commands</caption> 
    <tr><th>Command</th><th>Description</th><th>Default value</th></tr>
""" )
    def output_header(self, title):
        self.out.write( """<tr><th colspan="3" align="left">  %s <a name="%s"></a><a href="#top">top</a></th></tr>
   """ % (title, title) )

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

def command_fcn(cmd):
    return ( cmd.fCommandName, cmd.fHelpMsg, cmd.fCmd.GetCurrentValue() )

def get_talk_to_doc(plugins):
    # Get all relevant classes
    base_class = ROOT.TClass.GetClass("EXOAnalysisModule")
    
    namespace_match = re.compile("EXO.*")
    
    # Grab all classes in a dictionary
    # with the following format:
    # { 'classname' : { 'class' : TClass ) ... }
    all_classes =  dict(
                   [ (myclass.GetName(), { 'class' : myclass }) for myclass in 

                       [ aclass for aclass in 
                         [ 
                           ROOT.TClass.GetClass(ROOT.gClassTable.At(i)) 
                             for i in range(ROOT.gClassTable.Classes()) 
                         ] 
                         if (aclass.InheritsFrom(base_class) and
                             namespace_match.match(aclass.GetName())) 
                       ]
                   ] 
                   )

    # Get initial set of commands 
    talkto   = ROOT.EXOTalkToManager()
    mgr      = ROOT.EXOAnalysisManager(talkto)
    commands = talkto.GetAllCommands()
    defined_commands = map( command_fcn, commands )
    all_classes['Management'] = { 'class' : None, 'cmds' : defined_commands[:] }
    
    # now loop over modules
    for class_name, thedict in all_classes.items():
        if class_name == 'Management': continue
        try:
            myclass = getattr(ROOT, class_name)() 
        except TypeError:
            # Abstract classes throw a typeerror
            thedict['cmds'] = [] 
            continue
        myclass.TalkTo( talkto )
        commands = talkto.GetAllCommands()
        temp_list = map( command_fcn, [ i for i in commands if command_fcn(i) 
                                        not in defined_commands ] )
        thedict['cmds'] = temp_list[:]
        defined_commands.extend( temp_list )

    for aplug in plugins:
        class_name = ROOT.EXOROOT.LoadPlugin(aplug)
        if class_name == "": continue
        amod = getattr(ROOT, class_name)()
        thedict = {}
        amod.TalkTo( talkto )
        commands = talkto.GetAllCommands()
        temp_list = map( command_fcn, [ i for i in commands if command_fcn(i) 
                                        not in defined_commands ] )
        thedict['cmds'] = temp_list[:]
        defined_commands.extend( temp_list )
        all_classes[amod.GetName() + ' (Plugin)'] = thedict 


    return all_classes

if __name__ == '__main__':
    ROOT.gErrorIgnoreLevel = ROOT.kError
    ROOT.gSystem.Load("libEXOROOT")
    ROOT.EXOErrorLogger.GetLogger().SetOutputThreshold("panic")
    adict = get_talk_to_doc(sys.argv[2:])
    keys = adict.keys()
    keys.sort()
    # Get Management to be first
    keys = keys[-1:] + keys[:-1]

    output = EXOTalkToHTMLOutput(open(sys.argv[1], 'w'))
    output.output_initial_header(map(lambda x: x.replace('EXOROOT::', ''), keys))
    for i in keys:
        output.output_header(i.replace('EXOROOT::',''))
        cmds = adict[i]['cmds']
        map(output.output_data, cmds)
        output.output_footer()

    output.output_final_footer()
