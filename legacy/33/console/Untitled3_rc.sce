stacksize('max');


rw = 50;
rh=-1;
rt  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\response_t", rh, rw);
rre  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\response_re", rh, rw);
rim  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\response_im", rh, rw);

h = size(rt,1);

tMult = 1 / (60*60);

clf;
f = gcf();
f.color_map = graycolormap(512);

t=rt;
s = sqrt(rre.^2 + rim.^2);
//s = cos(atan(rim, rre));
//s = abs(rre);

x =1:size(s,1);
y = t(1,:)*tMult;
//y = 1:size(t,2);
grayplot(x, y, s);



