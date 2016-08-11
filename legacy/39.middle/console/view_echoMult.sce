stacksize('max');

w = 20;
h = 2000;
tm  = read("mt", h, 1);
am  = read("ma", h, w);
pm  = read("mp", h, w);
vm = am .* (cos(pm) + %i*sin(pm));


clf;

ph=1:1;
mult = 1;
plot(tm,real(vm(:,ph)*mult),'g');
plot(tm,imag(vm(:,ph)*mult),'y');
plot(tm,abs(vm(:,ph)*mult),'m');


ph=2:2;
mult = cos(pm(2)) + %i*sin(pm(2));

plot(tm,real(vm(:,ph)*mult),'b');
plot(tm,imag(vm(:,ph)*mult),'k');
plot(tm,abs(vm(:,ph)*mult),'r');
//plot(tm,atan(imag(vm(:,ph)), real(vm(:,ph))),'r');

