from base_doc_output import BaseEXODocOutput
import datetime
import re
"""
<style type="text/css">
div{ color: gray; }
div.hidden {
  display:none;
}
</style>
<script type="text/javascript">
 function unhide(divID) {
 var item = document.getElementsByName(divID);
 for ( var i = 0; i < item.length; i++ )
   {
     var anitem = item[i];
     if (anitem.style.display != "inline") {
       anitem.style.display = "inline";
     } else {      
       anitem.style.display = "none";
     }
   }
 }
 </script>

<a href="javascript:unhide('hidden');">Show</a>
"""

class EXOTreeHTMLOutput(BaseEXODocOutput):
    def output_function(self, previous, name, title, atype):
        title = re.sub('\s{3,}', ' ', title)
        temp = title.split(':')
        old_value = ' '
        doc_value = temp[0]
        
        if len(temp) > 1:
            old_value = temp[0]
            doc_value = ' '.join(temp[1:])
        # Remove straggling comments
        doc_value = doc_value.replace("*/","")
    
        print """<tr><td><font color="gray">%s</font>%s  </td><td>%s </td><td> %s </td><td> %s</td></tr>""" % \
        (previous, name, atype, old_value, doc_value)
    
    def output_initial_header(self):
        print """
    <html>
    <body>
    <table border="1" cellpadding="5" cellpadding="0">
    <caption>EXO ROOT file description, auto-generated on (UTC) %s</caption> 
    <tr><th>Name</th><th>Type</th><th>Old/Short Name</th><th>Description</th></tr>
        """ % (str(datetime.datetime.utcnow()))
    
    def output_footer(self):
        return
    
    def output_final_footer(self):
        print """
    </table>
    </body>
    </html>
        """
    
    def output_header(self, title):
        print """<tr><th colspan="4" align="left">  %s</th></tr>""" % title 
          
    def output_function_data(self, previous, data_mem):
        self.output_function(previous, data_mem.GetName(), data_mem.GetTitle(), data_mem.GetTypeName()) 
    

