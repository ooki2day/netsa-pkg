#! /usr/bin/perl -w
# MD5: 4c0f3cb20f3df4531b0b93e0536a75e0
# TEST: ./rwswapbytes --little-endian ../../tests/empty.rwf - | ./rwswapbytes --swap-endian - - | ../rwfileinfo/rwfileinfo --no-title --field=byte-order,count-records -

use strict;
use SiLKTests;

my $rwswapbytes = check_silk_app('rwswapbytes');
my $rwfileinfo = check_silk_app('rwfileinfo');
my %file;
$file{empty} = get_data_or_exit77('empty');
my $cmd = "$rwswapbytes --little-endian $file{empty} - | $rwswapbytes --swap-endian - - | $rwfileinfo --no-title --field=byte-order,count-records -";
my $md5 = "4c0f3cb20f3df4531b0b93e0536a75e0";

check_md5_output($md5, $cmd);
