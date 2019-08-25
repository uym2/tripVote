#! /bin/bash

data=$1

cd $data

echo "Running tripRoot ..."
tripRoot $data-taxonomy.tre $data.trees $data\_tripRoot.trees > $data\_tripRoot.log 2>&1 

#echo "Running MV ..."
#python ~/Documents/my_gits/MinVar-Rooting/FastRoot.py -i $data.trees -o $data\_MV.trees -m MV 

#echo "Running MP ..."
#python ~/Documents/my_gits/MinVar-Rooting/FastRoot.py -i $data.trees -o $data\_MP.trees -m MP 

echo "Running TreeCmp - tripRoot ..."
method=tripRoot; java -jar ~/Documents/Packages_N_Libraries/TreeCmp/bin/TreeCmp.jar -m -d mc -i $data\_$method.trees -o $data\_$method.dmc_pairs -N -P > $method_dmc.log 2>&1

#echo "Running TreeCmp - MP ..."
#method=MP; java -jar ~/Documents/Packages_N_Libraries/TreeCmp/bin/TreeCmp.jar -m -d mc -i $data\_$method.trees -o $data\_$method.dmc_pairs -N -P > $method_dmc.log 2>&1 &

#echo "Running TreeCmp - MV ..."
#method=MV; java -jar ~/Documents/Packages_N_Libraries/TreeCmp/bin/TreeCmp.jar -m -d mc -i $data\_$method.trees -o $data\_$method.dmc_pairs -N -P > $method_dmc.log 2>&1 &

#echo "Running TreeCmp - OG ..."
#method=OG; java -jar ~/Documents/Packages_N_Libraries/TreeCmp/bin/TreeCmp.jar -m -d mc -i $data\_$method.trees -o $data\_$method.dmc_pairs -N -P > $method_dmc.log 2>&1 &
