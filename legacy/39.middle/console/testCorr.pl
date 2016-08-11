use Corr;
use Data::Dumper;

my $corr = Corr->new;

$corr->push(1,1);
$corr->push(3,1.5);
$corr->push(4,1);
$corr->push(0.5,0.5);

print Dumper($corr->eval());
