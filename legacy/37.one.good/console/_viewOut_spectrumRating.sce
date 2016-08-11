stacksize('max');
w = 6000;
//w = 1000;

h=-1;
t  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\out_spectrumRatingPeriod", h, w);
re  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\out_spectrumRatingRe", h, w);
im  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.goertzel.5\console\out_spectrumRatingIm", h, w);

tMult = 1 / (60*60);
clf;

clf;
f = gcf();
f.color_map = graycolormap(512);

//s = (im-re);
s = (im.^2+re.^2).^0.125;
//s = sign(re) .* s;
//s = re;
//s = (atan(im, re));

x =1:size(s,1);
x = x;
y = t(1,:)*tMult;

//s = sqrt(re.^2+im.^2);
grayplot(x,y, s);
//plot(y, sum(s,'row'));

