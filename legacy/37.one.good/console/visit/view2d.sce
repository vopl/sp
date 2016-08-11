stacksize('max');
t = read("t", -1, 1);
tamount = size(t,1)*0.55;
t = t(1:tamount);
tamount = size(t,1);

x = read("x", -1, 1);
xamount = size(x,1);

re=read("re", xamount, tamount);
im=read("im", xamount, tamount);

v = re+%i*im;

clf;

f = gcf();
f.color_map = graycolormap(512);

xx = 1:xamount;
tt = 1:tamount;
xx = (x ./ (60*60));
tt = (t ./ (60*60));
grayplot((xx), (tt), abs((v)) );

//plot(tt,(abs(v(30,:))));
//plot(tt,(abs(v(31,:))));
//plot(tt,(abs(v(32,:))));

