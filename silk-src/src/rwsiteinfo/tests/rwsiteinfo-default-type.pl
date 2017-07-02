#! /usr/bin/perl -w
# MD5: 266c6d33666af3ecd03ac3bb03a55dcc
# TEST: ./rwsiteinfo --fields=default-type --site-config-file ../../tests/test-site.conf

use strict;
use SiLKTests;

my $rwsiteinfo = check_silk_app('rwsiteinfo');
my $cmd = "$rwsiteinfo --fields=default-type --site-config-file $SiLKTests::top_srcdir/tests/test-site.conf";
my $md5 = "266c6d33666af3ecd03ac3bb03a55dcc";

check_md5_output($md5, $cmd);
