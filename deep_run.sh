cmake . && make LLVMdeep_cp

for i in `ls $1` ; do
    # echo "Running on $i"
    $2/opt -enable-new-pm=0 -load=deep_cp/libLLVMdeep_cp.so --libCP_given < $1/$i
    cp output.txt $3/${i%.*}.txt
done