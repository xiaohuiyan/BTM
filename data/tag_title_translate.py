tags = {}
voca = {}

wf  = open('tag_title_ids.txt', 'w')

for l in open('tag_title.txt'):
    tag, ws = l.strip().split('\t')[1].split('|')
    ws = ws.split()
    if not tags.has_key(tag):
        tags[tag] = len(tags)
        
    for w in ws:
        if not voca.has_key(w):
            voca[w] = len(voca)
            
    s = ' '.join([str(voca[i]) for i in ws])
    wf.write('%d\t%s\n' % (tags[tag], s))

wf = open('voca.txt', 'w')
wf.write(''.join(['%d\t%s\n' % (v, k) for k,v in sorted(voca.items(), key=lambda d:d[1])]))

wf = open('tags.txt', 'w')
wf.write(''.join(['%d\t%s\n' % (v, k) for k,v in sorted(tags.items(), key=lambda d:d[1])]))

         
         
         
                  
                  
