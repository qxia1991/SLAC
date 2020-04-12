import ROOT
import re
import sys
from base_doc_output import BaseEXODocOutput
ROOT.gROOT.SetBatch()

OutputClass = BaseEXODocOutput() 
use_tree_vars = True

__getter_map = {
  "fUWires" : "GetUWireSignalArray", 
  "fUWiresInduction" : "GetUWireInductionSignalArray" 
}

def return_Get_function(aclass, array_method): 
    get_name = array_method.GetName().replace("Array", "")
    if aclass.GetMethodAllAny(get_name) != None: return get_name
    # Try without 's'
    if aclass.GetMethodAllAny(get_name[:-1]) != None: return get_name[:-1]
    return "Unknown"

def generate_doc_for_object(obj, previous = "", atitle=""):
    the_class = obj.Class()
    data_list = the_class.GetListOfDataMembers() 

    get_list  = [aname for aname in the_class.GetListOfAllPublicMethods() 
                   if re.match("Get.*Array()", aname.GetName()) ]
    get_ref_list  = [aname for aname in the_class.GetListOfAllPublicMethods() 
                       if (re.match("Get.*()", aname.GetName()) and not aname in get_list)  ]

    save_list = []
    for adat in data_list: 
        # Skip data members that aren't persistent (cache-only)
        if not adat.IsPersistent(): continue
        try:
            # TODO the following is part of the ongoing saga with ROOT's issues with set<UInt_t>
            # This should be removed in the future.
            if adat.GetTypeName() == "set<UInt_t>":
                raise AttributeError 
            attr = getattr(ROOT, adat.GetTypeName())
            save_list.append((adat, attr))
        except AttributeError: 
            OutputClass.output_function_data(previous, adat)

    for adat, attr in save_list:
        if hasattr(attr, "Class") and attr.Class() == ROOT.TClass.Class(): continue
        if hasattr(attr, "Class"):
            if attr.Class().InheritsFrom(ROOT.TCollection.Class()) and \
               not attr.Class().InheritsFrom(ROOT.TRefArray.Class()): 
                if not hasattr(obj, adat.GetName()): 
                    # Means it's protected, try to grab with an associated getter function
                    aname = adat.GetName()
                    if aname[0] == 'f': aname = aname[1:]
                    if aname[-1] == 's': aname = aname[:-1]
                    match_it_clus = re.match("(\w*)Cluster", aname) 
                    if match_it_clus: aname = match_it_clus.group(1) 
                    # Now see if we can find it in the getters
                    final_list = [mem for mem in get_list 
                                    if re.match(".*%s.*" % aname, mem.GetName())]
                    # Deal specially with Cluster objects
                    if len(final_list) != 1 and match_it_clus: 
                        final_list = [mem for mem in final_list if re.match(".*Cluster.*", mem.GetName())] 

                    # Deal with special cases
                    if adat.GetName() in __getter_map:
                        _aname = __getter_map[adat.GetName()]
                        final_list = [mem for mem in get_list 
                                        if mem.GetName() == _aname]

                    if len(final_list) != 1: 
                        print >> sys.stderr, "Error Finding", adat.GetName(), final_list
                        return
                    next_obj = getattr(obj, final_list[0].GetName())().New(0)
                    # Check for GetNum                         
                    get_new_list = [mem for mem in get_ref_list 
                                    if re.match("GetNum%s.*" % aname, mem.GetName())]

                    next_name = next_obj.Class().GetName() 
                    if not use_tree_vars:
                        next_name += (" " + return_Get_function(the_class, final_list[0]) + "(#)") 
                    OutputClass.output_header(next_name)
                    if len(get_new_list) == 1:
                        temp_dat = get_new_list[0]
                        size_name = "@" + adat.GetName()+".size()"
                        if not use_tree_vars:
                            size_name = temp_dat.GetName() + "()"
                        OutputClass.output_function(previous, size_name, temp_dat.GetTitle(), "Int_t")

                    next_name = adat.GetName() + "."
                    if not use_tree_vars:
                        next_name = return_Get_function(the_class, final_list[0]) + "(#)->" 
                    generate_doc_for_object(next_obj, previous + next_name)
  
            elif attr.Class() == ROOT.TRef.Class() or attr.Class().InheritsFrom(ROOT.TRefArray.Class()): 
                # Dealing with TRefs 
                aname = adat.GetName()
                aname = aname.replace('fRef', '')
                if aname[-1] == 's': aname = aname[:-1]
                match_it = re.match("(\w*)Cluster", aname) 
                if match_it: aname = match_it.group(1) 
                final_list = [mem for mem in get_ref_list 
                                if re.match("Get.*%s.*" % aname, mem.GetName())]
                if len(final_list) != 1: 
                    for i in final_list[:]: 
                        return_type_name = i.GetReturnTypeName().replace('*', '')
			# Remove methods that we don't know about, that have
			# const in the return name, or have more than one
			# argument.  The latter requirement is to limit it to
			# array-like access.
			if not ROOT.TClass.GetClass(return_type_name) or \
                           re.match("const.*", return_type_name) or      \
                           i.GetListOfMethodArgs().GetSize() > 1:
                            final_list.remove(i)
                            if not use_tree_vars:
                                OutputClass.output_function(previous, i.GetName() + "()", i.GetTitle(), i.GetReturnTypeName())
                if len(final_list) != 1: 
                    # Check if it still is
                    print >> sys.stderr, "Error Finding", adat.GetName(), final_list
                    return
                atype = final_list[0].GetReturnTypeName().replace('*','')
                aclass = attr.Class().GetName()
                aprev = previous + adat.GetName() + "."
                if not use_tree_vars:
                    aclass = atype
                    atype = final_list[0].GetName() + "()"
                    aprev = previous
                OutputClass.output_function(aprev, atype, adat.GetTitle(), aclass)
            else: 
                next_obj = getattr(ROOT, adat.GetTypeName())() 
                OutputClass.output_header(next_obj.Class().GetName())
                generate_doc_for_object(next_obj,
                    previous + adat.GetName() + ".") 
    OutputClass.output_footer()


