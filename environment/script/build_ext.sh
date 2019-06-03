set -e

# Build external packages



### function install_scorep()
### {
###     download_prefix=$1
###     build_prefix=$2
###     install_prefix=$3
### 
###     version=5.0
###     basename=scorep-$version
### 
###     # Download
###     wget --directory-prefix=$download_prefix -nc \
###         https://www.vi-hps.org/cms/upload/packages/scorep/$basename.tar.gz
### 
###     # Unpack, build and install
###     cd $build_prefix
###     rm -fr $basename
###     tar zxf $download_prefix/$basename.tar.gz
###     cd $basename
### 
###     mkdir _build
###     cd _build
###     ../configure --prefix=$install_prefix
### 
###     make -j6
###     make install
### }


function install_papi()
{
    download_prefix=$1
    build_prefix=$2
    install_prefix=$3

    version=5.7.0
    basename=papi-$version

    # Download
    wget --directory-prefix=$download_prefix -nc \
        http://icl.utk.edu/projects/papi/downloads/$basename.tar.gz

    # Unpack, build and install
    cd $build_prefix
    rm -fr $basename
    tar zxf $download_prefix/$basename.tar.gz
    cd $basename/src

    ./configure --prefix=$install_prefix

    make -j6
    make install
}


### function install_cubegui()
### {
###     download_prefix=$1
###     build_prefix=$2
###     install_prefix=$3
### 
###     main_version=4.4
###     version=$main_version.3
###     basename=cubegui-$version
### 
###     # Download
###     wget --directory-prefix=$download_prefix -nc \
###         http://apps.fz-juelich.de/scalasca/releases/cube/$main_version/dist/$basename.tar.gz
### 
###     # Unpack, build and install
###     cd $build_prefix
###     rm -fr $basename
###     tar zxf $download_prefix/$basename.tar.gz
###     cd $basename
### 
###     mkdir _build
###     cd _build
###     # configure: Try to test Qt with g++-7 and Qt specification "default".
###     # configure: If it fails, please specify proper frontend compiler using option --with-frontend-compiler-suite or different Qt specification using option --with-qt-specs.
###     # --with-qt-specs=linux-g++
###     # --with-frontend-compiler-suite=gcc
### 
###     PATH=$install_prefix/bin:$PATH ../configure --prefix=$install_prefix
### 
###     # --with-cubelib-config=$install_prefix/bin
###     #   --with-scorep=$install_prefix/bin
### 
###     make -j6
###     make install
### }


### function install_scalasca()
### {
###     download_prefix=$1
###     build_prefix=$2
###     install_prefix=$3
### 
###     version=2.5
###     basename=scalasca-$version
### 
###     # Download
###     wget --directory-prefix=$download_prefix -nc \
###         http://apps.fz-juelich.de/scalasca/releases/scalasca/$version/dist/$basename.tar.gz
### 
###     # Unpack, build and install
###     cd $build_prefix
###     rm -fr $basename
###     tar zxf $download_prefix/$basename.tar.gz
###     cd $basename
### 
###     mkdir _build
###     cd _build
###     PATH=$install_prefix/bin:$PATH \
###         ../configure --prefix=$install_prefix
### 
###     #   --with-otf2=$install_prefix/bin \
###     #   --with-cubew=$install_prefix/bin
### 
###     make -j6
###     make install
### }


function install_pdt()
{
    download_prefix=$1
    build_prefix=$2
    install_prefix=$3

    version=3.25.1
    basename=pdt-$version
    basename2=pdtoolkit-$version

    # Download
    wget --directory-prefix=$download_prefix -nc \
        https://www.cs.uoregon.edu/research/tau/pdt_releases/$basename.tar.gz

    # Unpack, build and install
    cd $build_prefix
    rm -fr $basename2
    tar zxf $download_prefix/$basename.tar.gz
    cd $basename2

    ./configure -prefix=$install_prefix -GNU

    make -j6
    make install
}


function install_tau()
{
    download_prefix=$1
    build_prefix=$2
    install_prefix=$3

    wget --directory-prefix=$download_prefix -nc \
        http://tau.uoregon.edu/ext.tgz

    # -ompt=download
    # -c++=<compiler>
    # -cc=<compiler>
    # -fortran=<compiler>
    # -pdt=<directory>
    # -otf=<directory>
    # -scalasca=<directory>
    # -papithreads(?)
    # -apex

    version=2.28.1
    basename=tau-$version

    # Download
    wget --directory-prefix=$download_prefix -nc \
        https://www.cs.uoregon.edu/research/tau/tau_releases/$basename.tar.gz

    # Unpack, build and install
    cd $build_prefix
    rm -fr $basename
    tar zxf $download_prefix/$basename.tar.gz
    cd $basename
    tar zxf $download_prefix/ext.tgz

    #   -scorep=$install_prefix
    ./configure -prefix=$install_prefix \
        -bfd=download \
        -dwarf=download \
        -papi=$install_prefix \
        -pthread \
        -scorep=download \
        -unwind=download \
        -iowrapper \
        -pdt=$install_prefix

    make -j6
    make install
}


download_prefix=$HOME/development/download
# build_prefix=~/tmp/build
build_prefix=$(mktemp -d)
install_prefix=$HOME/development/ext

echo "prefixes:"
echo "- download: $download_prefix"
echo "- install : $install_prefix"
echo "- build   : $build_prefix"

mkdir -p $download_prefix $install_prefix

unset CC CXX

install_papi $download_prefix $build_prefix $install_prefix
### install_cubegui $download_prefix $build_prefix $install_prefix
### install_scalasca $download_prefix $build_prefix $install_prefix
install_pdt $download_prefix $build_prefix $install_prefix
install_tau $download_prefix $build_prefix $install_prefix
