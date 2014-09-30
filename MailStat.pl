#!/usr/bin/perl

use strict;

sub trim($)
{
   my $string = shift;
   $string =~ s/^\s+//;
   $string =~ s/\s+$//;
   return $string;
}

my $HOME="$ENV{HOME}";

print "Please enter the name of the Mbox you want to process (default: spam):\n";
my $mbox;
chomp($mbox = <STDIN>);

if($mbox){
  print " opening $HOME/Mail/ . $mbox\n";
  open (FILE, $HOME . '/Mail/' . $mbox);
} else {
  print " opening $HOME/Mail/spam\n";
  open (FILE, $HOME . '/Mail/spam');
}

my %HashFrom;
my %HashTo;
my %HashEnvTo;

sub AddTO($){
  my $cur = shift;
  #printf "Adding $cur to TO\n"; tests
  ++$HashTo{$cur};
}
sub AddFROM{
   ++$HashFrom{$_};
}

sub AddMailToHash{
 my ($mailstring, %Hash) = @_ ;
 if(/</)
 {
   my @words=split(/</,$mailstring);
   my @mail=split(/>/,@words[1]);
   #print "current: @mail[0]\n";
   AddTO(trim(@mail[0]));
 }
 else
 {
   #print "New Mail : " . $mailstring . "\n";
   my $s = substr($mailstring,4);
   my @words=split(/,/,$s);
   #print "** Adding: ";
   foreach my $mailadd (@words)
   {
     #print " $mailadd ";
     AddTO(trim($mailadd));
   }
   #print "\n";
 }
}

while (<FILE>) {
  chomp;
  if(/^To:/ || /^Cc:/ || /^Bcc:/)
  {
    AddMailToHash($_,%HashTo);
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
