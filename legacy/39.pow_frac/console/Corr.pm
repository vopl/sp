package Corr;
use strict;
use Data::Dumper;

sub new
{
	return bless 
	{
		EX=>0,
		EY=>0,
		EXY=>0,
		EX2=>0,
		EY2=>0,
		EXminusY=>0,
		EXminusY2=>0,
		amount=>0,
	};
}

sub push
{
	my ($this, $X, $Y) = @_;
	
	$this->{'EX'} += $X;
	$this->{'EY'} += $Y;
	$this->{'EXY'} += $X * $Y;
	$this->{'EX2'} += $X * $X;
	$this->{'EY2'} += $Y * $Y;
	$this->{'EXminusY'} += $X - $Y;
	$this->{'EXminusY2'} += ($X - $Y)*($X - $Y);
	$this->{'amount'}++;
}

sub getAmount
{
	my ($this) = @_;
	return $this->{'amount'};
}

sub eval
{
	my ($this) = @_;
	
	my $amount = $this->{'amount'};
	return undef if $amount<2;
	my $EX = $this->{'EX'} / $amount;
	my $EY = $this->{'EY'} / $amount;
	my $EXY = $this->{'EXY'} / $amount;
	my $EX2 = $this->{'EX2'} / $amount;
	my $EY2 = $this->{'EY2'} / $amount;
	my $EXminusY = $this->{'EXminusY'} / $amount;
	my $EXminusY2 = $this->{'EXminusY2'} / $amount;

	###### Коэффициент корреляции
	my $corr;
	my $d1 = $EX2-$EX*$EX;
	my $d2 = $EY2-$EY*$EY;
	my $d = sqrt($d1) * sqrt($d2);
	if($d)
	{
		$corr = ($EXY-$EX*$EY)/$d;
	}
	else
	{
		$corr = 0;
	}
	
	######## абсолютное среднее значение погрешности
	my $meanDev = $EXminusY;
	
	######## Среднеквадратическая погрешность
	my $stdDev = sqrt($EXminusY2);
	
	######## Коэффициент несоответствия
	my $disparity = 0;
	$disparity = sqrt($EXminusY2/$EY2) if $EY2;
	
	######## составляющая дисперсии
	my $dispersion = $d;
	
	######## Коэффициент расхождения
	my $discrepancy = 0;
	$discrepancy = sqrt($EXminusY2) / $dispersion if $dispersion;
	
	
	
	return 
	{
		'amount'=>$amount,
		'corr'=>$corr,
		'meanDev'=>$meanDev,
		'stdDev'=>$stdDev,
		'disparity'=>$disparity,
		'dispersion'=>$dispersion,
		'dispersionX'=>$d1,
		'dispersionY'=>$d2,
		'discrepancy'=>$discrepancy,
	};
}

1;