#! /usr/bin/perl -w
# MD5: c9c383aae32e46323b8df833950937a1
# TEST: ./rwfilter --not-any-cidr=2001:db8:c0:a8::c0:0/106 --pass=stdout ../../tests/data-v6.rwf | ../rwcat/rwcat --compression-method=none --byte-order=little

use strict;
use SiLKTests;

my $rwfilter = check_silk_app('rwfilter');
my $rwcat = check_silk_app('rwcat');
my %file;
$file{v6data} = get_data_or_exit77('v6data');
check_features(qw(ipv6));
my $cmd = "$rwfilter --not-any-cidr=2001:db8:c0:a8::c0:0/106 --pass=stdout $file{v6data} | $rwcat --compression-method=none --byte-order=little";
my $md5 = "c9c383aae32e46323b8df833950937a1";

check_md5_output($md5, $cmd);
