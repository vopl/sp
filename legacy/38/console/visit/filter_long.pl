use strict;
use Data::Dumper;
use Statistics::Basic qw(:all);
use IO::Handle;
 
$|=1;
my $dir = "../out_small_50/spectr/";

my $minT = 1.5*60*60;
my $maxT = 4000*60*60;

my $tArrLen = 0.2/60; #количество отсчетов к статистике будет <=$t*$tArrLen
my $xLen = 60*60*1; #интервал дампирования 
my $vals={};# аккумулятор для статистики
my $lastx=undef;
my $lastxStep=undef;
my $skipAmount=0;

my $ts=[];
my $tsw=0;
sub mkOne($$)
{
	open(INF, "<".shift) or die $!;
	
	while(<INF>)
	{
		my $line = $_;
		chomp;
		my $arr = [split /\s/];
		
		next if @$arr != 4;
		
		my $x = $arr->[0];
		my $t = $arr->[1];
		
		next if $t<$minT || $t>$maxT;
		
		my $r = $arr->[2];
		my $i = $arr->[3];
		
		$lastx = $x unless defined $lastx;
		$lastxStep = $x unless defined $lastxStep;
		
		if($x - $lastx > $xLen)
		{
			if(--$skipAmount < 0)
			{
				my @ts = sort {$a<=>$b;} keys %$vals;
				foreach my $t (@ts)
				{
					my @vals = sort {$a<=>$b} @{$vals->{$t}};
					
					my $bnd = int(@vals*0.1) || 1;
					# my $avg = $vals[@vals/2];
					# my $var = $vals[-$bnd]-$vals[$bnd];
					
					@vals = @vals[$bnd..@vals-1-$bnd];
					#my $avg = mean(\@vals);
					
					my $avg;
					my $var;
					
					if(@vals > 1)
					{
						$avg = lsf([0..@vals-1], \@vals)->y_given_x( @vals/2 );
						$var = 0;#stddev($avg->query_vector);
					}
					else
					{
						$avg = 0;
						$var = 0;#stddev($avg->query_vector);
					}
					
					
					if($t*$tArrLen-1 > @{$vals->{$t}})
					{
						#$avg=0;
					}
					
					printf(OUT "%.16g\t%.16g\t%.16g\t%.16g\n", $x,$t,$avg,$var);
					
					printf(OUTM_M "%.16g\t", $avg);
					printf(OUTM_V "%.16g\t", $var);
					
					if(0==$tsw)
					{
						push @$ts, $t;
					}
				}
				printf(OUTM_M "\n");
				printf(OUTM_V "\n");

				printf(OUTM_X "%.16g\n", $x);
				
				if(0==$tsw)
				{
					foreach my $t (@$ts)
					{
						printf(OUTM_T "%.16g\n", $t);
					}
					$tsw = 2;
				}
			}			
			$lastx = $x;
		}
		
		my $amp = sqrt($r*$r + $i*$i);
		
		$vals->{$t} = [] unless $vals->{$t};
		
		push(@{$vals->{$t}}, $amp);
		
		##################
		#производная
		if($lastxStep != $x)
		{
			my $xIdx = scalar @{$vals->{$t}}-2;
			my @ts = sort {$a<=>$b;} keys %$vals;
			
			# вытащить
			my @vs;
			foreach my $t (@ts)
			{
				push @vs, $vals->{$t}->[$xIdx];
			}
			
			#преобразовать
			my @vs2 = @vs;
			my $range=2;
			$vs2[$_] = 0 foreach (0..$range-1);
			$vs2[$_] = 0 foreach (@vs2-$range..@vs2-1);
			
			foreach my $idx ($range..@vs2-1-$range)
			{
				my @loc = @vs[$idx-$range..$idx+$range];
				@loc = sort {$a<=>$b;} @loc;
				#warn Dumper([$vs2[$idx], $vs[$idx] - mean(\@loc)]);
				#$vs2[$idx] = $vs[$idx] - mean(\@loc);
				
				$vs2[$idx] = abs($vs[$idx] - lsf([0..@loc-1], \@loc)->y_given_x( @loc/2 ));
			}
			
			#положить обратно
			foreach my $t (@ts)
			{
				$vals->{$t}->[$xIdx] = shift @vs2;
			}
			#die Dumper($vals);
		
			$lastxStep = $x;
		}
		#производная
		##################
		shift @{$vals->{$t}} while scalar(@{$vals->{$t}}) > $t*$tArrLen;
}
	
	close INF;

}















###########################################################
open(OUT, ">s.3d") or die $!; OUT->autoflush(1);
print OUT "time2\tperiod2\treal2\timag2\n";
open(OUTM_T, ">t") or die $!; OUTM_T->autoflush(1);
open(OUTM_X, ">x") or die $!; OUTM_X->autoflush(1);
open(OUTM_M, ">m") or die $!; OUTM_M->autoflush(1);
open(OUTM_V, ">v") or die $!; OUTM_V->autoflush(1);

opendir(my $D, $dir);
while(my $fn=readdir($D))
{
	next if $fn eq '.' or $fn eq '..';
	warn $fn;
	mkOne($dir.$fn, 4);
}
closedir($D);


close OUT;
close OUTM_T;
close OUTM_X;
close OUTM_M;
close OUTM_V;
exit;
