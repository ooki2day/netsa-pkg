#! /usr/bin/perl -w
# MD5: bfd774bfc2b633e67e81bad5eec6cf01
# TEST: ./rwsettool --mask=61 ../../tests/set1-v6.set | ./rwsetcat

use strict;
use SiLKTests;

my $rwsettool = check_silk_app('rwsettool');
my $rwsetcat = check_silk_app('rwsetcat');
my %file;
$file{v6set1} = get_data_or_exit77('v6set1');
my $cmd = "$rwsettool --mask=61 $file{v6set1} | $rwsetcat";
my $md5 = "bfd774bfc2b633e67e81bad5eec6cf01";

check_md5_output($md5, $cmd);
