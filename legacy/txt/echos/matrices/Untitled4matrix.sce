w=50;
ma = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\matrixA",-1,w);
mp = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\matrixP",-1,w);
t = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\T",-1,1);

me = ma .*(cos(mp) + %i*sin(mp));

x = t;
y = 1:w;
y = (y-1)/w*2*%pi;
//e = (atan(imag(me), real(me)));
e = abs(me);
clf;
f = gcf();
f.color_map = graycolormap(512);
colorbar(min(e), max(e));
grayplot(x,y,    e  );

clf;
//plot(t, imag(me),'k');
plot(t, abs(me),'b');

