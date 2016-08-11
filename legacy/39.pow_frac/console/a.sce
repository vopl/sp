stacksize('max');


tsa = read("P:\finance\spectrumSeparator\nhel.Composite\13\console\spectr",-1,3);

clf
plot(tsa(:,1), tsa(:,2),'g');
plot(tsa(:,1), tsa(:,3),'r');


