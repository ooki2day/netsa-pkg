#! /usr/bin/perl -w
# MD5: 1659ee38467a3e76d0e71b6df397baeb
# TEST: ./rwsiteinfo --fields=sensor --site-config-file ../../tests/test-site.conf

use strict;
use SiLKTests;

my $rwsiteinfo = check_silk_app('rwsiteinfo');
my $cmd = "$rwsiteinfo --fields=sensor --site-config-file $SiLKTests::top_srcdir/tests/test-site.conf";
my $md5 = "1659ee38467a3e76d0e71b6df397baeb";

check_md5_output($md5, $cmd);
