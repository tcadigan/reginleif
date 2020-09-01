#!/usr/bin/perl
# Outputs a uniform length list of words

$FILLER = '%';
$MAXLENGTH = 20;
$LENGTH = $MAXLENGTH - 1; # This accounts for the newline

if (!$ARGV[0]) {
    print "You must specify a file name.\n";
    exit 1;
}

$file = $ARGV[0];

open(F, "<$file") || die "Cannot open $file: $!\n";

while (<F>) {
    chop;
    $l = length();

    if ($l > $LENGTH) {
        print STDERR "long word\n";
        $s = substr($_, 0, $LENGTH)
    }
    elsif ($l < $LENGTH) {
        $s = $_;
        $s = $s . $FILLER x ($LENGTH - $l);
    }
    else {
        $s = $_;
    }

    print $s . "\n";
}
