#! /usr/bin/perl -w
# MD5: 5efeade8745514b5f0a78dc01d350e59
# TEST: ./rwfilter --not-saddr=x:x:a:fc-ff::0-ffff:0,1-fab,fad-ffff,fac --pass=stdout ../../tests/data-v6.rwf | ../rwcat/rwcat --compression-method=none --byte-order=little

use strict;
use SiLKTests;

my $rwfilter = check_silk_app('rwfilter');
my $rwcat = check_silk_app('rwcat');
my %file;
$file{v6data} = get_data_or_exit77('v6data');
check_features(qw(ipv6));
my $cmd = "$rwfilter --not-saddr=x:x:a:fc-ff::0-ffff:0,1-fab,fad-ffff,fac --pass=stdout $file{v6data} | $rwcat --compression-method=none --byte-order=little";
my $md5 = "5efeade8745514b5f0a78dc01d350e59";

check_md5_output($md5, $cmd);
