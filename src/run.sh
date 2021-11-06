#make clean
#make all
FILES="../traces/*"
for (( g=2; g<=20; g=g+2 ))
	do
	echo "Processing $g bits"
	for f in $FILES
		do
		out="$(basename -s .bz2 $f)"
		echo "Processing $out file..."
		out_file="$out"
		bunzip2 -kc ../traces/$f | ./predictor --gshare:$g >> ../out/$out_file &
		done
	done
