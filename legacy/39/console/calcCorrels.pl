use Corr;
use Data::Dumper;

$| = 1;
open ORIG, '<out_original' or die $!;
open EXTRAP, '<out_extrap' or die $!;

my $corrs = [];
my $div = 100;
while(my $origLine = <ORIG>)
{
	my $extrapLine = <EXTRAP>;
	
	chomp $origLine;
	chomp $extrapLine;
	
	my @orig = split /\s/, $origLine;
	my @extrap = split /\s/, $extrapLine;

	my $start = scalar(@orig)/2;
	
	while(@$corrs < scalar(@orig)/$div)
	{
		push @$corrs, Corr->new;
	}
	
	
	for(my $idx=0; $idx<@$corrs; $idx++)
	{
		#for(my $idx2=$idx*$div; $idx2<$idx*$div+$div; $idx2++)
		my $idx2=$idx*$div;
		{
			$corrs->[$idx]->push($orig[$idx2]-$orig[$start], $extrap[$idx2]-$extrap[$start]);
		}
	}
	
	warn $corrs->[50]->getAmount();
}

my $h = $corrs->[$idx]->eval();
foreach(keys %$h)
{
	print $_, "\t";
}
print $h->{$_}, "\n";

for(my $idx=0; $idx<@$corrs; $idx++)
{
	my $h = $corrs->[$idx]->eval();
	foreach(keys %$h)
	{
		print $h->{$_}, "\t";
	}
	print $h->{$_}, "\n";
}

exit;