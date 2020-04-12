import re
from base_doc_output import BaseEXODocOutput

class EXOTreeAlias(BaseEXODocOutput):
    def output_function(self, previous, name, title, atype):
        title = re.sub('\s{3,}', ' ', title)
        temp = title.split(':')
        old_value = ' '
        new_value = temp[0]
        
        if len(temp) > 1:
            old_value = temp[0]
            new_value = temp[1]
    
        old_value = old_value.replace(' ', '')
        if old_value == "": return
        print """    tree->SetAlias("%s", "%s");""" % (old_value, previous+name)
    
    def output_initial_header(self):
        print """
void SetEXOAlias(TTree* tree)
{
"""

    def output_tree_header(self, tree_name, branch_name):
        print """
  if(string(tree->GetName()) == "%s") {
    if(!tree->GetBranch("%s")) {
        cout << "Can't find branch: %s in " << tree->GetName() << endl;
        cout << "Can't set aliases, returning..." << endl;
        return;
    } else if(!TClass::GetDict(tree->GetBranch("%s")->GetClassName()) && gSystem->DynamicPathName("libEXOUtilities", true)) {
      cout << "Loading libEXOUtilities, this will load the "
           <<tree->GetBranch("%s")->GetClassName()
           <<" dictionary" << endl;
      gSystem->Load("libEXOUtilities");
    }
    cout << "Setting aliases for tree \\"%s\\" ..." << endl;

""" % (tree_name, branch_name, branch_name, branch_name, branch_name, tree_name)

    def output_tree_footer(self):
        print """
  }
"""

    def output_footer(self):
        return
    
    def output_final_footer(self):
        print """
}
        """
    
    def output_function_data(self, previous, data_mem):
        self.output_function(previous, data_mem.GetName(), data_mem.GetTitle(), data_mem.GetTypeName()) 
    

