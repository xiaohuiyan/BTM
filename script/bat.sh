#!/bin/bash
# NOTICE: prepare doc_pt
#  mkdir lda/stat lda/mat lda/res 

k=20   # number of topics
W=16856 # vocabulary size

alpha=`echo "scale=3;50/$k"|bc`
beta=0.01
niter=500
save_step=101

doc_pt=../sample-data/doc_wids.txt
res_dir=../output/

make -C ../src
# learning parameters p(z) and p(z|w)
echo "=============== Topic Learning ============="
../src/btm est $k $W $alpha $beta $niter $save_step $doc_pt $res_dir

# infer p(z|d) for each doc
echo "================ Infer P(z|d)==============="
../src/btm inf sum_b $k $doc_pt ${res_dir}

# output top words of each topic
echo "================ Topic Display ============="
python tran.py $k
