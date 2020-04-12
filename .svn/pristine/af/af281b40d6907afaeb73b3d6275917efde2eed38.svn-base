import re
class BaseEXODocOutput:
    def output_function(self, previous, name, title, atype):
        title = re.sub('\s{3,}', ' ', title)
        temp = title.split(':')
        old_value = ' '
        new_value = temp[0]
        
        if len(temp) > 1:
            old_value = temp[0]
            new_value = temp[1]
    
    
    def output_initial_header(self):
        return

    def output_tree_header(self, tree_name, branch_name):
        return

    def output_tree_footer(self):
        return
   
    def output_footer(self):
        return
    
    def output_final_footer(self):
        return
   
    def output_header(self,title):
        return
          
    def output_function_data(self, previous, data_mem):
        return
    
    
