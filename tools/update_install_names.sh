cd ../build/modiqus/Build/Products/Debug
# install_name_tool -change /Users/dejean/Developer/libraries/csound/build/Debug/CsoundLib.framework/Versions/6.0/CsoundLib @executable_path/lib/CsoundLibd.framework/Versions/6.0/CsoundLib libmodiqd.dylib
cd ../Release
# install_name_tool -change /Users/dejean/Developer/libraries/csound/build/Debug/CsoundLib.framework/Versions/6.0/CsoundLib @executable_path/lib/CsoundLib.framework/Versions/6.0/CsoundLib libmodiq.dylib
cd ../../../../../ext/csound/debug/CsoundLib.framework
install_name_tool -id @executable_path/lib/CsoundLib.framework/Versions/6.0/CsoundLib CsoundLib
install_name_tool -change /usr/local/lib/libsndfile.1.dylib @executable_path/lib/libsndfile.1.dylib CsoundLib
cd ../../release/CsoundLib.framework
install_name_tool -id @executable_path/lib/CsoundLib.framework/Versions/6.0/CsoundLib CsoundLib
install_name_tool -change /usr/local/lib/libsndfile.1.dylib @executable_path/lib/libsndfile.1.dylib CsoundLib
	