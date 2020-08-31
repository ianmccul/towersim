for i in 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do (../../translate-methods plain $i cc-collection/plain$i.txt > plain$i.txt) ; done
for i in 6 8 10 12 14 16 ; do (../../translate-methods surprise $i cc-collection/surp$i.txt > surprise$i.txt) ; done
for i in 5 6 8 10 12 14 16 ; do (../../translate-methods alliance $i cc-collection/all$i.txt > alliance$i.txt) ; done
for i in 6 8 10 12 14 16 ; do (../../translate-methods delight $i cc-collection/del$i.txt > delight$i.txt) ; done
for i in 6 8 10 12 14 16 ; do (../../translate-methods treblebob $i cc-collection/tb$i.txt > treblebob$i.txt) ; done
for i in 5 6 7 8 9 10 11 12 16 ; do (../../translate-methods trebleplace $i cc-collection/tpl$i.txt > trebleplace$i.txt) ; done
for i in 4 5 6 ; do (../../translate-methods hybrid $i cc-collection/hyb$i.txt > hybrid$i.txt) ; done
for i in 4 5 6 7 8 9 10 11 12 13 15 ; do (../../translate-methods principle $i cc-collection/prin$i.txt > principle$i.txt) ; done
