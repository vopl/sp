
tap = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.onlyDsp.2\console\tap", -1, 3);

t = tap(1:200,1);

function y=mn(n)
  an =  tap(200*n+1:200*n+200,2);
  pn =  tap(200*n+1:200*n+200,3);
  y = an.*(cos(pn) + %i*sin(pn));
endfunction;

n = 1;

clf;

plot(real(mn(0)),'k');
plot(real(-mn(0)+mn(1))*14,'b');


//plot(real(mn(0)),'b');
//plot(real(mn(1)),'b');
//plot(real(mn(2)),'b');
//plot(real(mn(2)),'b');
//plot(real(mn(3)),'b');
//plot(real(mn(4)),'b');
//plot(real(mn(5)),'b');
//plot(real(mn(6)),'b');
//plot(real(mn(7)),'b');
//plot(real(mn(8)),'b');
//plot(real(mn(9)),'b');
//plot(real(mn(10)),'b');
abort;


x=1:100000;
x = x/100;
y1 = zeros(1,100000);
y2 = zeros(1,100000);

v1 = mn(0);
v2 = mn(8);
for i=1:200 do
  y1 = y1+abs(v1(i)).*cos(2*%pi*(x-80)/t(i) + atan(imag(v1(i)), real(v2(i))));
  y2 = y2+abs(v2(i)).*cos(2*%pi*(x-80)/t(i) + atan(imag(v2(i)), real(v2(i))));
end;
clf;
plot(x,y1-y2,'r');


