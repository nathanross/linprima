echo "file,func,callno,totaltime,numExecs,percentTotal" > tmp/profiny.flat.csv
cat tmp/profiny.out | sed -r "s/ [T#%]/ /g" | sed -r "s/ //g" |  sort -nr -t: -k4 | sed -r "s/:/,/g" >> tmp/profiny.flat.csv
rm tmp/profiny.out
