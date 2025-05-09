pkgconfigdir    = $(libdir)/pkgconfig
pkgconfig_DATA  = libeditline.pc
doc_DATA        = README.md LICENSE
EXTRA_DIST      = README.md LICENSE ChangeLog.md INSTALL.md
SUBDIRS         = src include man

if ENABLE_EXAMPLES
SUBDIRS            += examples
endif

## Check if tagged in git
release-hook:
	if [ ! `git tag | grep $(PACKAGE_VERSION) | grep $(PACKAGE_VERSION)` ]; then	\
		echo;									\
		printf "\e[1m\e[41mCannot find release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[5mDo release anyway?\e[0m "; read yorn;			\
		if [ "$$yorn" != "y" -a "$$yorn" != "Y" ]; then				\
			printf "OK, aborting release.\n";				\
			exit 1;								\
		fi;									\
		echo;									\
	else										\
		echo;									\
		printf "\e[1m\e[42mFound GIT release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[44m>>Remember to push tags!\e[0m\n";			\
		echo;									\
	fi

# lintian --profile debian -i -I --show-overrides ../$PKG.changes
package:
	dpkg-buildpackage -uc -us -B

## Target to run when building a release
release: release-hook distcheck
	@for file in $(DIST_ARCHIVES); do						\
		md5sum    $$file > ../$$file.md5;					\
		sha256sum $$file > ../$$file.sha256;					\
	done
	@mv $(DIST_ARCHIVES) ../
	@echo
	@echo "Resulting release files:"
	@echo "========================================================================="
	@for file in $(DIST_ARCHIVES); do						\
		printf "%-40s Distribution tarball\n" $$file;				\
		printf "%-40s " $$file.md5;    cat ../$$file.md5    | cut -f1 -d' ';	\
		printf "%-40s " $$file.sha256; cat ../$$file.sha256 | cut -f1 -d' ';	\
	done
	@for file in `cd ..; ls *$(PACKAGE)*_$(subst _,.,$(VERSION))*`; do		\
		printf "%-40s Debian/Ubuntu package file\n" $$file;			\
	done
