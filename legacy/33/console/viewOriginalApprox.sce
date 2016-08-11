stacksize('max');

w = 1;
tm  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\mt", -1, 1);
am  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\ma", -1, w);
pm  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\mp", -1, w);
vm = am .* (cos(pm) + %i*sin(pm));


tm1  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\mt_1", -1, 1);
am1  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\ma_1", -1, w);
pm1  = read("P:\finance\spectrumSeparator\nhel.Composite\33\console\mp_1", -1, w);
vm1 = am1 .* (cos(pm1) + %i*sin(pm1));


clf;

ph=1:1;
//plot(tm1,real(vm1(:,ph)),'g');
//plot(tm1,imag(vm1(:,ph)),'y');
//plot(tm1,abs(vm1(:,ph)),'m');

//vm = vm;
mult = 1;
plot(tm,real(vm(:,ph)*mult),'b');
plot(tm,imag(vm(:,ph)*mult),'k');
plot(tm,abs(vm(:,ph)*mult),'r');
//plot(tm,atan(imag(vm(:,ph)), real(vm(:,ph))),'r');
abort;

clf
plot(tm,real(vm(:,ph)-vm1(:,ph)),'b');
plot(tm,imag(vm(:,ph)-vm1(:,ph)),'k');
plot(tm,abs(vm(:,ph)-vm1(:,ph)),'r');

