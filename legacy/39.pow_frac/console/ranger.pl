use Data::Dumper;

open IN, '<'.$ARGV[0] || die $!;

my @F;
my $L;

my $bnd=500;
my $skip=0;
my $subskip=1;
my $ssc=0;
while(<IN>)
{
	chomp;
	my @a = split /\t/, $_;
	
	pop @a while scalar @a>15;
	
	foreach(0..scalar @a -1)
	{
		$a[$_]= $bnd if $a[$_]> $bnd;
		$a[$_]=-$bnd if $a[$_]<-$bnd;
		
		if($a[$_])
		{
			$L->{$_}->{'sum'}+=$a[$_];
			$L->{$_}->{'cnt'}++;
		}
	}
	
	next if $skip-- > 0;
	
	if($ssc < $subskip)
	{
		$ssc++;
		next;
	}
	$ssc=0;
	
	push @F, \@a;
}
close IN;

open IN, '>'.$ARGV[0]."1";

foreach(@F)
{
	print IN join("\t", @$_);
	print IN "\n";
}
close IN;

open IN, '>'.$ARGV[0]."2";

my $tots=0;
my $tota=0;
foreach(sort {$a<=>$b} keys %$L)
{
	$tots += $L->{$_}->{sum}/$L->{$_}->{cnt};
	$tota ++;
	
	print IN $L->{$_}->{sum}/$L->{$_}->{cnt};
	print IN "\n";
}
close IN;

if($tota)
{
	print $tots/$tota;
}
else
{
	print 0;
}
	
print "\n";