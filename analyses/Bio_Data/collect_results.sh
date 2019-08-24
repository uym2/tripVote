grep 0 */*dmc_pairs | awk '{print $1, $9;}' | sed -e "s/.*\///g" -e "s/\.dmc.* / /g" -e "s/_/ /g" > dmc.dat
