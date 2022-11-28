#!/usr/bin/perl
#
# This script is for checking who's logged in without connecting to the
# GB server. It uses the user.log file to determine who's logged in.
# -Michael Wilkinson (04/26/05)

# Set the location of your user.log file
$LOGFILE = "/home/gbp/GB+/logs/user.log";

# Accept optional debug argument
if ($ARGV[0] eq '-d') {
    $debug = 1;
}

# Calculate the starting position in the log file. We do this because the
# log files can get quite large. Doing this calculation first then starting
# from the @startfrom position in the file speeds up this program ~10x.
$lines = `wc -l $LOGFILE`;
$position = `grep -m \"GB server started\" $LOGFILE | tail -1 | cut -f1 -d:`;
$startfrom = ($lines - $position + 1);

# Open the log file from our $startfrom position
open(LOG, "tail -$startfrom $LOGFILE |") || die("Can't open logfile: $!");

# Number of users connected
$concnt = 0;

# Loop on the output of the log file
while (<LOG>) {
    # A start message, mark everyone as disconnected so far.
    if ($_ =~ /GB server started/) {
        foreach $key (keys %con) {
            $con{$key} = 0;
        }
    }

    # A bit of cleanup prior to parsing
    s/^ +//g;
    s/ - / /g;
    s/: / /g;
    s/"/ /g;

    # Parse components from record
    ($date, $time, $status, $desc, $id, $race, $gov, $ip) = split(/\s+/);

    # Clean up IP address
    $ip =~ s/0+/0/g;
    $ip =~ s/^0([1-9])/\1/g;
    $ip =~ s/\.0([1-9])/\.\1/g;

    # Debug output
    if ($debug) {
        print "$date\n";
        print "$time\n";
        print "$status\n";
        print "$desc\n";
        print "$id\n";
        print "$race\n";
        print "$gov\n";
        print "$ip\n\n";
    }

    # Look for connect or disconnect record
    if ($status =~ /CONNECTED/) {
        # Save parsed information to hash tables
        $con{$desc} = 1;
        $race{$desc} = $race;
        $gov{$desc} = $gov;
        $ip{$desc} = $ip;
        $date{$desc} = $date;
        $time{$desc} = $time;
        $id{$desc} = $id;
    }
    elsif ($status =~ /DISCONNECT/) {
        # If there's a disconnect record for this descriptor mark it as offline
        $con{$desc} = 0;
    }
}

# Find the sum of active connections
foreach $key (keys %con) {
    if ($con{$key} == 1) {
        $concnt++;
    }
}

# Are there connections?
if ($concnt) {
    # If so, print a friendly header
    printf("%s User%s Connected:\n", $concnt, ($concnt > 1 ? "s" : ""));
    print("Con  User    Race Name        Gov Name          On Since        IP Addr\n");
    print("---  ------  ---------------  ----------------  -------------   --------------\n");
}
else {
    # If not, say so and exit
    print("No users connected.\n");
    exit 0;
}

# Loop through our connections hash table
foreach $key (keys %con) {
    # Is it marked as connected?
    if ($con{$key} == 1) {
        # If so, print out the entry
        printf("%3s  %6s  %-16s %-16s %5s %8s   %-15s\n", $key, $id{$key}, $race{$key}, $gov{$key}, $date{$key}, $time{$key}, $ip{$key});
    }
}

# Exit nicely
exit 0;

# The format of a login/logout in users.log (for reference)
#
# 4/04 18:49:54 - CONNECTED : D12 [02,1] Guestians  "Guest1"    203.118.131.065
# 4/04 18:49:54 - SETTING bit 12 on D1
# 4/04 18:50:13 - DISCONNECT: D12 [02,1] Guestians  "Guest1"    203.118.131.065
