stacksize('max');
t = read("t", -1, 1);
tamount = size(t,1)*1;
t = t(1:tamount);
tamount = size(t,1);

x = read("x", -1, 1);
xamount = size(x,1);

re=read("re", xamount, tamount);
im=read("im", xamount, tamount);

v = re+%i*im;

xx = 1:xamount;
tt = 1:tamount;
xx = (x ./ (60*60));
tt = (t ./ (60*60));



clf;

f = gcf();
f.color_map = graycolormap(512);

grayplot((xx), (tt), ((im)) );
abort;

plot((tt),sum(abs(real(v)), 'r'),'k');
plot((tt),sum(abs(imag(v)), 'r'),'b');
plot((tt),sum(abs( abs(v)), 'r'),'r');
abort;

plot(tt,    abs((v(6,:))),'r');
plot(tt,    abs((v(7,:))),'r');

//plot(xx, abs((v(:,740))),'r');

//vr = v_ - v_10;

