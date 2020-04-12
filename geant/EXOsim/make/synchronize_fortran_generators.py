################################################################################
# synchronize_fortran_generators.py written by M. Marino Jun 2011.  
#
# Designed to sync fortran generators in the geant3 svn repo with the
# EXOOffline repo.
################################################################################
import os
import glob
import sys
import re
import tempfile
import shutil
from subprocess import call 

Usage = """
%s [path_to_sync]

running this program synchronizes the path_to_sync with the fortran generators
in the geant3 repository.
""" % sys.argv[0] 

base_server = "svn+ssh://svn.slac.stanford.edu/nfs/slac/g/exo/subversion/geant3/trunk"
base_server = "svn+ssh://svn.slac.stanford.edu/nfs/slac/g/exo/subversion/geant3/branches/DL2"
if len(sys.argv) != 2:
    print Usage
    sys.exit(1)
dest_dir = sys.argv[1] 
ignore_files = ["rn_decays.orig.f"]#[ "rn222.f", "th232.f", "u238.f"]

print "Pulling down the list of needed fortran generator files from: ", base_server 
tmp_directory_base = tempfile.mkdtemp() 
tmp_directory = os.path.join(tmp_directory_base, os.path.split(base_server)[1])

path_of = lambda x: x
ext_of = lambda x: os.path.splitext(path_of(x))[1]
local_path = lambda x: dest_dir + str(x).replace(tmp_directory,'').replace('/fgen/src','')
updated_files = [] 
try:
    
    print "Outputting to temp dir ==>", tmp_directory
    call(["svn", "export", base_server, tmp_directory])
    # Get the files to pull down
    # Include files
    include_files = glob.glob(tmp_directory + "/include/*.inc")
    include_files.extend(glob.glob(tmp_directory + "/include/*.h"))
    # Fortran src files
    src_files = glob.glob(tmp_directory + "/fgen/src/*.f")
    
    file_dict = {}
    print "Modifying to local directory structure and grabbing file contents..."
    for afile in include_files + src_files: 
        local_file = local_path(afile) 
        if os.path.split(local_file)[1] in ignore_files:
            print "Ignoring file:", local_file
            continue
        file_dict[local_file] = open(afile).read()

    print "Checking for necessary updates..."
    for local_file, contents in file_dict.items():
        if not os.path.exists(local_file) or open(local_file).read() != contents:
            print "Updating: ", local_file
            if not os.path.exists(os.path.split(local_file)[0]):
                os.makedirs(os.path.split(local_file)[0])
            open(local_file, "w").write(contents)
            updated_files.append(local_file)

    # Now trying to clean up things that may have been deleted
    loc_include_files = glob.glob(dest_dir + "/include/*.inc")
    # Fortran src files
    loc_src_files = glob.glob(dest_dir + "/*.f")
    for afile in loc_src_files + loc_include_files:
        if afile not in file_dict.keys():
            print "Local file %s not in svn repository, deleting..." % afile
            os.unlink(afile)
            updated_files.append(afile)
        
except Exception, e:
    print "Error processing: %s" % e

print "Removing temporary directory..."
shutil.rmtree(tmp_directory_base)
if len(updated_files) != 0:
    print "The following files were updated, you will need to commit the changes to svn:"
    for afile in updated_files: 
        prefix = " M "
        if not os.path.exists(afile): prefix = " D "
        print " %s ==> %s" % (prefix, afile)

print "Done."
