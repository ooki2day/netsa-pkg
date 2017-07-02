#! /usr/bin/perl -w
# MD5: 471cbea9ae88f807c461983506a8ddc2
# TEST: ./rwsiteinfo --fields=default-class --site-config-file ../../tests/test-site.conf

use strict;
use SiLKTests;

my $rwsiteinfo = check_silk_app('rwsiteinfo');
my $cmd = "$rwsiteinfo --fields=default-class --site-config-file $SiLKTests::top_srcdir/tests/test-site.conf";
my $md5 = "471cbea9ae88f807c461983506a8ddc2";

check_md5_output($md5, $cmd);
