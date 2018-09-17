CC          = clang
CCC         = clang++
EOF
		;;
	*) echo "Host compiler is default";;
	esac
	
	# requires wine and g++-mingw-w64-i686
	if [ "$WINE" = "32" ]
	then
		echo "Cross mingw32"
		sed -i -e '/CMPLR_PREFIX/d' $EPICS_BASE/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw
		cat << EOF >> "$EPICS_BASE/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw"
CMPLR_PREFIX=i686-w64-mingw32-
EOF
		cat << EOF >> "$EPICS_BASE/configure/CONFIG_SITE"
CROSS_COMPILER_TARGET_ARCHS+=win32-x86-mingw
EOF
	fi
	
	# set RTEMS to eg. "4.9" or "4.10"
	if [ -n "$RTEMS" ]
	then
		echo "Cross RTEMS${RTEMS} for pc386"
		install -d /home/travis/.cache
		curl -L "https://github.com/mdavidsaver/rsb/releases/download/travis-20160306-2/rtems${RTEMS}-i386-trusty-20190306-2.tar.gz" \
		| tar -C /home/travis/.cache -xj
	
		sed -i -e '/^RTEMS_VERSION/d' -e '/^RTEMS_BASE/d' $EPICS_BASE/configure/os/CONFIG_SITE.Common.RTEMS
		cat << EOF >> "$EPICS_BASE/configure/os/CONFIG_SITE.Common.RTEMS"
RTEMS_VERSION=$RTEMS
RTEMS_BASE=/home/travis/.cache/rtems${RTEMS}-i386
EOF
		cat << EOF >> $EPICS_BASE/configure/CONFIG_SITE
CROSS_COMPILER_TARGET_ARCHS+=RTEMS-pc386
EOF
	
	fi
	
	make -C "$EPICS_BASE" -j2

	
	# get MSI for 3.14
	case "$BASE" in
	3.14*)
		if [ ! -d "$HOME/msi/extensions/src" ]
		then
			echo "Build MSI"
			install -d "$HOME/msi/extensions/src"
			curl https://epics.anl.gov/download/extensions/extensionsTop_20120904.tar.gz | tar -C "$HOME/msi" -xvz
			curl https://epics.anl.gov/download/extensions/msi1-7.tar.gz | tar -C "$HOME/msi/extensions/src" -xvz
			mv "$HOME/msi/extensions/src/msi1-7" "$HOME/msi/extensions/src/msi"
		
			cat << EOF > "$HOME/msi/extensions/configure/RELEASE"
EPICS_BASE=$EPICS_BASE
EPICS_EXTENSIONS=\$(TOP)
EOF
	
		fi
		
		make -C "$HOME/msi/extensions"
		cp "$HOME/msi/extensions/bin/$EPICS_HOST_ARCH/msi" "$EPICS_BASE/bin/$EPICS_HOST_ARCH/"
		echo 'MSI:=$(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/msi' >> "$EPICS_BASE/configure/CONFIG_SITE"
	
		cat <<EOF >> configure/CONFIG_SITE
MSI = \$(EPICS_BASE)/bin/\$(EPICS_HOST_ARCH)/msi
EOF
	
		;;
	*) echo "Use MSI from Base"
		;;
	esac
fi

alias get_support='shallow_support'
alias get_repo='shallow_repo'

get_support support synApps_5_8
cd support

get_support configure        synApps_5_8

echo "SUPPORT=$HOME/.cache/support" > configure/RELEASE
echo "EPICS_BASE=$EPICS_BASE" >> configure/RELEASE

# modules ##################################################################

#get_repo   Git Project      Git Repo         RELEASE Name     Tag
if [[ $ASYN ]]; then   get_repo              epics-modules    asyn             ASYN             $ASYN          ; fi
if [[ $STD ]]; then   get_repo               epics-modules    std              STD              $STD           ; fi

if [[ $SNCSEQ ]]
then

# seq
wget http://www-csr.bessy.de/control/SoftDist/sequencer/releases/seq-$SNCSEQ.tar.gz
tar zxf seq-$SNCSEQ.tar.gz
# The synApps build can't handle '.'
mv seq-$SNCSEQ seq-${SNCSEQ//./-}
rm -f seq-$SNCSEQ.tar.gz
echo "SNCSEQ=\$(SUPPORT)/seq-${SNCSEQ//./-}" >> ./configure/RELEASE

fi


make release
make

cp -f configure/RELEASE $TRAVIS_BUILD_DIR/configure/RELEASE
