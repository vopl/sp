stacksize('max');

w = 4;
h = -1;
tm  = read("mt", h, 1);
am  = read("ma", h, w);
pm  = read("mp", h, w);
vm = am .* (cos(pm) + %i*sin(pm));
h = size(vm,1);


tm1  = read("mt_1", h, 1);
am1  = read("ma_1", h, w);
pm1  = read("mp_1", h, w);
vm1 = am1 .* (cos(pm1) + %i*sin(pm1));

clf;

ph=1:4;
plot(tm,real(vm(:,ph)-vm1(:,ph)),'b');
plot(tm,imag(vm(:,ph)-vm1(:,ph)),'k');
//plot(tm,abs(vm(:,ph)-vm1(:,ph)),'r');
abort;

//plot(tm1,real(vm1(:,ph)),'g');
//plot(tm1,imag(vm1(:,ph)),'b');
//plot(tm1,abs(vm1(:,ph)),'m');
//abort
//vm = vm;
mult = 1;

plot(tm,real(vm(:,ph)*mult),'b');
plot(tm,imag(vm(:,ph)*mult),'k');
plot((tm),abs(vm(:,ph)*mult),'r');
//plot(tm,atan(imag(vm(:,ph)), real(vm(:,ph))),'r');

