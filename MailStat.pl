#!/usr/bin/perl

use strict;

sub trim($)
{
   my $string = shift;
   $string =~ s/^\s+//;
   $string =~ s/\s+$//;
   return $string;
}


open (FILE, '/home/leibi/Mail/spam');

my %HashFrom;
my %HashTo;
my %HashEnvTo;

while (<FILE>) {
chomp;

  if(/^To:/)
  {
    if(/</)
    {
     my @words=split(/</,$_);
     my @mail=split(/>/,@words[1]);
     ++$HashTo{trim(@mail[0])};
    }
    else
    {
     #print "New Mail : " . $_ . "\n";
     my $s = substr($_,4);
     my @words=split(/,/,$s);
     #print "** Adding: ";
     foreach my $mailadd (@words)
     {
        #print " $mailadd ";
        ++$HashTo{trim($mailadd)};
     }
     #print "\n";
    }
  }
}
close (FILE);

print "Processing Hash\n";
print "Size from : ". keys(%HashFrom) . "\n";
print "Size to : ". keys(%HashTo) . "\n";
print "Size env-to : ". keys(%HashEnvTo) . "\n";

for my $sender ( sort keys %HashTo ) {
  print "$sender: $HashTo{$sender}\n";
}

exit;
