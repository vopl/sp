
stacksize('max');

clf;
w = 5925;
tail = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.7\console\tail",-1,w);

h = size(tail,1);

plot(tail(1,:),'b');

hh=3;

if hh<=h then
  plot(tail(hh,:),'r');
end;

