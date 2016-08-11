

open FT, "<outTime" or die $!;
open FE, "<outExtrap" or die $!;
open FO, "<outOrig" or die $!;

my $cnt=0;
while(<FT>)
{
	$cnt++;
}

warn $cnt;