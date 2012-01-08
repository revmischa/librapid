## To build the git checkout:
```
git submodule init
git submodule update
autoreconf -i
automake -a -c
./configure
make
```

## Cross-compiling:
This library is designed for use in embedded systems. If you want to cross-compile it, try something along the lines of:
`./configure --build=x86_64-linux-gnu --host=mips-openwrt-linux`

You will need to have the appropriate library and executable search paths in $PATH and $LDFLAGS.

If you are using the OpenWRT SDK, you'll need to set up your environment like so:
```
OPENWRT_SDK="$HOME/dev/toolchains/openwrt"
OPENWRT_TOOLCHAIN="$OPENWRT_SDK/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1"
export PATH="$OPENWRT_TOOLCHAIN/usr/bin/:$PATH"
export LDFLAGS="-L$OPENWRT_TOOLCHAIN/usr/lib -L$OPENWRT_TOOLCHAIN/lib -Wl,-rpath-link=$OPENWRT_TOOLCHAIN/lib/"
export ac_cv_func_malloc_0_nonnull=yes
```

