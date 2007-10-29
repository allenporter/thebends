#!/usr/bin/perl

my $bin = shift @ARGV;
die "$!: $bin" if (!-f $bin);

my $tool = "/usr/bin/otool";
#fahlLtdoOrTMRIHScis
my @flags = ( '-f', '-a', '-h', '-l', '-L', '-d', '-r', '-S', '-T', '-M', '-R', '-I', '-H', '-tv', '-c');
my $out;
foreach my $flag (@flags) {
  $out = `$tool $flag $bin`;
  $out =~ s/^$bin:\n//;
  print "FLAGS: $flag\n";
  print "$out";
  if ($flag eq '-l') {
    @lines = split /\n/, $out;
    my $secname = '';
    my $segname = '';
    while ($#lines > 0) {
      $_ = shift @lines;
      if (/Section/) {
        $_ = shift @lines;
        if (/sectname (.*)/) {
          my $sectname = $1;
          $_ = shift @lines;
          if (/segname (.*)/) {
            my $segname = $1;
            $out = `$tool -s $segname $sectname $bin`;
            $out =~ s/^$bin:\n//;
            print "FLAGS: -s $segname $sectname\n";
            print "$out";
          }
        }
      }
    }
  }
}
