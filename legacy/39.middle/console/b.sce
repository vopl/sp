stacksize('max');


w = 60*24*30*6;
time = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.8\console\outTime",-1,w);
orig = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.8\console\outOrig",-1,w);
extrap = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.8\console\outExtrap",-1,w);

clf

pos = 50;
timeMult = 1/60/60/24;

mn = min(orig(pos,:));
mx = max(orig(pos,:));
bx=[time(pos,1),time(pos,w/2),time(pos,w/2),time(pos,w)];
by=[mn,mn,mx,mx];

plot(bx*timeMult, by,'r');
plot(time(pos,:)*timeMult, orig(pos,:),'k');
plot(time(pos,:)*timeMult, extrap(pos,:)-extrap(pos,w/2)+orig(pos,w/2),'b');


