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
}
function dotvistex() {
    echo $graphdot | dot2tex -f tikz# > $fname.tex
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
function timing3() {
    echo "for i in {4..35..1}; do nauty/geng -q 9 i:((i+1)) | sed '1!d' | mytutte | sed '2!d'"
    for i in {4..35..2}; do
    nauty/geng -q 9 $i:$((i+1)) | sed '1!d' | mytutte | sed '2!d'
    done
}
function mpitiming1() {
    echo "for i in 4 8 16 32; do dretest hpr/edge15.dre | mpitutte i ((i * 3 / 2)) | sed '2!d'"
    for i in 4 8 16 32; do
    dretest hpr/edge15.dre | mpitutte $i $(($i * 3 / 2)) | sed '2!d'
    done
}
function mpitiming2() {
    echo "for i in {2..32}; do dretest hpr/edge13.dre | mpitutte 8 $i | sed '2!d'"
    for i in {2..32}; do
        dretest hpr/edge13.dre | mpitutte 8 $i | sed '2!d'
    done
}
function mpitiming3() {
    echo "for i in {4..35..2}; do nauty/geng -q 9 i:((i+1)) | sed '1!d' | mpitutte 8 12 | sed '2!d'"
    for i in {4..35..2}; do
    nauty/geng -q 9 $i:$((i+1)) | sed '1!d' | mpitutte 8 12 | sed '2!d'
    done
}
