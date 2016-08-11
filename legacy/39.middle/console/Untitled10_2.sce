stacksize('max');

clf
w=1000;
//sv = read("sv", w, w);
//su = read("su", w, w);
//ss = read("ss", w, 1);
//sv = sv';
c = su+%i*sv;

clf;
x=1:w;
y=1:w;
f=gcf();
f.color_map=jetcolormap(512);
//grayplot(x,y,abs(c(1:w,1:w)));
plot(((imag(c(1500,:)))));
abort;
//plot(ss);
plot(abs(c));


