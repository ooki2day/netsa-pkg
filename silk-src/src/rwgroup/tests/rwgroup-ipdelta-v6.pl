#! /usr/bin/perl -w
# MD5: 310353bb4b471356c86517154f713025
# TEST: ../rwsort/rwsort --fields=1 ../../tests/data-v6.rwf | ./rwgroup --delta-field=1 --delta-value=64 | ../rwcat/rwcat --compression-method=none --byte-order=little

use strict;
use SiLKTests;

my $rwgroup = check_silk_app('rwgroup');
my $rwcat = check_silk_app('rwcat');
my $rwsort = check_silk_app('rwsort');
my %file;
$file{v6data} = get_data_or_exit77('v6data');
check_features(qw(ipv6));
my $cmd = "$rwsort --fields=1 $file{v6data} | $rwgroup --delta-field=1 --delta-value=64 | $rwcat --compression-method=none --byte-order=little";
my $md5 = "310353bb4b471356c86517154f713025";

check_md5_output($md5, $cmd);
