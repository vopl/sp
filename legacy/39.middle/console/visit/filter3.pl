#use strict;
$| = 1;

my $xStart = undef;
my $xLast = undef;
my $cnt=-1;
my $step=6;

my $minPeriod = 10*60;
my $maxPeriod = 4*4*5*24*60*60;

my $dir = "../out_small_19/response/";









my $m_t=[];
my $m_tIdx=0;

#########
open(OUT, ">s.3d") or die $!;
print OUT "time2\tperiod2\tsmotheda2\tx\n";
open(OUTM_T, ">t") or die $!;
open(OUTM_X, ">x") or die $!;
open(OUTM_RE, ">re") or die $!;
open(OUTM_IM, ">im") or die $!;




#my $smothPeriods = 2;



use Data::Dumper;

my $smothedPeriods = {};
sub doSmoth($)
{
	my ($t,$p,$r,$i) = @{$_[0]};
	#warn Dumper([$t,$p,$r,$i]);
	
	my $am = sqrt($r*$r + $i*$i);
	push @{$smothedPeriods->{$p}}, $am;
	
	my $amount = 3;#int($p/10)||1;
	
	my $arr;

	while($amount < @{$smothedPeriods->{$p}})
	{
		shift @{$smothedPeriods->{$p}};
	}

	if(@{$smothedPeriods->{$p}} < $amount)
	{
		return 0;
	}

	$arr = $smothedPeriods->{$p};
	#$arr = [sort {$a <=> $b} @$arr];
	#return $arr->[int(@$arr/2)] || 0;
	
	my $s = 0;
	$s+=$_ foreach @$arr;
	return $s/@$arr/ $p;
}

opendir(my $D, $dir);
while(my $fn=readdir($D))
{
	next if $fn eq '.' or $fn eq '..';
	warn $fn;
	mkOne($dir.$fn, 4);
}
closedir($D);

foreach(@$m_t)
{
	print OUTM_T $_,"\n";
}
exit;

close OUT;
close OUTM_T;
close OUTM_X;
close OUTM_RE;
close OUTM_IM;


sub mkOne($$)
{
open(INF, "<".shift) or die $!;

#my $key = shift;
my $prev2arr = undef;
while(<INF>)
{
	my $line = $_;
	chomp;
	my $arr = [split /\s/];

	next if $arr->[1] < $minPeriod;
	next if $arr->[1] > $maxPeriod;
	$xStart = $arr->[0] unless defined $xStart;

	if($xLast != $arr->[0])
	{
		print OUTM_X $xLast-$xStart,"\n" if $m_tIdx;
		print OUTM_RE "\n" if $m_tIdx;
		print OUTM_IM "\n" if $m_tIdx;

		$cnt++;
		$xLast = $arr->[0];
		$m_tIdx=0;
		$prev2arr = undef;
	}
	next if $cnt % $step;

	if(!$cnt)
	{
		push @$m_t, $arr->[1];
		$m_tIdx++;
	}
	else
	{
		next if $m_tIdx >= @$m_t;
		$m_tIdx++;
	}


	$arr->[0] -= $xStart;





	my $out = [$arr->[0], $arr->[1], $arr->[2], $arr->[3]];
	my $smothedA = doSmoth($out);

	if($smothedA)
	{
		print OUT join("\t", ($arr->[0], $arr->[1], $smothedA, 0));
		print OUT "\n";
	}

	print OUTM_RE $arr->[2],"\t";
	print OUTM_IM $arr->[3],"\t";
	
	

}

print "$cnt\n";
close INF;

}