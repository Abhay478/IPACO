# cmake . && make LLVMmy_cp

# for i in `ls $1/assign` ; do
#     # echo "Running on $i"
#     $L_BIN/opt -enable-new-pm=0 -load=my_cp/libLLVMmy_cp.so --libCP_given < $1/assign/$i
#     cp output.txt $1/output/${i%.*}.txt
# done
# # $L_BIN/opt -enable-new-pm=0 -load=my_cp/libLLVMmy_cp.so --libCP_given < $1


cmake . && make LLVMmy_cp

for i in `ls $1` ; do
    # echo "Running on $i"
    $2/opt -enable-new-pm=0 -load=my_cp/libLLVMmy_cp.so --libCP_given < $1/$i
    cp output.txt $3/${i%.*}.txt
done
# $L_BIN/opt -enable-new-pm=0 -load=my_cp/libLLVMmy_cp.so --libCP_given < $1
