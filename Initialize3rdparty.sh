
PATH_TO_LFRAYTRACERPBRT="../LFRayTracerPBRT"

mkdir 3rdparty

mkdir 3rdparty/lfraytracer
mkdir 3rdparty/lfraytracer/include
mkdir 3rdparty/lfraytracer/include/pbrt
mkdir 3rdparty/lfraytracer/lib
mkdir 3rdparty/lfraytracer/lib/Debug
mkdir 3rdparty/lfraytracer/lib/Release

cp $PATH_TO_LFRAYTRACERPBRT/src/LFRayTracerPBRT.h       3rdparty/lfraytracer/include/LFRayTracerPBRT.h
cp $PATH_TO_LFRAYTRACERPBRT/src/core/error.h            3rdparty/lfraytracer/include/pbrt/error.h
cp $PATH_TO_LFRAYTRACERPBRT/src/core/imageio.h          3rdparty/lfraytracer/include/pbrt/imageio.h
cp $PATH_TO_LFRAYTRACERPBRT/src/core/pbrt.h             3rdparty/lfraytracer/include/pbrt/pbrt.h

cp $PATH_TO_LFRAYTRACERPBRT/build/Debug/libpbrt.lib     3rdparty/lfraytracer/lib/Debug/libpbrt.lib
cp $PATH_TO_LFRAYTRACERPBRT/build/Release/libpbrt.lib   3rdparty/lfraytracer/lib/Release/libpbrt.lib




mkdir 3rdparty/glog
mkdir 3rdparty/glog/include
mkdir 3rdparty/glog/include/glog
cp -R $PATH_TO_LFRAYTRACERPBRT/src/ext/glog/src/windows/.  3rdparty/glog/include
mkdir 3rdparty/glog/lib
mkdir 3rdparty/glog/lib/Debug
mkdir 3rdparty/glog/lib/Release
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/glog/Debug/*.lib   3rdparty/glog/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/glog/Debug/*.pdb   3rdparty/glog/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/glog/Release/*.lib 3rdparty/glog/lib/Release/



mkdir 3rdparty/ptex
mkdir 3rdparty/ptex/lib
mkdir 3rdparty/ptex/lib/Debug
mkdir 3rdparty/ptex/lib/Release
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/ptex/src/ptex/Debug/*.lib   3rdparty/ptex/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/ptex/src/ptex/Debug/*.pdb   3rdparty/ptex/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/ptex/src/ptex/Release/*.lib 3rdparty/ptex/lib/Release/



mkdir 3rdparty/zlib
mkdir 3rdparty/zlib/lib
mkdir 3rdparty/zlib/lib/Debug
mkdir 3rdparty/zlib/lib/Release
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/zlib/Debug/*.lib   3rdparty/zlib/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/zlib/Debug/*.pdb   3rdparty/zlib/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/zlib/Release/*.lib 3rdparty/zlib/lib/Release/



mkdir 3rdparty/openexr
mkdir 3rdparty/openexr/lib
mkdir 3rdparty/openexr/lib/Debug
mkdir 3rdparty/openexr/lib/Release
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/openexr/OpenEXR/IlmImf/Debug/*.lib   3rdparty/openexr/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/openexr/OpenEXR/IlmImf/Debug/*.pdb   3rdparty/openexr/lib/Debug/
cp -R $PATH_TO_LFRAYTRACERPBRT/build/src/ext/openexr/OpenEXR/IlmImf/Release/*.lib 3rdparty/openexr/lib/Release/