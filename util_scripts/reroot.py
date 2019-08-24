#!/usr/bin/env python
'''
Created on Jun 3, 2011

@author: smirarab
'''
import dendropy
import sys
#from dendropy.dataobject.taxon import Taxon
# uym2 editted: hack to move on to dendropy4
from dendropy.datamodel.taxonmodel import Taxon
from dendropy import TreeList

''' roots the tree on the given outgroups. Input is:
-- first argument: a path to the tree file
-- second argument: a list of outgroups, separated by comma. 
-- third argument: (optional) -mrca
The script goes through the list of outgroups. 
If the outgroup is found in the tree, the tree is rooted at that outgroup.
Otherwise, the next outgroup in the list is used. 
Each element in the camma-delimited list is itself a + delimited list of taxa.
By default the script makes sure that the set of outgroups used are monophyletic
in the tree and roots the tree at the node leading to the clade represented by outgroups.
If outgroups are not monophyletic, the first one is used.
Using -mrca this monophyletic requirement can be relaxed, so that MRCA is always used. 
'''

def main(args):    
    if len (args) < 2:
        print '''USAGE: %s [tree_file] [outgroups] [-mrca -mrca-dummy (optional)] [output name (optional)] [-igerr (optional)]

-- tree_file: a path to the newick tree file

-- outgroups: a list of outgroups, separated by comma.
The script goes through the list of outgroups. If the outgroup is found in the tree, 
the tree is rooted at that outgroup. Otherwise, the next outgroup in the list is used. 
Each element in the comma-delimited list is itself a + delimited list of taxa.
By default the script makes sure that this list of taxa are monophyletic
in the tree and roots the tree at the node leading to the clade represented 
by outgroups given in the + delimited list.
Alternatively, you can specify -m which will result in mid-point rooting.

Example: HUMAN,ANOCA,STRCA+TINMA first tries to root at HUMAN, if not present, 
tries to use ANOCA, if not present, tries to root at parent of STRCA and TINMA
which need to be monophyletic. If not monophyletic, roots at STRCA.

-- (optional) -mrca: using this option the mono-phyletic requirement is relaxed 
and always the mrca of the + delimited list of outgroups is used.
-- (optional) -mrca-dummy: is like -mrca, but also adds a dummy taxon as outgroup to the root. 
''' %args[0]
        sys.exit(1)
    treeName = args[1]

    outgroups = [x.replace("_"," ") for x in args[2].split(",")]

    # uym2 editted: keep underscore
    #outgroups = [x for x in args[2].split(",")]
    
    use_mrca = True if len(args) > 3 and (args[3] == "-mrca" or args[3] == "-mrca-dummy") else False
    add_dummy = True if len(args) > 3 and (args[3] == "-mrca-dummy") else False
    resultsFile= args[4] if len(args) > 4 else ("%s.rooted"%treeName[:-9] if treeName.endswith("unrooted") else "%s.rooted" % treeName)
    ignore= True if len(args) > 5 and args[5] == "-igerr" else False
    print >>sys.stderr, "Reading input trees %s ..." %treeName, 
    #trees = dendropy.treelist.get_from_path(treename, 'newick',rooted=true)
# uym2 edited: hack for dendropy4
    trees = dendropy.TreeList.get_from_path(treeName, "newick")
    print >>sys.stderr, "%d tree(s) found" %len(trees)
    i = 0;   
    outtrees=TreeList() 
    for tree in trees:
        i+=1
        print >>sys.stderr, ".",
        oldroot = tree.seed_node
        #print "Tree %d:" %i
        if outgroups[0] == "-m":            
            print >>sys.stderr, "Midpoint rooting ... "
            tree.reroot_at_midpoint(update_splits=False)
        else:             
            mrca = None
            for outgroup in outgroups:  
                outs = outgroup.split("+")
                outns = []                    
                for out in outs:          
                    n = tree.find_node_with_taxon_label(out)
                    if n is None:
                        print >>sys.stderr, "outgroup not found %s," %out,
                        continue            
                    outns.append(n.taxon)
                if len (outns) != 0:
                    # Find an ingroup and root the tree there
                    for n in tree.leaf_node_iter():
                        if n.taxon not in outns:
                            ingroup=n
                            break
                    #print "rerooting at ingroup %s" %ingroup.taxon.label
                    '''reroot at an ingroup, so that outgroups form monophyletic groups, if possible'''
                    if ingroup.edge.length is not None:
                        #tree.reroot_at_edge(ingroup.edge, update_splits=True,length1=ingroup.edge.length/2,length2=ingroup.edge.length/2)
                        # uym2 editted: hack for dendropy4
                        tree.reroot_at_edge(ingroup.edge,length1=ingroup.edge.length/2,length2=ingroup.edge.length/2)
                    else:
                        #tree.reroot_at_edge(ingroup.edge, update_splits=True)
                        tree.reroot_at_edge(ingroup.edge)

                    mrca = tree.mrca(taxa=outns)
                    break            
            if mrca is None:
                if ignore:
                   print  >>sys.stderr, "Outgroups not found: %s" %outgroups
                   print  >>sys.stdout, tree.as_string(schema="newick"), 
                   continue
                else:
                   print  >>sys.stderr, "Outgroups not found: %s" %outgroups
		   continue
                   #raise KeyError("Outgroups not found %d: %s" %(i,outgroups))
            #print mrca.leaf_nodes()
            #if not mono-phyletic, then use the first
            if not use_mrca and len (mrca.leaf_nodes()) != len(outns):
                print >>sys.stderr, "selected set is not monophyletic. Using %s instead. " %outns[0]
                mrca = tree.find_node_with_taxon_label(outns[0].label)
            if mrca.parent_node is None:
                print  >>sys.stderr, "Already rooted at the root."
                #print "rerooting on %s" % [s.label for s in outns]
                #tree.reroot_at_midpoint()
            elif mrca.edge.length is not None:
                #print "rerooting at %s" %mrca.as_newick_string()
                if ingroup.edge.length is not None:
                    #tree.reroot_at_edge(mrca.edge, update_splits=False,length1=mrca.edge.length/2,length2=mrca.edge.length/2)        
                    #uym2 editted: hack for dendropy4
                    tree.reroot_at_edge(mrca.edge, length1=mrca.edge.length/2,length2=mrca.edge.length/2)        
                else:
                    #tree.reroot_at_edge(mrca.edge, update_splits=False)        
                    #uym2 editted: hack for dendropy4
                    tree.reroot_at_edge(mrca.edge)        
            else:
                tree.reroot_at_edge(mrca.edge, update_splits=False)
            if add_dummy:
                dummy = tree.seed_node.new_child(taxon=Taxon(label="outgroup"),edge_length=1)
                tree.reroot_at_edge(dummy.edge, update_splits=False)
            outtrees.append(tree)
        '''This is to fix internal node labels when treated as support values''' 
        while oldroot.parent_node != tree.seed_node and oldroot.parent_node != None:
            oldroot.label = oldroot.parent_node.label
            oldroot = oldroot.parent_node
        if len(oldroot.sister_nodes()) > 0:
            oldroot.label = oldroot.sister_nodes()[0].label    
            #tree.reroot_at_midpoint(update_splits=False)

    print  >>sys.stderr, "writing results to %s" %resultsFile        
    #outtrees.write(open(resultsFile,'w'),'newick',edge_lengths=True, internal_labels=True,write_rooting=False)
    #uym2 editted: hack for dendropy4
    outtrees.write(path=resultsFile,schema='newick',suppress_rooting=True)#,edge_lengths=True, internal_labels=True,write_rooting=False)

if __name__ == '__main__':
    main(sys.argv)
