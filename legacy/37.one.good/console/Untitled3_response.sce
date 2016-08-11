stacksize('max');

r1  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.10.test3cos\console\response", -1, 3);

clf;

//plot(r1(:,1),sqrt(r1(:,2).^2 + r1(:,3).^2),'m');



c1  = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.10.test3cos\console\convolution", -1, 3);

plot(c1(:,1),sqrt(c1(:,2).^2 + c1(:,3).^2),'r');
plot(c1(:,1),c1(:,2),'k');
plot(c1(:,1),c1(:,3),'b');


