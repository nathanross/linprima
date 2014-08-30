#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR;
rm -rf cmpR_tmp
rm -f codetotest.js
rm -rf cmpR_out
mkdir cmpR_tmp 
mkdir cmpR_tmp/files/

cat keylist.txt vallist.txt | tr "\n" "|" > cmpR_tmp/valid.txt

for i in `ls srcs`;
do
#function of these in order.
#1. cp, node: run esprima on the file
#2. sed: find string keys, string values, and string array values so long as the string only contains alphanumeric and underscore (this also excludes the problems of strings with escaped quotes). Put those strings on their own line.
#3. grep: filter out any lines which do not exactly match one of the terms linprima might generate or add to any file with certain syntax, then grep out blank lines
#4. sort, uniq -c: (sort is needed for uniq to work) count number of times each line is copied.
#5. sed:
#    'uniq -c' outputs in a format like this:
#     50 if
#    124 var
#  then sed outputs the result in this format:
#50,if
#124,var

cp srcs/$i cmpR_tmp/codetotest.js && node spaceless_esprima.js | sed -r "s/[,{:]\"([a-zA-Z_0-9<>]+)\"/\n\1\n/g" | grep -xE -f cmpR_tmp/valid.txt | grep -vE "^$" | sort | uniq -c | sed -r 's/^\s*([0-9]+)\s(.+)\s*$/\1,\2/g' > cmpR_tmp/files/$i ;
done

#functions in order

#sort so all counts for a particular keyword are grouped together
#use awk, set delimeter to comma. if on the first line, set x to keyword, set total to 0
#     every line, add the count to the total
#     if the new line's keyword is different than x, print x and the total, and once again set x to keyword and total to 0
#     once we get to EOF, because the last keyword never ran into a change, print that keyword and its total here.
# sort by the totals (t, : comma as delimiter, -k1 1st column, -nr numeric sort, higher numbers at the top
# cut out the total field.
# surround the terms 

cat cmpR_tmp/files/*.js | sort -t, -k2 | awk -F, '{ if (NR ==0 || (NR > 0 && term != $2)) { if (NR > 0) { print total "," term; } total =0; term=$2; } total += $1 } END { print total "," term }' | sort -t, -k1 -nr | cut -d"," -f2 | grep -vE "^$" > cmpR_tmp/ordered.txt

#for each term in this list that is not already in the ordered text, append it to the end of the ordered text
for term in `cat keylist.txt vallist.txt`; 
do 
    if ! grep -x $term cmpR_tmp/ordered.txt > /dev/null; 
    then 
        echo $term >> cmpR_tmp/ordered.txt; 
    fi; 
done

mkdir cmpR_out
python generateJsonCompressors.py
wait $!
#rm -rf cmpR_tmp
