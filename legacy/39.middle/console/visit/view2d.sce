stacksize('max');
t = read("t", -1, 1);
tamount = size(t,1);
t = t(1:tamount);
tamount = size(t,1);

x = read("x", -1, 1);
xamount = size(x,1);

re=read("re", xamount, tamount);
im=read("im", xamount, tamount);

v = re+%i*im;
p = atan(v);

xx = 1:xamount;
tt = 1:tamount;
xx = (x ./ (60*60));
tt = (t ./ (60*60));



clf;

//plot(tt, abs(v(1,:)),'k');
//plot(tt, abs(v(1,:)),'b.');
//abort;

f = gcf();
f.color_map = graycolormap(512);


//hr = imag(hilbert(real(v)));
//hi = -imag(hilbert(imag(v)));
//h = hi+%i*hr;


grayplot((xx), (tt), abs(v) );
abort;

//plot((tt),sum(abs(real(v)), 'r'),'k');
//plot((tt),sum(abs(imag(v)), 'r'),'b');
//plot((tt),sum(abs( abs(v)), 'r'),'r');
//abort;

plot(real(v(10,:)),'k');
plot(imag(v(10,:)),'b');
plot(real(v(10,:))+real(h(10,:)),'r');
plot(imag(v(10,:))+imag(h(10,:)),'m');
abort
plot(-real(h(1,:)),'r');
plot(-imag(h(1,:)),'m');

//plot(xx, abs((v(:,740))),'r');

//vr = v_ - v_10;

