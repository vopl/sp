stacksize('max');
w=20;
h = -1;
ma = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\ma",h,w);
mp = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\mp",h,w);
t = read("P:\finance\spectrumSeparator\nhel.Composite\31\console\mt",h,1);

me = ma .*(cos(mp) + %i*sin(mp));

x = t;
y = 1:w;
y = (y-1)/w*2*%pi;

clf;
plot(y, real(me(1173,:)),'k');
plot(y+2*%pi, real(me(1173,:)),'k');
plot(y+4*%pi, real(me(1173,:)),'k');
//abort;
//plot(t, real(me(:,2)),'k');
//plot(t, real(me(:,3)),'k');
//plot(t, real(me(:,4)),'k');

//abort
e = abs(me);
clf;

f = gcf();
f.color_map = graycolormap(512);
colorbar(min(e), max(e));
grayplot(x,y,    e  );

