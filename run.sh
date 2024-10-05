if [[ $# -ne 1 ]]; then
	echo "Wrong argcount. Exiting."
	exit 1
fi

if [[ $1 == "cln" ]]; then
	if [[ -d "build/" ]]; then
		rm -rf "build/"
	fi
	cmake -S "." -B "build/"
	cd "build/"
	make
	./exe
	cd ".."
elif [[ $1 == "cmk" ]]; then
	cmake -S "." -B "build/"
	cd "build/"
	make
	./exe
	cd ".."
elif [[ $1 == "mk" ]]; then
	cd "build/"
	make
	./exe
	cd ".."
elif [[ $1 == "run" ]]; then
	cd "build/"
	./exe
	cd ".."
else
	echo "Wrong argument. Use cln, cmp or run. Exiting."
	exit 1
fi
