#!/usr/bin/env python
#coding=utf-8
# Function: translate the results from BTM
# Input:
#    mat/pw_z.k20

import sys

# return:    {wid:w, ...}
def read_voca(pt):
    voca = {}
    for l in open(pt):
        wid, w = l.strip().split('\t')[:2]
        voca[int(wid)] = w
    return voca

def read_pz(pt):
    return [float(p) for p in open(pt).readline().strip().split()]
    
# voca = {wid:w,...}
def read_topwords(pt, voca, pz):
    k = 0
    topics = []
    for l in open(pt):
        vs = [float(v) for v in l.split()]
        wvs = zip(range(len(vs)), vs)
        wvs = sorted(wvs, key=lambda d:d[1], reverse=True)
        tmps = ' '.join(['%s' % voca[w] for w,v in wvs[:10]])
        topics.append((pz[k], tmps))
        k += 1
        
    print 'p(z)\t\tTop words'
    for pz, s in sorted(topics, reverse=True):
        print '%f\t%s' % (pz, s)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: python %s <K>' % sys.argv[0]
        print '\tK is the number of topics'
        exit()
        
    K = int(sys.argv[1])
    data_dir = '../sample-data/'
    voca_pt = data_dir + 'voca.txt'
    voca = read_voca(voca_pt)    
    W = len(voca)
    print 'K:%d, n(W):%d' % (K, W)

    pz_pt = data_dir + '../output/pz.k%d' % K
    pz = read_pz(pz_pt)
    
    zw_pt = data_dir + '../output/pw_z.k%d' %  K
    read_topwords(zw_pt, voca, pz)