if __name__ == '__main__':

    if len(sys.argv) > 1:
        out = sys.argv[1]
        if out == "html": 
            from generate_documentation_tree_html import\
                EXOTreeHTMLOutput 
            OutputClass = EXOTreeHTMLOutput() 
        elif out == "basic": 
            from generate_documentation_basic import\
                EXOBasicHTMLOutput 
            OutputClass = EXOBasicHTMLOutput() 
        elif out == "html+": 
            from generate_documentation_tree_html import\
                EXOTreeHTMLOutput 
            OutputClass = EXOTreeHTMLOutput() 
            use_tree_vars = False
        elif out == "alias": 
            from generate_tree_aliases import\
                EXOTreeAlias 
            OutputClass = EXOTreeAlias() 

    ROOT.gSystem.Load("libEXOUtilities")

    # Parse listing of trees from EXOMiscUtil
    TreeList = []
    for member in ROOT.EXOMiscUtil.__dict__.items():
        if member[0].endswith("TreeName"):
          position = member[0].find("TreeName")
          TreeList.append(member[0][3:position])

    OutputClass.output_initial_header() 

    for name in TreeList:
        try:
            classname = getattr(ROOT.EXOMiscUtil, "Get%sClassName" % name)()
            treename = getattr(ROOT.EXOMiscUtil, "Get%sTreeName" % name)()
            branchname = getattr(ROOT.EXOMiscUtil, "Get%sBranchName" % name)()
            OutputClass.output_tree_header(treename, branchname)
            OutputClass.output_header(classname)
            generate_doc_for_object(getattr(ROOT, classname)())
            OutputClass.output_tree_footer()
        except AttributeError: pass

    OutputClass.output_final_footer()
