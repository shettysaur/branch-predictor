#make clean
#make all
FILES="../traces/*"
#for (( g=2; g<=20; g=g+2 ))
g="13"
echo "Processing $g bits"
for f in $FILES
    do
    out="$(basename -s .bz2 $f)"
    echo "Processing $out file..."
    out_file="$out"
    echo "Gshare"
    bunzip2 -kc ../traces/$f | ./predictor --gshare:13 
    echo "Tournament"
    bunzip2 -kc ../traces/$f | ./predictor --tournament:9:10:10 
    echo "Custom"
    bunzip2 -kc ../traces/$f | ./predictor --custom
    done 
