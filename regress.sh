function mytutte {
    ./tutte
}
function mpitutte {
    mpirun -n $1 ./tutte-mpi -s$2
}
function hprtutte() {
    gtoHPRg | hprtutte/tutte/tutte --stdin
}
function gtoHPRg() {
    nauty/showg -qe | sed -e '2!d' -e 's/  /,/g' -e 's/ /--/g'
}
function gengtest() {
    nauty/geng -q $1 | sed $2'!d'
}
function dretest() {
    nauty/dretog -q graphs/$1
}
function dotviz() {
    nauty/showg -d | nauty/dretodot | dot -Tpdf -o output/viz/$1  2> /dev/null
# echo $graphdot | dot2tex -f tikz > $fname.tex
# pdflatex $fname.tex
}
function multigengtest() {
    mkdir -p output/geng
    for i in `seq 2 $2`; do
#        gengtest $1 $i | dotviz geng-$1-$i.pdf
        gengtest $1 $i | mytutte > output/geng/mytutte-$1-$i.out
        gengtest $1 $i | hprtutte > output/geng/hprtutte-$1-$i.out
        ./polyDiff.py $1 output/geng/mytutte-$1-$i.out output/geng/hprtutte-$1-$i.out
    done
}
function mpimultigengtest() {
    mkdir -p output/geng
    for i in `seq 2 $2`; do
        gengtest $1 $i | mpitutte $3 > output/geng/mytutte-$1-$i.out
        gengtest $1 $i | hprtutte > output/geng/hprtutte-$1-$i.out
        ./polyDiff.py $1 output/geng/mytutte-$1-$i.out output/geng/hprtutte-$1-$i.out
    done
}
function multidretest() {
    mkdir -p output/$1
    for i in $( ls graphs/$1 ); do
#        dretest $1/$i | dotviz dre-$1-$i.pdf
        dretest $1/$i | mytutte > output/$1/mytutte-$i.out
        dretest $1/$i | hprtutte > output/$1/hprtutte-$i.out
        n=$( cat graphs/$1/$i | grep -o -E 'n=[0-9]+' | sed 's/[^0-9]*//g' )
        ./polyDiff.py $n output/'$1'/mytutte-$i.out output/'$1'/hprtutte-$i.out
    done
}
function regress() {
    multigengtest 2 2
    multigengtest 3 4
    multigengtest 4 11
    multigengtest 5 34
    multigengtest 6 156
    #multigengtest 7 1044
    #multidretest test
}
function timing() {
    for i in `seq 12 35`; do
    #nauty/geng -q 9 $i:$((i+1)) | sed '1!d' | mytutte | sed '2!d'
    nauty/geng -q 9 $i:$((i+1)) | sed '1!d' | mpitutte 4 12 # | sed '2!d'
    #nauty/geng -q 7 | sed $i'!d' | nauty/showg -qe | sed '1!d'
    done
}
