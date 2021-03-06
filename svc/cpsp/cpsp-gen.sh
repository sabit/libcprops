#!/bin/sh

MAKE=make
if [ "x$MAKE" = "x" ] ; then 
  MAKE=make
fi

cwd=$PWD
logfile="$cwd/cpsp-gen.log"

makefile=Makefile.cpsp
if test "$2" ; then
  makefile="$2/$makefile"
fi

echo  >> $logfile
echo `date` $0 starting >> $logfile

filename=`echo $1 | sed 's|\.\/||'`
echo compiling $filename >> $logfile

if [ ! -r $filename ] ;
then
	echo $filename not found >> $logfile
	exit -1
fi

CPSP_SRC_lit=$filename

x=`echo $1 | sed 's|\(.*\)\/.*|\1|'`
y=`echo $1 | sed 's|.*\/||'`

if [ "$1" = "$x" ] ; then
  prefix=""
  fname=$1
  cpsp_target=_cpsp_$fname
  gen_target=`echo gen_cpsp_$fname | sed 's|\.cpsp$|\.so|'`
else
  prefix=$x
  fname=$y
  cpsp_target=$prefix/_cpsp_$fname
  gen_target=`echo $prefix/gen_cpsp_$fname | sed 's|\.cpsp$|\.so|'`
fi

CPSP_TARGET_lit=`echo $cpsp_target | sed 's|psp$||'`

# if the c file exists but is 0 bytes long, delete it
if [ -r $CPSP_TARGET_lit -a ! -s $CPSP_TARGET_lit ] ; then
  rm -f $CPSP_TARGET_lit
fi

CPSP_TARGET=$CPSP_TARGET_lit CPSP_SRC=$CPSP_SRC_lit \
    $MAKE -f $makefile cpsp-page >> $logfile 2>&1

if [ $? -ne 0 ] ; then exit $? ; fi

target=`echo $cpsp_target | sed 's|\.cpsp$|\.so|'`
echo target: $target >> $logfile
if [ -r $target ] ;
then
	cp -f $target $target~ >& /dev/null
fi

mv -f $gen_target $target >> $logfile 2>&1

