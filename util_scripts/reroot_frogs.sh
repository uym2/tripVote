#!/bin/sh
reroot.py $1 Latimeria_chalumnae_Latimeriidae,Protopterus_annectens_Protopteridae,Homo_sapiens_Hominidae,Crocodylus_siamensis_Crocodylidae+Gallus_gallus_Phasianidae,Ichthyophis_bannanicus_Ichthyophiidae,Batrachuperus_yenyuanensis_Hynobiidae+Andrias_davidianus_Cryptobranchidae -mrca $2

if [ ! -s $2 ]; then 
	echo "Rerooting using midpoint ..." 
	FastRoot.py -i $1 -o $2 -m MP
fi
