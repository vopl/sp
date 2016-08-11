stacksize('max');
w = 60*24*30;
h = -1;
t  = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\out_small_10\out_time", h, w);
t = t/60/60-336200;
e  = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\out_small_10\out_extrap", h, w);
o  = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\out_small_10\out_original", h, w);

clf;

pos = 34;

plot(t(pos,:),o(pos,:),'k');

bx=[t(pos,1),t(pos,w/2),t(pos,w/2),t(pos,w)];
mn = min(o(pos,:));
mx = max(o(pos,:));
by=[mn,mn,mx,mx];

plot(bx, by,'g');

eo = mean(o(pos,:)) - mean(e(pos,:));
//eo = o(pos,w/2) - e(pos,w/2);
plot(t(pos,:),e(pos,:)+eo,'b');

plot(t(1,:),e(1,:)+eo,'r');

