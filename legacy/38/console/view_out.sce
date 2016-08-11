stacksize('max');

h = 9;
w = -1;

out = read("out", w, h);

clf;

t = out(:,1);
rr = out(:,2);
ri = out(:,3);
ir = out(:,4);
ii = out(:,5);

plot(t,rr,'b');
plot(t,ri,'b');
//plot(t,ir,'r');
//plot(t,ii,'m');
abort;

