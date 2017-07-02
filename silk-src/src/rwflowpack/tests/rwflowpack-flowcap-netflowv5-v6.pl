#! /usr/bin/perl -w
#
#
# RCSIDENT("$SiLK: rwflowpack-flowcap-netflowv5-v6.pl 23e62811e29c 2016-11-16 15:30:29Z mthomas $")

use strict;
use SiLKTests;

my $NAME = $0;
$NAME =~ s,.*/,,;

# find the apps we need.  this will exit 77 if they're not available
my $rwflowpack = check_silk_app('rwflowpack');
my $rwcut = check_silk_app('rwcut');
my $rwsort = check_silk_app('rwsort');

# verify that required features are available
check_features(qw(inet6));

# prefix any existing PYTHONPATH with the proper directories
check_python_bin();

# the script that sends the PDUs uses the optional Socket6.pm Perl
# module to send to an IPv6 socket; ensure the module is available
eval "require Socket6";
if ($@) {
    # Socket6 is not available
    skip_test('The Socket6 Perl module is not available');
}

# create our tempdir
my $tmpdir = make_tempdir();

# send data to this port and host
my $host = '::1';
my $port = get_ephemeral_port($host, 'udp');

unless (defined $ENV{FLOWCAP}) {
    $ENV{FLOWCAP} = check_silk_app('rwflowpack');
}

# Generate the config.lua file, and name it based on the test's name
my $config_lua = $0;
$config_lua =~ s,^.*\b(rwflowpack-.+)\.pl,$tmpdir/$1.lua,;
make_config_file($config_lua, get_config_lua_body());

# the command that wraps flowcap
my $cmd = join " ", ("$SiLKTests::PYTHON $srcdir/tests/flowcap-daemon.py",
                     "--basedir=$tmpdir",
                     "--daemon-timeout=120",
                     "--pdu 40000,$host,$port",
                     "--limit=40000",
                     "--",
                     $config_lua,
    );

# run it and check the MD5 hash of its output
check_md5_output('f6e9b35dc226f9975c8c14d9ab332bd7', $cmd);

# path to the directory holding the output files
my $data_dir = "$tmpdir/destination";
die "$NAME: ERROR: Missing data directory '$data_dir'\n"
    unless -d $data_dir;

# check for zero length files in the directory
opendir D, "$data_dir"
    or die "$NAME: ERROR: Unable to open directory $data_dir: $!\n";
for my $f (readdir D) {
    next if (-d "$data_dir/$f") || (0 < -s _);
    warn "$NAME: WARNING: Zero length files in $data_dir\n";
    last;
}
closedir D;

# create a command to sort all files in the directory and output them
# in a standard form.
$cmd = join " ",("find $data_dir -type f -print",
                 "| $rwsort --xargs --fields=stime,sip",
                 "| $rwcut --delim=,",
                 "--ip-format=hexadecimal --timestamp-format=epoch",
                 "--fields=".join(",", qw(stime sip dip sport dport proto
                                          packets bytes flags etime sensor
                                          initialFlags sessionFlags attributes
                                          application type iType iCode)),
    );

exit check_md5_output('1f0b6d393aa2fdd418212e86977be4ee', $cmd);


sub get_config_lua_body
{
    my $debug = ($ENV{SK_TESTS_LOG_DEBUG} ? "\n    level = \"debug\"," : "");

    # The Lua configuration file is the text after __END__.  Read it,
    # perform variable substitution, and return a scalar reference.
    local $/ = undef;
    my $text = <DATA>;
    $text =~ s/\$\{host\}/$host/g;
    $text =~ s/\$\{port\}/$port/;
    $text =~ s/\$\{tmpdir\}/$tmpdir/;
    $text =~ s/\$\{debug\}/$debug/;

    # Check for unexpanded variable
    if ($text =~ /(\$\{?\w+\}?)/) {
        die "$NAME: Unknown variable '$1' in Lua configuration\n";
    }
    return \$text;
}

__END__
if not silk.site.have_site_config() then
  if not silk.site.init_site(nil, nil, true) then
    error("The silk.conf file was not found")
  end
end

local sc = silk.sidecar_create()
sc.tos = { type = "uint8", element_id = 5 }
silk.sidecar_freeze(sc)

local tmpdir = "${tmpdir}"
input = {
    mode = "stream",
    probes = {
        P0 = {
            name = "P0",
            type = "netflow-v5",
            output_file_info = {
                record_format = silk.file_format_id("FT_FLOWCAP"),
                sidecar = sc,
            },
            source = {
                protocol = "udp",
                listen = "[${host}]:${port}",
                accept = { "${host}" },
            },
            packing_function = function (probe, rec, pdu)
              rec.sidecar = { tos = pdu.tos }
              write_rwrec(rec)
            end
        },
    },
}
output = {
    mode = "flowcap",
    flush_interval = 10,
    synchronize_flush = 2,  -- This is equivalent to --clock-time=2
    output_directory = tmpdir .. "/destination",
    max_file_size = "100k",
}
log = {
    destination = "stderr",${debug}
}
daemon = {
    fork = false,
}