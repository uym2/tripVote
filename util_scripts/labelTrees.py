#! /usr/bin/env python

from dendropy import Tree, TaxonNamespace

def read_label_from_primary_tree(t,prefix="I"):
    label_mapping = {}
    nextID = 0
    t.encode_bipartitions()
    
    for node in t.preorder_node_iter():
        if not node.is_leaf():
            label_mapping[node.bipartition] = node.label
            nextID = max(nextID,float(node.label[len(prefix):])) 

    
    return label_mapping, nextID+1
    

def label_primary_tree(t,prefix="I"):
    label_mapping = {}
    labelID = 0
    t.encode_bipartitions()
    for node in t.preorder_node_iter():
        if not node.is_leaf():
            node.label = prefix + str(labelID)
            label_mapping[node.bipartition] = node.label
            labelID += 1

    
    return label_mapping, labelID
    

def label_secondary_tree(t,label_mapping,startID,prefix="I"):
    t.encode_bipartitions()
    labelID = startID
    for node in t.preorder_node_iter():
        if not node.is_leaf():
            key = node.bipartition            
            if key in label_mapping:
                node.label = label_mapping[key]
            else:
                node.label = prefix + str(labelID)
                label_mapping[key] = node.label
                labelID += 1    
    

    return label_mapping, labelID
                  
                  
def main():

    import argparse

    parser = argparse.ArgumentParser()

    parser.add_argument("-i","--input",required=True,help="Input trees")
    parser.add_argument("-o","--output",required=True,help="Output trees")
    parser.add_argument("-p","--primary",required=False,help="Primary tree: if specified, the input trees will be labeled according to this tree. Must specify the prefix via -r option when you have a primary tree")
    parser.add_argument("-r","--prefix",required=False,help="Prefix: all labels will be added this prefix. Default:I")

    args = vars(parser.parse_args())

    inputfiles = args["input"].split()
    outputfiles = args["output"].split()
    primeFile = args["primary"] if args["primary"] else None
    prefix = args["prefix"] if args["prefix"] else "I"

    if not (len(outputfiles) == 1 or len(outputfiles) == len(inputfiles)):
        print("The number of output files must either be 1 or the same as the number of input files!")
    else:
        multi_output = len(outputfiles) > 1
         
        if not multi_output:
            fout = open(outputfiles[0],'w')
        
        is_primary = True
        taxa = TaxonNamespace()
   
        if primeFile:
            tree = Tree.get_from_path(primeFile,"newick",taxon_namespace=taxa,rooting="force-rooted")
            label_mapping, labelID = read_label_from_primary_tree(tree,prefix=prefix)
            is_primary = False

    
    # Although using TreeList provided in Dendropy can be a more convenient solution,
    # I opted out for that because it requires storing a large number of trees in the memory at the same time
    # If the input trees are big then we will run out of memory 
    # Had problem with a set of 7k trees of 10k leaves which required >60G of memory just to store the trees
    # Here I read each tree and label it one-by-one. 
    #Just have to be thoughtful about making the taxon_namespace shared among all the trees
        for i,filein in enumerate(inputfiles):
            if multi_output:
                fout = open(outputfiles[i],'w')
            with open(filein,'r') as fin:
                strings = fin.readlines()       
                for s in strings:
                    tree = Tree.get(data=s,schema="newick",taxon_namespace=taxa,rooting="force-rooted")
                    if is_primary:
                        label_mapping, labelID = label_primary_tree(tree,prefix=prefix)
                        is_primary = False
                    else:
                        label_mapping, labelID = label_secondary_tree(tree,label_mapping,labelID,prefix=prefix)
                    fout.write(tree.as_string("newick"))
            if multi_output:
                fout.close()
        
        if not multi_output:        
            fout.close()                                     

if __name__=="__main__":
    main()                                
