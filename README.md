POSIX::strftime() behavior change in Perl 5.39.8.
=================================================

I have observed a behavior change in `POSIX::strftime()` in Perl 5.39.8. This is most succinctly demonstrated by this code:

    say strftime '%Y-%m-%d %H:%M:%S', gmtime $epoch;

**If** the `$epoch` represents a time when the local zone is shifted to Summer time, the output will be an hour later than the actual time. That is, at 2024-05-01 23:30:00 the output will be `2024-05-02 00:30:00`.

Perls before 5.39.8 do **not** exhibit this behavior.

The behavior seems to be related to the value of element `[8]` of the returned array, the `isdst` element. If this is `0` (as it is by definition in the case of `gmtime()`), the behavior under Perl 5.39.8 is as described in the previous paragraph. If this element is absent or `-1`, the output is an hour later only if the results of `gmtime()` specify a time which is during the jump to Summer time.

These tests were done under Olson zone `America/New_York`, and with the `TZ` environment variable undefined. I note that the core Perl test `ext/POSIX/t/time.t` jumps through all sorts of hoops to try to be portable among time zones. My read of this says that the steps relevant to formatting `gmtime` output are

    $ENV{TZ} = 'UTC0UTC';
    tzset(); # From the POSIX module

Doing these does not affect the behavior of the code under any combination of OS and Perl I have tried. But setting `$ENV{TZ}` to any one of `GMT`, `UTC`, or `UTC0` **does** cause Perl 5.39.8 to behave like previous Perls, even in the absence of the `tzset()`.

I have read through the `perldelta.pod` for Perl 5.39.8 and not found this called out as a deliberate change. A `diff -u` between the 5.39.7 and 5.39.8 versions of `POSIX.xs` finds two changes, but it is not obvious to me that either is relevant.

In addition to this document, this Git repository contains two Perl scripts:

- strftime-year

  This script takes a year on the command line (defaulting to 2024), and formats the date and time for every 30 minutes throughout the year using both `strftime()` and `sprintf()` on the output of `gmtime()`. If it detects intervals when the output of the two are not the same, it reports the first time where the difference occurs, and the first time where there ceases to be a difference. You can get more information using `strftime-year --help`.

- strftime-try

  This script formats its command-line arguments using `strftime()` and prints the results. The arguments are used verbatim. You can get more information using `strftime-try --help`.

Relevant data on the macOS Perl 5.39.8 build:

    $ perl-5.39.8/perl -V
    Summary of my perl5 (revision 5 version 39 subversion 8) configuration:
       
      Platform:
        osname=darwin
        osvers=22.6.0
        archname=darwin-2level
        uname='darwin samwise.local 22.6.0 darwin kernel version 22.6.0: tue nov 7 21:48:06 pst 2023; root:xnu-8796.141.3.702.9~2release_x86_64 x86_64 '
        config_args='-Dprefix=/trw/local/perl/5.39.8 -Dcf_email=wyant@cpan.org -Uinstallusrbinperl -e -d -Dusedevel -Uversiononly'
        hint=recommended
        useposix=true
        d_sigaction=define
        useithreads=undef
        usemultiplicity=undef
        use64bitint=define
        use64bitall=define
        uselongdouble=undef
        usemymalloc=n
        default_inc_excludes_dot=define
      Compiler:
        cc='cc'
        ccflags ='-fno-common -DPERL_DARWIN -mmacosx-version-min=13.6 -DNO_THREAD_SAFE_QUERYLOCALE -DNO_POSIX_2008_LOCALE -fno-strict-aliasing -pipe -fstack-protector-strong -I/usr/local/include -I/opt/local/include'
        optimize='-O3'
        cppflags='-fno-common -DPERL_DARWIN -mmacosx-version-min=13.6 -DNO_THREAD_SAFE_QUERYLOCALE -DNO_POSIX_2008_LOCALE -fno-strict-aliasing -pipe -fstack-protector-strong -I/usr/local/include -I/opt/local/include'
        ccversion=''
        gccversion='Apple LLVM 15.0.0 (clang-1500.1.0.2.5)'
        gccosandvers=''
        intsize=4
        longsize=8
        ptrsize=8
        doublesize=8
        byteorder=12345678
        doublekind=3
        d_longlong=define
        longlongsize=8
        d_longdbl=define
        longdblsize=16
        longdblkind=3
        ivtype='long'
        ivsize=8
        nvtype='double'
        nvsize=8
        Off_t='off_t'
        lseeksize=8
        alignbytes=8
        prototype=define
      Linker and Libraries:
        ld='cc'
        ldflags =' -mmacosx-version-min=13.6 -fstack-protector-strong -L/usr/local/lib -L/opt/local/lib'
        libpth=/usr/local/lib /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/15.0.0/lib /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib /opt/local/lib /usr/lib
        libs=-lgdbm
        perllibs=
        libc=
        so=dylib
        useshrplib=false
        libperl=libperl.a
        gnulibc_version=''
      Dynamic Linking:
        dlsrc=dl_dlopen.xs
        dlext=bundle
        d_dlsymun=undef
        ccdlflags=' '
        cccdlflags=' '
        lddlflags=' -mmacosx-version-min=13.6 -bundle -undefined dynamic_lookup -L/usr/local/lib -L/opt/local/lib -fstack-protector-strong'


    Characteristics of this binary (from libperl): 
      Compile-time options:
        HAS_LONG_DOUBLE
        HAS_STRTOLD
        HAS_TIMES
        PERLIO_LAYERS
        PERL_COPY_ON_WRITE
        PERL_DONT_CREATE_GVSV
        PERL_HASH_FUNC_SIPHASH13
        PERL_HASH_USE_SBOX32
        PERL_MALLOC_WRAP
        PERL_OP_PARENT
        PERL_PRESERVE_IVUV
        PERL_USE_DEVEL
        PERL_USE_SAFE_PUTENV
        USE_64_BIT_ALL
        USE_64_BIT_INT
        USE_LARGE_FILES
        USE_LOCALE
        USE_LOCALE_COLLATE
        USE_LOCALE_CTYPE
        USE_LOCALE_NUMERIC
        USE_LOCALE_TIME
        USE_PERLIO
        USE_PERL_ATOF
      Built under darwin
      Compiled at Feb 24 2024 08:32:39
      @INC:
        /trw/local/perl/5.39.8/lib/site_perl/5.39.8/darwin-2level
        /trw/local/perl/5.39.8/lib/site_perl/5.39.8
        /trw/local/perl/5.39.8/lib/5.39.8/darwin-2level
        /trw/local/perl/5.39.8/lib/5.39.8


