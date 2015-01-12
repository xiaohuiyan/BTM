#!/bin/bash
# run an toy example for BTM

K=20   # number of topics
W=16856 # vocabulary size

alpha=`echo "scale=3;50/$K"|bc`
beta=0.01
niter=500
save_step=101

input_dir=../sample-data/
# the input docs for training
doc_pt=${input_dir}doc_info.txt

echo "=============== Index Docs ============="
# docs after indexing
dwid_pt=${input_dir}doc_wids.txt
# vocabulary file
voca_pt=${input_dir}voca.txt
python indexDocs.py $doc_pt $dwid_pt $voca_pt

## learning parameters p(z) and p(z|w)
echo "=============== Topic Learning ============="
make -C ../src
model_dir=../output/
echo "../src/btm est $K $W $alpha $beta $niter $save_step $dwid_pt $model_dir"
../src/btm est $K $W $alpha $beta $niter $save_step $dwid_pt $model_dir

## infer p(z|d) for each doc
echo "================ Infer P(z|d)==============="
echo "../src/btm inf sum_b $K $dwid_pt $model_dir"
../src/btm inf sum_b $K $dwid_pt $model_dir

## output top words of each topic
echo "================ Topic Display ============="
python topicDisplay.py $model_dir $K $voca_pt
