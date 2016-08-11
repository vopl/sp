
doOne('xag');
doOne('cad');
doOne('nzd');
doOne('usd');
doOne('aud');
doOne('jpy');
doOne('gbp');
doOne('eur');
doOne('chf');
doOne('xau');


exit;
sub doOne($)
{
	my $dir = shift;
	
	print "$dir\t";
	open F, "<$dir/tans" or die $!;
	open F2, ">$dir/tans2" or die $!;
	
	
	my $sum=0;
	my $amount = 0;
	my $maxper=0;
	while(<F>)
	{
		chomp;
		
		my $mp=0;
		foreach(split /\t/, $_)
		{
			if($_)
			{
				$sum+=$_;
				$amount++;
				$mp++;
			}
		}
		
		$maxper = $mp if $maxper < $mp;
	}
	
	$sum /= $amount if $amount;
	print "$sum\t$maxper\t";

	my $stamp;
	while(<F2>)
	{
		$stamp = $_;
	}
	
	chomp $stamp;
	
	print "$stamp\n";
	
	close F;
	close F2;
}