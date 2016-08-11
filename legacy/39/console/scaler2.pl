use strict;
use warnings;
use Data::Dumper;
use POSIX;

if(@ARGV < 2)
{
	print "use ".$ARGV[0]." n out\n";
	exit;
}
my $fileIn = $ARGV[0];
my $fileOut = $ARGV[1];

open IN, '<'.$fileIn || die $!;
open OUT, '>'.$fileOut || die $!;




my @names;


my $data={};


while(<IN>)
{
	chomp;
	my @lineArr = split "\t", $_;
	
	if(!scalar @names)
	{
		@names = @lineArr;
		next;
	}
	

	foreach(0..scalar @names-1)
	{
		push(@{$data->{$names[$_]}}, $lineArr[$_]);
	}
}





foreach(@names)
{
	$data->{$_} = normalizeDistrib($data->{$_});
	print OUT "$_\t";
}
print OUT "\n";

while(1)
{
	my $vals=[];
	$vals->[0] = pop(@{$data->{$names[0]}});
	last unless defined $vals->[0];
	
	foreach(1..scalar @names-1)
	{
		$vals->[$_] = pop @{$data->{$names[$_]}};
	}
	

	print OUT join "\t", @$vals;
	print OUT "\n";
}
exit;


















sub normalizeDistrib($)
{
	my @data = @{$_[0]};
	my @sdata = sort {$a <=> $b} @data;


	my $maxx = scalar(@sdata)-1;
	
	my $miny = $sdata[0];
	my $maxy = $sdata[$maxx];
	
	return $_[0] if $maxy == $miny;

	my $b = $miny;
	my $k = ($maxy - $b)/$maxx;


	my @out;
	foreach my $v (@data)
	{
		my $pred = sub 
		{
			my ($a, $b) = @_;
			$a <=> $b;
		};
		
		my $pos = binary_search(\@sdata, $v, $pred);
		
		push @out, (($k*$pos+$b)-$miny)/($maxy-$miny)*2-1;
	}
	
	return \@out;
}

sub binary_search($$&)
{
  my $arr = shift;
  my $value = shift;
  my $cmpf = shift || sub {shift cmp shift};

  my $left = 0;
  my $right = @$arr - 1;
  while ($left <= $right)
  {
    my $mid = int($right + $left) >> 1;

    my $c = &$cmpf($arr->[$mid], $value);
    if ($c == 0)
	{
      return $mid;
    }
    if ($c > 0)
	{
      $right = $mid - 1
    }
    else
	{
      $left  = $mid + 1
    }
  }
 return -1;
};
