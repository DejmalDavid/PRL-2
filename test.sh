#!/bin/bash

if [ $# -ne 1 ];then 
    echo "Wrong argument's! Usage: test.sh nadmorske_vysky"
    exit 1
fi

pocet=0

IFS=',' read -ra ADDR <<< "$1"
for i in "${ADDR[@]}"; do
   pocet=$((pocet + 1))
done 

pocet=$((pocet - 1))
#echo $pocet

if [ $pocet -eq 0 ];then 
    echo "_"
    exit 0
fi

pocet=$(bc -l <<< "x=l($pocet)/l(2); scale=0; 2^(x/1)" )

#echo Pocet procesoru $pocet



#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o vid vid.cpp

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $pocet vid $1

#uklid
rm -f vid 

