#/bin/sh
cd $1

SNDFILE_REGEX='.*libsndfile.1.dylib\>'

LIB_PATH=lib

if [ "$2" = "bundle" ]; then
	LIB_PATH=../Frameworks
fi	

# Update CsoundLib
LIBSNDFILE_PATH=$(otool -L CsoundLib64 | grep -o $SNDFILE_REGEX)
echo sndfile path : $LIBSNDFILE_PATH
install_name_tool -id @executable_path/$LIB_PATH/CsoundLib64 CsoundLib64
install_name_tool -change $LIBSNDFILE_PATH @executable_path/$LIB_PATH/libsndfile.1.dylib CsoundLib64
otool -L CsoundLib64

# Update dependencies
for i in *.dylib; do
	echo Processing $i
	install_name_tool -id @executable_path/$LIB_PATH/$i $i

	# Update sndfile dependency if exists and lib is not sndfile itself
	if [ "$i" != "libsndfile.1.dylib" ]; then
		LIBSNDFILE_PATH=$(otool -L $i | grep -o $SNDFILE_REGEX)

		if [ "$LIBSNDFILE_PATH" != "" ]; then
			echo sndfile path : $LIBSNDFILE_PATH
			install_name_tool -change $LIBSNDFILE_PATH @executable_path/$LIB_PATH/libsndfile.1.dylib $i
		fi
	fi
	
	otool -L $i
done
